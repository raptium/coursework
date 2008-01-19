// NetProbe.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "es_TIMER.H"


// CNetProbeApp:
// See NetProbe.cpp for the implementation of this class
//

class CNetProbeApp : public CWinApp
{
public:
	CNetProbeApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CNetProbeApp theApp;


class NetProbe{
public:
	void startSend();
	BOOL startReceive();
	void stop();
	void setLocal(char *h);
	void setRemote(char *h);
	void setLocalPort(int n);
	void setRemotePort(int n);
	void setRefreshInterval(int t);
	void setPacketSize(int n);
	void setSendingRate(int n);
	void setNumPackets(int n);
	void setProtocol(int p);
	int getTransferRate(void);
	double getPacketLoss(void);
	int getPacketTransferred(void);
	int getPacketLost(void);
	double getTime(void);
	int getStatus(void);

private:
	int status;
	char *local;
	char *remote;
	int localPort;
	int remotePort;
	int refreshInterval;
	int packetSize;
	int sendingRate;
	int numPackets;
	int packetTransferred;
	int protocol;
	ES_FlashTimer timer;
	
};

extern NetProbe theProbe;