// NetProbeClient.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h" // main symbols
#include "es_TIMER.H"
#include "NetProbeClientDlg.h"
#include <winsock2.h>


// CNetProbeClientApp:
// See NetProbeClient.cpp for the implementation of this class
//

class CNetProbeClientApp : public CWinApp
{
public:
	CNetProbeClientApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CNetProbeClientApp theApp;



class NetProbe
{
private: 
	struct sockaddr_in *Server_Addr;
	struct sockaddr_in *createSockAddr(char *host, int port);
	SOCKET tcpfd, udpfd;
	CNetProbeClientDlg *theDlg;
	double bytesTransferred;
	int packetsTransferred;
	int maxPacketNum;
	ES_FlashTimer timer;


public:
	NetProbe(CNetProbeClientDlg *dialog, char *host, int port);
	CWinThread *wThread;
	static DWORD WINAPI threadUpdateUI(LPVOID lpInstance);
	static DWORD WINAPI threadTCP(LPVOID lpInstance);
	static DWORD WINAPI threadUDP(LPVOID lpInstance);
};