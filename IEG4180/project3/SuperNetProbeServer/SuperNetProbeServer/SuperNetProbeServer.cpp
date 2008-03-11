#include "SuperNetProbeServer.h"



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

int Init(void){
	if (!AfxSocketInit()) {
		cerr << "Cannot initialize the WinSock library." << endl;
		return -1;
	}
	return 0;
}


NetProbeServer::NetProbeServer(const char *tcp_h, int tcp_p, const char *udp_h, int udp_p, const char *http_h, int http_p){

	TCP_Addr = createSockAddr((char *)tcp_h, tcp_p);
	UDP_Addr = createSockAddr((char *)udp_h, udp_p);
	HTTP_Addr = createSockAddr((char *)http_h, http_p);
	tcpfd = 0;
	udpfd = 0;
	httpfd = 0;
	maxNumClients = 0;
	numClients = 0;
	
	if( TCP_Addr == NULL || UDP_Addr == NULL){
		cerr << "error." << endl;
		exit(-1);
	}
}


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


int NetProbeServer::TCPReady(void){
	int retVal;

	u_long iMode = 1;


	SOCKET Sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(Sockfd == INVALID_SOCKET){
		errorMessageBox();
		return -1;
	}
	cout << "-> TCP : Created Socket" << endl;

	retVal = bind(Sockfd, (struct sockaddr *)TCP_Addr, sizeof(struct sockaddr_in));
	if(retVal == SOCKET_ERROR){
		closesocket(Sockfd);
		errorMessageBox();
		return -1;
	}
	cout  << "-> TCP : Binded Socket" << endl;

	retVal = listen(Sockfd, 5);
	if(retVal == SOCKET_ERROR){
		errorMessageBox();
		return -1;
	}
	cout << "-> TCP : Waiting Connection" << endl;


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
	cout <<	"-> UDP : Created Socket" << endl;

	BOOL bOptVal = TRUE;
	int bOptLen = sizeof(BOOL);
	if (setsockopt(Sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&bOptVal, bOptLen) == SOCKET_ERROR) {
		errorMessageBox();
	}

	retVal = bind(Sockfd, (struct sockaddr *)UDP_Addr, sizeof(struct sockaddr_in));
	if(retVal == SOCKET_ERROR){
		closesocket(Sockfd);
		errorMessageBox();
		return -1;
	}
	cout << "-> UDP : Binded Socket" << endl;

	/*
	retVal = ioctlsocket(Sockfd, FIONBIO, &iMode);
	if(retVal == SOCKET_ERROR){
		errorMessageBox();
		return -1;
	}
	*/
	
	DWORD dwBytesReturned = 0;
	BOOL bNewBehavior = false;

	// disable  new behavior using
	// IOCTL: SIO_UDP_CONNRESET
	retVal = WSAIoctl(Sockfd, SIO_UDP_CONNRESET, &bNewBehavior, sizeof(bNewBehavior), NULL, 0, &dwBytesReturned, NULL, NULL);
	if (retVal == SOCKET_ERROR){
		errorMessageBox();
		return -1;
	}
	
	cout << "-> UDP : Waiting Incoming Data" << endl;
	udpfd = Sockfd;

	return 0;

}


