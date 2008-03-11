#ifndef SERVER_H
#define SERVER_H

#include "stdafx.h"
#include "es_TIMER.h"
#include <string.h>
#include <winsock2.h> 
#include <iostream>
#include <stdlib.h>

using namespace std;

class NetProbeServer{
private: 
	struct sockaddr_in *TCP_Addr;
	struct sockaddr_in *UDP_Addr;
	struct sockaddr_in *HTTP_Addr;
	struct sockaddr_in *createSockAddr(char *host, int port);
	SOCKET tcpfd, udpfd, httpfd;
	int maxNumClients;
	int numClients;

public:
	NetProbeServer(const char *tcp_h, int tcp_p, const char *udp_h, int udp_p, const char *http_h, int http_p);
	int TCPReady(void);
	int UDPReady(void);
	int HTTPReady(void);
	int detectProtocol(void);
	static DWORD WINAPI threadTCPSend(LPVOID lpInstance);
	static DWORD WINAPI threadUDPSend(LPVOID lpInstance);
	static DWORD WINAPI threadHTTPServer(LPVOID lpInstance);

};
#endif