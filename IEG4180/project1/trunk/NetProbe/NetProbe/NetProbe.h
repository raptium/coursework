// NetProbe.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "NetProbeDlg.h"
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
	NetProbe();
	BOOL startSend();
	BOOL startReceive();
	void stop(void);
	void stop(BOOL n);
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
	int getPacketLost(void);
	double getTime(void);
	int getStatus(void);
	char *getLocal(void);
	char *getRemote(void);
	int getLocalPort(void);
	int getRemotePort(void);
	void setStatus(int n);
	int getPacketSize(void);
	int getInterval(void);
	ES_FlashTimer timer;
	void packetTransfer(int n);
	int getMaxNum(void);
	int getPacketTransfer(void);
	double getByteTransfer(void);
	void byteTransfer(int n);
	int getNumPackets(void);

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
	int maxPacketNum;
	double byteTransferred;

	
};

extern NetProbe theProbe;