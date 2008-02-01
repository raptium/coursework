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







UINT __cdecl TCPStartRecv(LPVOID pParam);
UINT __cdecl UDPStartRecv(LPVOID pParam);
UINT __cdecl TCPStartSend(LPVOID pParam);
UINT __cdecl UDPStartSend(LPVOID pParam);
void errorMessageBox(void);

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



NetProbe::NetProbe(){

	status = 0;
	local = NULL;
	remote = NULL;
	localPort = 0;
	remotePort = 0;
	refreshInterval = 100;
	packetSize = 0;
	sendingRate = 0;
	numPackets = 0;
	packetTransferred = 0;
	protocol = 0;
	maxPacketNum = 0;
	byteTransferred = 0;
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

void NetProbe::stop(){
	status = 0;
	numPackets = 0;
	packetTransferred = 0;
	maxPacketNum = 0;
	byteTransferred = 0;
}


BOOL NetProbe::startReceive(void){

	if(this->status)
		return false;
	if(this->protocol == 0){
		MessageBox(NULL,"Please choose a Protocol.","Error",0);
		return false;
	}
	if(this->protocol == 1){
		AFX_THREADPROC pfn = TCPStartRecv;
		AfxBeginThread(pfn, this);
		return true;
	}else if(this->protocol == 2){
		AFX_THREADPROC pfn = UDPStartRecv;
		AfxBeginThread(pfn, this);
		return true;
	}
}

BOOL NetProbe::startSend(void){

	if(this->status)
		return false;
	if(this->protocol == 0){
		MessageBox(NULL,"Please choose a Protocol.","Error",0);
		return false;
	}
	if(this->protocol == 1){
		AFX_THREADPROC pfn = TCPStartSend;
		AfxBeginThread(pfn, this);
		return true;
	}else if(this->protocol == 2){
		AFX_THREADPROC pfn = UDPStartSend;
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

int NetProbe::getInterval(void){
	return refreshInterval;
}


void NetProbe::packetTransfer(int n){
	if(n > maxPacketNum)
		maxPacketNum = n + 1;
	packetTransferred++;
	if(n < 0)
		maxPacketNum = packetTransferred;
}

int NetProbe::getMaxNum(void){
	return maxPacketNum - 1;
}

int NetProbe::getPacketTransfer(void){
	return packetTransferred;
}

int NetProbe::getPacketLost(void){
	return maxPacketNum - packetTransferred;
}

double NetProbe::getPacketLoss(void){
	return 1 - (double)packetTransferred / maxPacketNum;
}


double NetProbe::getByteTransfer(void){
	return byteTransferred;
}

void NetProbe::byteTransfer(int n){
	this->byteTransferred += n;
}

int NetProbe::getTransferRate(void){
	return this->sendingRate;
}

void errorMessageBox(void){
	int e = WSAGetLastError();
	LPSTR pBuf = NULL;
	FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER |FORMAT_MESSAGE_IGNORE_INSERTS |FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		e,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
		(LPSTR)&pBuf,
		0 ,
		NULL);
	theProbe.stop();
	switch(e){
		case 0:
			AfxMessageBox(pBuf, MB_ICONINFORMATION);
			break;
		default:
			AfxMessageBox(pBuf, MB_ICONERROR);
	}
}



UINT __cdecl TCPStartSend(LPVOID pParam){
	//NetProbe *theProbe = (NetProbe *)pParam;
	struct addrinfo aiHints;
	struct addrinfo *aiList = NULL;
	int retVal;
	int flag = 0;
	long sp;


	struct sockaddr_in *TCP_PeerAddr;
	TCP_PeerAddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	memset(TCP_PeerAddr, 0, sizeof(struct sockaddr_in));


	memset(&aiHints,0,sizeof(aiHints));
	aiHints.ai_family = AF_INET;
	aiHints.ai_socktype = SOCK_STREAM;
	aiHints.ai_protocol = IPPROTO_TCP;
	if((retVal = getaddrinfo(theProbe.getRemote(), NULL, &aiHints, &aiList))!=0){
		theProbe.stop();
		errorMessageBox();
		AfxEndThread(0);
	}


	TCP_PeerAddr->sin_family = AF_INET;
	TCP_PeerAddr->sin_port = htons(theProbe.getRemotePort());
	memcpy(&(TCP_PeerAddr->sin_addr),(aiList->ai_addr->sa_data+2),4);

	theProbe.timer.Start();
	theProbe.setStatus(3);

	SOCKET Sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(Sockfd == INVALID_SOCKET){
		theProbe.stop();
		errorMessageBox();
		AfxEndThread(0);
	}

	int retval = connect(Sockfd, (struct sockaddr *)TCP_PeerAddr, sizeof(struct sockaddr_in));
	if (retval == SOCKET_ERROR){
		theProbe.stop();
		errorMessageBox();
		AfxEndThread(0);
	}

	char *buf = (char *)malloc(sizeof(char)*4096);
	memset(buf, 0, sizeof(buf));

	int len = theProbe.getPacketSize();

	while(true){
		int ret;
		int no;
		
		if(theProbe.getTransferRate())
			if(flag && (theProbe.getByteTransfer()  / ((theProbe.timer.Elapsed() - sp) / 1000.0)) > theProbe.getTransferRate()){
				Sleep(10);
				continue;
			}

		ret = send(Sockfd, buf, len, 0);
		if(ret == SOCKET_ERROR){
			theProbe.stop();
			closesocket(Sockfd);
			errorMessageBox();
			break;
		}

		theProbe.packetTransfer(-1);
		theProbe.byteTransfer(ret);
		
		if(!flag && theProbe.getByteTransfer() != 0){
			sp = theProbe.timer.Elapsed();
			flag = 1;
		}

		if(theProbe.getStatus() == 0){
			closesocket(Sockfd);
			break;
		}
	}

	AfxEndThread(0);
	return 0;
}

UINT __cdecl UDPStartSend(LPVOID pParam){
	struct addrinfo aiHints;
	struct addrinfo *aiList = NULL;
	int retVal;
	int flag = 0;
	long sp;
	long seqN;

	struct sockaddr_in *UDP_PeerAddr;
	UDP_PeerAddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	memset(UDP_PeerAddr, 0, sizeof(struct sockaddr_in));


	memset(&aiHints,0,sizeof(aiHints));
	aiHints.ai_family = AF_INET;
	aiHints.ai_socktype = SOCK_STREAM;
	aiHints.ai_protocol = IPPROTO_TCP;
	if((retVal = getaddrinfo(theProbe.getRemote(), NULL, &aiHints, &aiList))!=0){
		errorMessageBox();
		return false;
	}


	UDP_PeerAddr->sin_family = AF_INET;
	UDP_PeerAddr->sin_port = htons(theProbe.getRemotePort());
	memcpy(&(UDP_PeerAddr->sin_addr),(aiList->ai_addr->sa_data+2),4);

	theProbe.timer.Start();
	theProbe.setStatus(4);

	SOCKET Sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(Sockfd == INVALID_SOCKET){
		theProbe.stop();
		errorMessageBox();
		AfxEndThread(0);
	}

	char *buf = (char *)malloc(sizeof(char)*4096);
	memset(buf, 0, sizeof(buf));

	int len = theProbe.getPacketSize();

	seqN = 0;
	while(true){
		if(theProbe.getTransferRate())
			if(flag && (theProbe.getByteTransfer()  / ((theProbe.timer.Elapsed() - sp) / 1000.0)) > theProbe.getTransferRate()){
				Sleep(10);
				continue;
			}

		memcpy(buf, &seqN, sizeof(seqN));
		retVal = sendto(Sockfd, buf, len, 0, (sockaddr *)UDP_PeerAddr, sizeof(struct addrinfo));
		if(retVal == SOCKET_ERROR){
			closesocket(Sockfd);
			theProbe.stop();
			errorMessageBox();
			break;
		}

		theProbe.packetTransfer(seqN++);
		theProbe.byteTransfer(retVal);
		
		if(!flag && theProbe.getByteTransfer() != 0){
			sp = theProbe.timer.Elapsed();
			flag = 1;
		}

		if(theProbe.getStatus() == 0){
			seqN = -1;
			memcpy(buf, &seqN, sizeof(seqN));
			retVal = sendto(Sockfd, buf, len, 0, (sockaddr *)UDP_PeerAddr, sizeof(struct addrinfo));
			if(retVal == SOCKET_ERROR){
				closesocket(Sockfd);
				theProbe.stop();
				errorMessageBox();
				break;
			}
			closesocket(Sockfd);
			break;
		}
	}

	AfxEndThread(0);
	
}



UINT __cdecl TCPStartRecv(LPVOID pParam){
	//NetProbe *theProbe = (NetProbe *)pParam;
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
	if((retVal = getaddrinfo(theProbe.getLocal(), NULL, &aiHints, &aiList))!=0){
		errorMessageBox();
		AfxEndThread(0);
	}


	TCP_Addr->sin_family = AF_INET;
	TCP_Addr->sin_port = htons(theProbe.getLocalPort());
	memcpy(&(TCP_Addr->sin_addr),(aiList->ai_addr->sa_data+2),4);

	theProbe.timer.Start();
	theProbe.setStatus(1);

	SOCKET Sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(Sockfd == INVALID_SOCKET){
		theProbe.stop();
		errorMessageBox();
		AfxEndThread(0);
	}
	retVal = bind(Sockfd, (struct sockaddr *)TCP_Addr, sizeof(struct sockaddr_in));
	if(retVal == SOCKET_ERROR){
		closesocket(Sockfd);
		theProbe.stop();
		errorMessageBox();
		AfxEndThread(0);
	}
	retVal = listen(Sockfd, 5);
	if(retVal == SOCKET_ERROR){
		theProbe.stop();
		errorMessageBox();
		AfxEndThread(0);
	}
	int addrlen = sizeof(struct sockaddr_in);
	SOCKET newsfd = accept(Sockfd, (struct sockaddr *)TCP_PeerAddr, &addrlen);
	if(newsfd == INVALID_SOCKET){
		theProbe.stop();
		errorMessageBox();
		AfxEndThread(0);
	}
	retVal = closesocket(Sockfd);
	if(retVal == SOCKET_ERROR){
		theProbe.stop();
		errorMessageBox();
		AfxEndThread(0);
	}

	char *buf = (char *)malloc(sizeof(char)*4096);

	while(true){
		int no;

		if(theProbe.getStatus() == 0){
			retVal = closesocket(newsfd);
			if(retVal == SOCKET_ERROR){
				theProbe.stop();
				errorMessageBox();
				AfxEndThread(0);
			}
		}

		retVal = recv(newsfd, buf, 4096, 0);
		if(retVal == SOCKET_ERROR){
			theProbe.stop();
			errorMessageBox();
			AfxEndThread(0);
		}

		theProbe.packetTransfer(-1);
		theProbe.byteTransfer(retVal);

	}

	AfxEndThread(0);
	
}


UINT __cdecl UDPStartRecv(LPVOID pParam){
	//NetProbe *theProbe = (NetProbe *)pParam;
	struct addrinfo aiHints;
	struct addrinfo *aiList = NULL;
	int retVal;


	struct sockaddr_in *UDP_Addr;
	struct sockaddr_in *UDP_PeerAddr;
	int PeerAddrSize = sizeof(sockaddr_in);
	UDP_Addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	UDP_PeerAddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	memset(UDP_Addr, 0, sizeof(struct sockaddr_in));
	memset(UDP_PeerAddr, 0, sizeof(struct sockaddr_in));


	memset(&aiHints,0,sizeof(aiHints));
	aiHints.ai_family = AF_INET;
	aiHints.ai_socktype = SOCK_STREAM;
	aiHints.ai_protocol = IPPROTO_TCP;
	if((retVal = getaddrinfo(theProbe.getLocal(), NULL, &aiHints, &aiList))!=0){
		errorMessageBox();
		AfxEndThread(0);
	}


	UDP_Addr->sin_family = AF_INET;
	UDP_Addr->sin_port = htons(theProbe.getLocalPort());
	memcpy(&(UDP_Addr->sin_addr),(aiList->ai_addr->sa_data+2),4);

	theProbe.timer.Start();
	theProbe.setStatus(2);

	SOCKET Sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(Sockfd == INVALID_SOCKET){
		theProbe.stop();
		errorMessageBox();
		AfxEndThread(0);
	}

	retVal = bind(Sockfd, (struct sockaddr *)UDP_Addr, sizeof(struct sockaddr_in));
	if(retVal == SOCKET_ERROR){
		closesocket(Sockfd);
		theProbe.stop();
		errorMessageBox();
		AfxEndThread(0);
	}

	char *buf = (char *)malloc(sizeof(char)*4096);

	while(true){
		int no;

		retVal = recvfrom(Sockfd, buf, 4096, 0, (sockaddr *)&UDP_PeerAddr, &PeerAddrSize);
		if(retVal == SOCKET_ERROR){
			closesocket(Sockfd);
			theProbe.stop();
			errorMessageBox();
			AfxEndThread(0);
		}
		memcpy(&no, buf, sizeof(no));

		if(no == -1){
			closesocket(Sockfd);
			theProbe.stop();
			errorMessageBox();
			AfxEndThread(0);
		}
		theProbe.packetTransfer(no);
		theProbe.byteTransfer(retVal);

		if(theProbe.getStatus() == 0){
			closesocket(Sockfd);
			break;
		}

	}

	AfxEndThread(0);
	return 0;
}

