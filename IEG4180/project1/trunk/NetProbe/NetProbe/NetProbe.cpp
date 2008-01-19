// NetProbe.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "NetProbe.h"
#include "NetProbeDlg.h"
#include <string.h>
#include <winsock2.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif






UINT __cdecl TCPStart(LPVOID pParam);








// CNetProbeApp

BEGIN_MESSAGE_MAP(CNetProbeApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CNetProbeApp construction

CNetProbeApp::CNetProbeApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CNetProbeApp object

CNetProbeApp theApp;

NetProbe theProbe;


// CNetProbeApp initialization

BOOL CNetProbeApp::InitInstance()
{

	WSADATA wsaData;
	int iResult;

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));


	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(iResult != 0){
		printf("WSAStartup failed: %d\n", iResult);
		return FALSE;
	}



	CNetProbeDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	WSACleanup();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}



void NetProbe::setRefreshInterval(int t){
	refreshInterval = t;
}

void NetProbe::setPacketSize(int n){
	packetSize = n;
}


void NetProbe::setSendingRate(int n){
	sendingRate = n;
}
	
void NetProbe::setNumPackets(int n){
	numPackets = n;
}
	
void NetProbe::setProtocol(int p){
	protocol = p;
}


void NetProbe::setLocal(char *h){
	if(local != NULL)
		free(local);
	local=(char *)malloc(sizeof(char)*(strlen(h)+1));
	strcpy(local, h);
}

void NetProbe::setRemote(char *h){
	if(remote != NULL)
		free(remote);
	remote=(char *)malloc(sizeof(char)*(strlen(h)+1));
	strcpy(remote, h);
}

void NetProbe::setLocalPort(int n){
	localPort = n;
}

void NetProbe::setRemotePort(int n){
	remotePort = n;
}

int NetProbe::getStatus(void){
	return status;
}

void NetProbe::stop(void){
	status = 0;
}


BOOL NetProbe::startReceive(void){

	if(this->status)
		return false;
	if(this->protocol == 0){
		MessageBox(NULL,"Please choose a Protocol.","Error",0);
		return false;
	}
	if(this->protocol == 1){
		AFX_THREADPROC pfn = TCPStart;
		AfxBeginThread(pfn, this);
		return true;
	}
}


char *NetProbe::getLocal(void){
	return local;
}

char *NetProbe::getRemote(void){
	return remote;
}

int NetProbe::getLocalPort(void){
	return localPort;
}

int NetProbe::getRemotePort(void){
	return remotePort;
}

void NetProbe::setStatus(int n){
	status = n;
}

int NetProbe::getPacketSize(void){
	return packetSize;
}


UINT __cdecl TCPStart(LPVOID pParam){
	NetProbe *theProbe = (NetProbe *)pParam;
	struct addrinfo aiHints;
	struct addrinfo *aiList = NULL;
	int retVal;

	struct sockaddr_in *TCP_Addr;
	struct sockaddr_in *TCP_PeerAddr;
	TCP_Addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	TCP_PeerAddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	memset(TCP_Addr, 0, sizeof(struct sockaddr_in));
	memset(TCP_PeerAddr, 0, sizeof(struct sockaddr_in));


	memset(&aiHints,0,sizeof(aiHints));
	aiHints.ai_family = AF_INET;
	aiHints.ai_socktype = SOCK_STREAM;
	aiHints.ai_protocol = IPPROTO_TCP;
	if((retVal = getaddrinfo(theProbe->getLocal(), NULL, &aiHints, &aiList))!=0){
		return false;
	}


	TCP_Addr->sin_family = AF_INET;
	TCP_Addr->sin_port = htons(theProbe->getLocalPort());
	memcpy(&(TCP_Addr->sin_addr),(aiList->ai_addr->sa_data+2),4);

	theProbe->timer.Start();
	theProbe->setStatus(1);

	SOCKET Sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bind(Sockfd, (struct sockaddr *)TCP_Addr, sizeof(struct sockaddr_in));
	listen(Sockfd, 5);
	int addrlen = sizeof(struct sockaddr_in);
	SOCKET newsfd = accept(Sockfd, (struct sockaddr *)TCP_PeerAddr, &addrlen);
	closesocket(Sockfd);

	int len = theProbe->getPacketSize();
	if(len == 0){
		MessageBox(NULL, "Please input a PacketSize.", "Error", 0);
		AfxEndThread(-1);
	}
	char *buf = (char *)malloc(sizeof(char)*len);

	while(true){
		recv(newsfd, buf, len, MSG_PEEK);
		printf("%d s:%d Bytes received.\n", len, theProbe->timer.ElapseduSec());
		if(theProbe->getStatus() == 0){
			closesocket(newsfd);
			break;
		}
	}

	AfxEndThread(0);
}

