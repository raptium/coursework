#include "stdafx.h"
#include <string.h>
#include <winsock2.h> 
#include <iostream>
#include <stdlib.h>

using namespace std;





/*

UINT __cdecl TCPStart(LPVOID pParam){
	//NetProbe *theProbe = (NetProbe *)pParam;
	struct addrinfo aiHints;
	struct addrinfo *aiList = NULL;
	int retVal;
	int addrlen = sizeof(struct sockaddr_in);


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
	
	SOCKET newsfd = accept(Sockfd, (struct sockaddr *)TCP_PeerAddr, &addrlen);
	if(newsfd == INVALID_SOCKET){
		closesocket(Sockfd);
		theProbe.stop();
		errorMessageBox();
		AfxEndThread(0);
	}
	closesocket(Sockfd);


	int len = theProbe.getPacketSize();
	char *buf = (char *)malloc(sizeof(char)*len);

	while(true){
		int no;

		if(theProbe.getStatus() == 0){
			retVal = closesocket(newsfd);
			theProbe.stop();
			AfxEndThread(0);
		}

		retVal = recv(newsfd, buf, len, 0);
		if(retVal == SOCKET_ERROR){
			closesocket(newsfd);
			theProbe.stop();
			errorMessageBox();
			AfxEndThread(0);
		}
		if(retVal == 0){
			theProbe.stop();
			AfxEndThread(0);
		}
		theProbe.packetTransfer(-1);
		theProbe.byteTransfer(retVal);
	}

	AfxEndThread(0);
	return 0;
	
}
*/

void errorMessageBox(){
	int e = WSAGetLastError();
	LPSTR pBuf = NULL;
	FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER |FORMAT_MESSAGE_IGNORE_INSERTS |FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		e,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
		(LPSTR)&pBuf,
		0 ,
		NULL);
	/*
	if(e)
		AfxMessageBox(pBuf, MB_ICONINFORMATION);
		*/
}

int Init(void){
	if (!AfxSocketInit()) {
		cerr << "Cannot initialize the WinSock library." << endl;
		return -1;
	}
	return 0;
}


class NetProbeServer{

private: 
	char *tcp_host;
	char *udp_host;
	int tcp_port;
	int udp_port;
	struct sockaddr_in *TCP_Addr;
	struct sockaddr_in *UDP_Addr;
	struct sockaddr_in *createSockAddr(char *host, int port);
	SOCKET tcpfd, udpfd;

public:
	NetProbeServer(const char *tcp_h, int tcp_p, const char *udp_h, int udp_p);
	int TCPReady(void);
	int UDPReady(void);
	int detectProtocol(void);
	static DWORD WINAPI threadTCPSend(LPVOID lpInstance);
	static DWORD WINAPI threadUDPSend(LPVOID lpInstance);

};


struct sockaddr_in * NetProbeServer::createSockAddr(char *host, int port){
	struct addrinfo aiHints;
	struct addrinfo *aiList = NULL;
	struct sockaddr_in *Addr = new struct sockaddr_in;
	int retVal;

	memset(&aiHints,0,sizeof(aiHints));
	aiHints.ai_family = AF_INET;
	aiHints.ai_socktype = SOCK_STREAM;
	aiHints.ai_protocol = IPPROTO_TCP;
	if((retVal = getaddrinfo(host, NULL, &aiHints, &aiList))!=0){
		cerr << "error." << endl;
		return NULL;
	}

	Addr->sin_family = AF_INET;
	Addr->sin_port = htons(port);
	memcpy(&(Addr->sin_addr),(aiList->ai_addr->sa_data+2),4);

	return Addr;
}

NetProbeServer::NetProbeServer(const char *tcp_h, int tcp_p, const char *udp_h, int udp_p){

	/*
	tcp_host = new char[strlen(tcp_h)+1];
	udp_host = new char[strlen(udp_h)+1];
	strcpy_s(tcp_host, sizeof(tcp_host), tcp_h);
	strcpy_s(udp_host, sizeof(udp_host), udp_h);
	tcp_port = tcp_p;
	udp_port = udp_p;
	*/

	TCP_Addr = createSockAddr((char *)tcp_h, tcp_p);
	UDP_Addr = createSockAddr((char *)udp_h, udp_p);
	
	if( TCP_Addr == NULL || UDP_Addr == NULL){
		cerr << "error." << endl;
		exit(-1);
	}
}

