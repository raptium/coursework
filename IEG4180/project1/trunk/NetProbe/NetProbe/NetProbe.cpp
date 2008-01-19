// NetProbe.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "NetProbe.h"
#include "NetProbeDlg.h"
#include <string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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

void NetProbe::startReceive(void){
	if(this->status)
		return;
	if(this->protocol == 0){
		MessageBox(m_pMainWnd,"Please choose a Protocol!.","Error",0);
	SOCKET Sockfd = socket(AF_INET, SOC

}