int NetProbeServer::HTTPReady(void){
	int retVal;

	SOCKET Sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(Sockfd == INVALID_SOCKET){
		errorMessageBox();
		return -1;
	}
	
	retVal = bind(Sockfd, (struct sockaddr *)HTTP_Addr, sizeof(struct sockaddr_in));
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

	httpfd = Sockfd;

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
	// create a new thread and send packet to the client
	// the sending prarameters will be sent by the client 
	// at the beginning of comunication

	int addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in *PeerAddr = new struct sockaddr_in;
	int retVal;
	NetProbeServer *Server = (NetProbeServer *)lpInstance;
	// statistics variables
	int packetsSent; // number of packets sent
	//double bytesSent; // total bytes received
	ES_FlashTimer timer;
	/*****************/
	double T;
	double sleepT;



	SOCKET newsfd = accept(Server->tcpfd, (struct sockaddr *)PeerAddr, &addrlen);
	if(newsfd == INVALID_SOCKET){
		errorMessageBox();
		AfxEndThread(-1);
	}
	cout << "-> TCP : Accepted Sender" << endl;
	Server->numClients++;

	u_long iMode = 0;
	retVal = ioctlsocket(newsfd, FIONBIO, &iMode);
	if(retVal == SOCKET_ERROR){
		errorMessageBox();
		return -1;
	}

	char *buf = new char[12];
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

	cout << "=> Packet Size: " << PacketSize << endl;
	cout << "=> Sending Rate: " << SendingRate << endl;
	cout << "=> Number of Packets to Send: " <<  NumPackets << endl;
	
	T = (double)PacketSize / SendingRate;

	timer.Start(); // Start the timer
	cout << "-> TCP : Sending Data" << endl;

	packetsSent = 0;

	delete buf;
	buf = new char[PacketSize];
	while(1){
		if(NumPackets && packetsSent >= NumPackets)
			break;
	
		retVal = send(newsfd, buf, PacketSize, 0);
		if(retVal == SOCKET_ERROR){
			closesocket(newsfd);
			errorMessageBox();
			AfxEndThread(0);
		}
		if(retVal == 0)
			break;

		packetsSent++;
		if(SendingRate){
			sleepT = packetsSent * T * 1000 - timer.Elapsed();
			if(sleepT > 0)
				Sleep((DWORD)sleepT);
		}
	}

	closesocket(newsfd);
	
	AfxEndThread(0);
	return 0;
}

DWORD WINAPI NetProbeServer::threadUDPSend(LPVOID lpInstance){
	// create a new thread and send packet to the client
	// the sending prarameters will be sent by the client 
	// at the beginning of comunication

	int addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in *Addr = new struct sockaddr_in;
	struct sockaddr_in *PeerAddr = new struct sockaddr_in;
	int retVal;
	NetProbeServer *Server = (NetProbeServer *)lpInstance;

	// statistics variables
	int packetsSent; // number of packets sent
	ES_FlashTimer timer;
	/*****************/
	double T;
	double sleepT;

	char *buf = new char[12];
	retVal = recvfrom(Server->udpfd, buf, 12, 0, (sockaddr *)PeerAddr, &addrlen);
	if(retVal == SOCKET_ERROR){
		if(WSAGetLastError() == WSAECONNRESET)
			AfxEndThread(0);
		errorMessageBox();
		AfxEndThread(0);
	}
	cout << "-> UDP : Get new Client" << endl;
	Server->numClients++;


	int PacketSize;
	int SendingRate;
	int NumPackets;
	memcpy(&PacketSize, buf, 4);
	memcpy(&SendingRate, buf + 4, 4);
	memcpy(&NumPackets, buf + 8, 4);

	cout << "=> Packet Size: " << PacketSize << endl;
	cout << "=> Sending Rate: " << SendingRate << endl;
	cout << "=> Number of Packets to Send: " <<  NumPackets << endl;

	//if(!SendingRate)
	//	SendingRate = 1024 * 1024 * 15;

	FD_SET Peer;
	
	T = (double)PacketSize / SendingRate;

	timer.Start(); // Start the timer
	cout << "-> UDP : Sending Packets" << endl;

	packetsSent = 0;

	delete buf;
	buf = new char[PacketSize];
	while(1){
		if(NumPackets && packetsSent >= NumPackets)
			break;

		FD_ZERO(&Peer);
		FD_SET(Server->udpfd, &Peer);
		retVal = select(0, NULL, &Peer, NULL, NULL);

		if(retVal && FD_ISSET(Server->udpfd, &Peer)){
			memcpy(buf, &packetsSent, sizeof(packetsSent));
			retVal = sendto(Server->udpfd, buf, PacketSize, 0, (const sockaddr *)PeerAddr, addrlen);
			if(retVal == SOCKET_ERROR){
				errorMessageBox();
				AfxEndThread(0);
			}
			if(retVal == 0)
				break;

			packetsSent++;
		}

		if(SendingRate){
			sleepT = packetsSent * T * 1000 - timer.Elapsed();
			if(sleepT > 0)
				Sleep((DWORD)sleepT);
		}

	}
	
	AfxEndThread(0);
	return 0;
}