int NetProbeServer::TCPReady(void){
	int retVal;

	u_long iMode = 1;


	SOCKET Sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(Sockfd == INVALID_SOCKET){
		errorMessageBox();
		return -1;
	}
	retVal = bind(Sockfd, (struct sockaddr *)TCP_Addr, sizeof(struct sockaddr_in));
	if(retVal == SOCKET_ERROR){
		closesocket(Sockfd);
		errorMessageBox();
		return -1;
	}
	retVal = listen(Sockfd, 5);
	if(retVal == SOCKET_ERROR){
		errorMessageBox();
		return -1;
	}
	retVal = ioctlsocket(Sockfd, FIONBIO, &iMode);
	if(retVal == SOCKET_ERROR){
		errorMessageBox();
		return -1;
	}

	tcpfd = Sockfd;

	return 0;
}

int NetProbeServer::UDPReady(void){
	int retVal;	
	u_long iMode = 1;

	SOCKET Sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(Sockfd == INVALID_SOCKET){
		errorMessageBox();
		return -1;
	}

	retVal = bind(Sockfd, (struct sockaddr *)TCP_Addr, sizeof(struct sockaddr_in));
	if(retVal == SOCKET_ERROR){
		closesocket(Sockfd);
		errorMessageBox();
		return -1;
	}

	retVal = ioctlsocket(Sockfd, FIONBIO, &iMode);
	if(retVal == SOCKET_ERROR){
		errorMessageBox();
		return -1;
	}

	udpfd = Sockfd;

	return 0;

}

int NetProbeServer::detectProtocol(void){
	FD_SET Peer;
	int retVal;

	FD_ZERO(&Peer);

	FD_SET(tcpfd, &Peer);
	FD_SET(udpfd, &Peer);
	retVal = select(0, &Peer, NULL, NULL, NULL);
	if(retVal == SOCKET_ERROR){
		errorMessageBox();
		return -1;
	}
	if(retVal == 0)
		return 0;
	if(FD_ISSET(tcpfd, &Peer)){
		AfxBeginThread((AFX_THREADPROC)threadTCPSend, this);
		return 0;
	}else if(FD_ISSET(udpfd, &Peer)){
		AfxBeginThread((AFX_THREADPROC)threadUDPSend, this);
		return 0;
	}
	
	return 0;
}

DWORD WINAPI NetProbeServer::threadTCPSend(LPVOID lpInstance){
	int addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in *TCP_PeerAddr = new struct sockaddr_in;
	int retVal;
	NetProbeServer *Server = (NetProbeServer *)lpInstance;

	u_long iMode = 0;

	SOCKET newsfd = accept(Server->tcpfd, (struct sockaddr *)TCP_PeerAddr, &addrlen);
	if(newsfd == INVALID_SOCKET){
		errorMessageBox();
		AfxEndThread(-1);
	}

	cout << "New TCP connection." << endl;

	retVal = ioctlsocket(newsfd, FIONBIO, &iMode);
	if(retVal == SOCKET_ERROR){
		errorMessageBox();
		return -1;
	}


	char *buf = new char[1024];
	int len = 1024;

	retVal = recv(newsfd, buf, 12, 0);
	if(retVal == SOCKET_ERROR){
		closesocket(newsfd);
		errorMessageBox();
		AfxEndThread(0);
	}
	int PacketSize;
	int SendingRate;
	int NumPackets;
	memcpy(&PacketSize, buf, 4);
	memcpy(&SendingRate, buf + 4, 4);
	memcpy(&NumPackets, buf + 8, 4);

	/*
	while(1){
		retVal = recv(newsfd, buf, len, 0);
		if(retVal == SOCKET_ERROR){
			closesocket(newsfd);
			errorMessageBox();
			AfxEndThread(0);
		}
		if(retVal == 0)
			break;
		cout << retVal << " Bytes received." << endl;
		for(int i=0;i<retVal;i++)
			cout << (int *)buf[i] << " ";
	}
	*/

	cout << "Packet Size: " << PacketSize << endl;
	cout << "Sending Rate: " << SendingRate << endl;
	cout << "Number of Packets to Send: " <<  NumPackets << endl;

	closesocket(newsfd);
	
	AfxEndThread(0);
	return 0;
}

DWORD WINAPI NetProbeServer::threadUDPSend(LPVOID lpInstance){
	cout << "New UDP connection." << endl;
	AfxEndThread(0);
	return 0;
}




int main(int argc, char const *argv[]){

	//initialization
	if(Init() == -1)
		return -1;

	if(argc!=5){
		cerr << "Invalid parameters." << endl;
		return -1;
	}

	NetProbeServer *Server = new NetProbeServer(argv[1], atoi(argv[2]), argv[3], atoi(argv[4]));
		


	//bind
	//listen
	//TCP
	Server->TCPReady();
	Server->UDPReady();

	//accept
	//UDPRecv
	while(1){
		Server->detectProtocol();
		Sleep(50);
	}





	return 0;
}