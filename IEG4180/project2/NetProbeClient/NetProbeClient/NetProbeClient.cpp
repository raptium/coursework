// NetProbeClient.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "NetProbeClient.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNetProbeClientApp

BEGIN_MESSAGE_MAP(CNetProbeClientApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CNetProbeClientApp construction

CNetProbeClientApp::CNetProbeClientApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CNetProbeClientApp object

CNetProbeClientApp theApp;


// CNetProbeClientApp initialization

BOOL CNetProbeClientApp::InitInstance()
{
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

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CNetProbeClientDlg dlg;
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

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


void errorMessageBox(){
	int e = WSAGetLastError();
	LPSTR pBuf = NULL;
	FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		e,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
		(LPSTR)&pBuf,
		0 ,
		NULL);
	
	if(e)
		AfxMessageBox(pBuf, MB_ICONINFORMATION);
		
}

struct sockaddr_in *NetProbe::createSockAddr(char *host, int port){
	struct addrinfo aiHints;
	struct addrinfo *aiList = NULL;
	struct sockaddr_in *Addr = new struct sockaddr_in;
	int retVal;

	memset(&aiHints,0,sizeof(aiHints));
	aiHints.ai_family = AF_INET;
	aiHints.ai_socktype = SOCK_STREAM;
	aiHints.ai_protocol = IPPROTO_TCP;
	if((retVal = getaddrinfo(host, NULL, &aiHints, &aiList))!=0){
		errorMessageBox();
		return NULL;
	}

	Addr->sin_family = AF_INET;
	Addr->sin_port = htons(port);
	memcpy(&(Addr->sin_addr),(aiList->ai_addr->sa_data+2),4);

	return Addr;
}

NetProbe::NetProbe(CNetProbeClientDlg *dialog, char *host, int port){
	theDlg = dialog;
	Server_Addr = createSockAddr(host, port);
	bytesTransferred = 0;
	packetsTransferred = 0;
	maxPacketNum = -1;
	status = 0;
}

DWORD WINAPI NetProbe::threadUpdateUI(LPVOID lpInstance){
	NetProbe *pClass = (NetProbe *)lpInstance;
	int count = 0;
	char t[10];
	double bps;
	CNetProbeClientDlg *dlg = pClass->theDlg;

	int refreshInterval = dlg->GetDlgItemInt(IDC_RI);

	while(1){
		while(1){
			Sleep(10);
			if(pClass->status == -1)
				break;
			if(pClass->timer.Elapsed() > count * refreshInterval && pClass->status != 0){
				count = pClass->timer.Elapsed() / refreshInterval + 1;
				break;
			}
		}

		if(pClass->status == -1)
			break;

		sprintf(t, "%.2f sec", pClass->timer.Elapsed()/1000.0);
		dlg->SetDlgItemTextA(IDC_TE, t);

		sprintf(t, "%d", pClass->packetsTransferred);
		dlg->SetDlgItemTextA(IDC_NPT, t);

		sprintf(t, "%d", pClass->maxPacketNum + 1 - pClass->packetsTransferred);
		dlg->SetDlgItemTextA(IDC_NPL, t);

		sprintf(t, "%.2f%", 100.0 - pClass->packetsTransferred / (pClass->maxPacketNum + 1.0) * 100.0);
		dlg->SetDlgItemTextA(IDC_PL, t);

		bps = pClass->bytesTransferred  / (pClass->timer.Elapsed() / 1000.0);

		if(bps<0)
			bps = 0;
		sprintf(t, "%.3f Bps", bps);
		dlg->SetDlgItemTextA(IDC_DTR, t);

	}

	pClass->theDlg->SetDlgItemTextA(IDCONNECT, "Connect");
	AfxEndThread(0);

	return 0;
}

int NetProbe::TCPConnect(LPVOID lpInstance){
	int retVal;

	NetProbe *pClass = (NetProbe *)lpInstance;

	pClass->Sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(pClass->Sockfd == INVALID_SOCKET){
		pClass->stop();
		errorMessageBox();
		return -1;
	}

	retVal = connect(pClass->Sockfd, (const sockaddr *)pClass->Server_Addr, sizeof(struct sockaddr));
	if(retVal == SOCKET_ERROR){
		pClass->stop();
		closesocket(pClass->Sockfd);
		errorMessageBox();
		return -1;
	}


	pClass->PacketSize = pClass->theDlg->GetDlgItemInt(IDC_PS);
	pClass->SendingRate = pClass->theDlg->GetDlgItemInt(IDC_SR);
	pClass->NumPackets = pClass->theDlg->GetDlgItemInt(IDC_NPS);

	char *buf = new char[12];
	memcpy(buf, &pClass->PacketSize, 4);
	memcpy(buf + 4, &pClass->SendingRate, 4);
	memcpy(buf + 8, &pClass->NumPackets, 4);
	retVal = send(pClass->Sockfd, buf, 12, 0);
	if(retVal == SOCKET_ERROR){
		pClass->stop();
		closesocket(pClass->Sockfd);
		errorMessageBox();
		return -1;
	}

	return 0;
}

DWORD WINAPI NetProbe::threadTCPReceive(LPVOID lpInstance){
	int retVal;

	NetProbe *pClass = (NetProbe *)lpInstance;

	pClass->timer.Start();
	pClass->status = 1;

	char *buf = new char[pClass->PacketSize];
	while(1){

		retVal = recv(pClass->Sockfd, buf, pClass->PacketSize, 0);
		if(retVal == SOCKET_ERROR){
			pClass->stop();
			closesocket(pClass->Sockfd);
			errorMessageBox();
			AfxEndThread(0);
		}
		if(retVal == 0 || pClass->status == -1)
			break;

		pClass->bytesTransferred += retVal;
		pClass->packetsTransferred++;
		pClass->maxPacketNum++;
	}

	closesocket(pClass->Sockfd);
	return 0;
}

DWORD WINAPI NetProbe::threadUDP(LPVOID lpInstance){
	int retVal;
	int seqN;

	NetProbe *pClass = (NetProbe *)lpInstance;

	SOCKET Sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(Sockfd == INVALID_SOCKET){
		errorMessageBox();
		return -1;
	}

	int PacketSize;
	int SendingRate;
	int NumPackets;
	PacketSize = pClass->theDlg->GetDlgItemInt(IDC_PS);
	SendingRate = pClass->theDlg->GetDlgItemInt(IDC_SR);
	NumPackets = pClass->theDlg->GetDlgItemInt(IDC_NPS);

	char *buf = new char[PacketSize];
	memcpy(buf, &PacketSize, 4);
	memcpy(buf + 4, &SendingRate, 4);
	memcpy(buf + 8, &NumPackets, 4);
	retVal = sendto(Sockfd, buf, 12, 0, (const sockaddr *)pClass->Server_Addr, sizeof(struct sockaddr_in));
	if(retVal == SOCKET_ERROR){
		pClass->stop();
		closesocket(Sockfd);
		errorMessageBox();
		return -1;
	}

	pClass->timer.Start();
	pClass->status = 2;
	while(1){

		retVal = recvfrom(Sockfd, buf, PacketSize, 0, NULL, NULL);
		if(retVal == SOCKET_ERROR){
			pClass->stop();
			closesocket(Sockfd);
			errorMessageBox();
			AfxEndThread(0);
		}
		if(retVal == 0 || pClass->status == -1)
			break;

		memcpy(&seqN, buf, sizeof(seqN));
		pClass->bytesTransferred += retVal;
		pClass->packetsTransferred++;
		pClass->maxPacketNum = seqN;

		if(NumPackets && pClass->maxPacketNum == NumPackets - 1)
			break;
	}

	closesocket(Sockfd);
	return 0;
}

void NetProbe::stop(){
	status = -1;
}

int NetProbe::MsgDrivenReady(){
	const long WM_WINSOCK = WM_USER + 1;

	WSAAsyncSelect(Sockfd, theDlg->m_hWnd, WM_WINSOCK, FD_READ | FD_CLOSE);
}