DWORD WINAPI NetProbeServer::threadHTTPServer(LPVOID lpInstance){
	int addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in *PeerAddr = new struct sockaddr_in;
	int retVal;
	NetProbeServer *Server = (NetProbeServer *)lpInstance;



	char *buf = new char[2048];
	string *str;
	int pos;

	while(1){

		SOCKET newsfd = accept(Server->httpfd, (struct sockaddr *)PeerAddr, &addrlen);
		if(newsfd == INVALID_SOCKET){
			errorMessageBox();
			AfxEndThread(-1);
		}


		memset(buf,0,2048);
		retVal = recv(newsfd, buf, 2048, 0);
		if(retVal == SOCKET_ERROR){
			closesocket(newsfd);
			errorMessageBox();
			AfxEndThread(0);
		}
		str = new string(buf);
		pos = str->find(string("GET"));
		if(pos)
			continue;
		pos = str->find(string("HTTP"));

		char *outbuf = new char[1024];
		memset(outbuf, 0, 1024);
		memcpy(outbuf, buf+4,pos-5);
		cout << outbuf;
		if(strcmp("/", outbuf))
			continue;

		buf[0]=0;
		strcat_s(buf, 2048, "HTTP/1.1 200 OK\n");
		strcat_s(buf, 2048, "Date: Mon, 10 Mar 2008 22:38:34 GMT\n");
		strcat_s(buf, 2048, "Server: SuperNetProbeServer/0.3.1 (Microsoft Windows)\n");
		//strcat_s(buf, 2048, "Last-Modified: Wed, 08 Jan 2003 23:11:55 GMT\n");
		//strcat_s(buf, 2048, "Etag: \"3f80f-1b6-3e1cb03b\"\n");
		strcat_s(buf, 2048, "Accept-Ranges: bytes\n");
		strcat_s(buf, 2048, "Content-Length: 200\n");
		strcat_s(buf, 2048, "Connection: close\n");
		strcat_s(buf, 2048, "Content-Type: text/html; charset=UTF-8\n");
		//strcat_s(buf, 2048, "\nhello world!\n");
		strcat_s(buf, 2048, "\n<html><head><title>IEG4180 Project 3:SuperNetProbeServer</title></head><body><h1>Hello World!</h1></body></html>");

		retVal = send(newsfd, buf, strlen(buf), 0);
		if(retVal == SOCKET_ERROR){
			closesocket(newsfd);
			errorMessageBox();
			AfxEndThread(0);
		}
		shutdown(newsfd, SD_SEND);
		closesocket(newsfd);
		delete str;

	}

	return 0;
}



int main(int argc, char const *argv[]){
	NetProbeServer *Server;

	//initialization
	if(Init() == -1)
		return -1;

	
	if(argc!=7){
		Server = new NetProbeServer("localhost", 12345, "localhost", 12345, "localhost", 80);
	}else
		Server = new NetProbeServer(argv[1], atoi(argv[2]), argv[3], atoi(argv[4]), argv[5], atoi(argv[6]));
	


	//bind
	//listen
	//TCP
	Server->TCPReady();
	Server->UDPReady();
	Server->HTTPReady();

	AfxBeginThread((AFX_THREADPROC)Server->threadHTTPServer, (LPVOID)Server);
	//accept
	//UDPRecv
	while(1){
		Server->detectProtocol();
		Sleep(50);
	}



	return 0;
}