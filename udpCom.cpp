#include "udpCom.h"
udpCom::udpCom(u_short port, PWSTR  ip,u_long blockMode)
{
	
#if defined(WIN32) || defined(WIN64)
	/* Initialise winsock */
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
#ifdef _DEBUG
			printf("-- ERROR --> Winsock Initialization failed. Error Code : %d\n", WSAGetLastError());
#endif
			exit(-1);
	}
	printf("-- Winsock Initialised.\n");
#endif

	/* Create socket */
	if ((commSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
#ifdef _DEBUG
		
			printf(" ERROR --> Socket creation failed ");
#if defined( WIN32 ) || defined( WIN64 )
			printf("%d\n", WSAGetLastError());
#else
			perror("Socket \n");
#endif
#endif		
		exit(-1);
	}
#ifdef _DEBUG
		printf(" Socket created.\n");
#endif
	/* Set socket to Non-blocking */
	
#if defined( WIN32 ) || defined( WIN64 )
	ioctlsocket(commSocket, FIONBIO, &blockMode);
#ifdef _DEBUG
	if(!blockMode)
		printf("blocking mode ON.\n");
#endif
	char broadcast = '1';



	if (setsockopt(commSocket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)

	{

		std::cout << "Error in setting Broadcast option";

		closesocket(commSocket);

		exit(-1);

	}
#else
	ioctl(serverSocket, FIONBIO, &noBlock);
#ifdef _DEBUG
		printf("blocking mode ON.\n");
#endif
#endif

	/* Prepare the sockaddr_in structure */
	memset((char*)&serverClientStruct, 0, sizeof(serverClientStruct));

	serverClientStruct.sin_family = AF_INET;
	serverClientStruct.sin_port = htons(port);
	//serverClientStruct.sin_addr.s_addr = inet_addr((const char*)ip);
	InetPton(AF_INET, ip, &serverClientStruct.sin_addr.s_addr);
	addrlen = sizeof(serverClientStruct);

	/*InetPton(serverClientStruct.sin_fa//mily, target_ip, &serverClientStruct.sin_addr.s_addr);
	InetNtop(serverClientStruct.sin_family, &serverClientStruct.sin_addr.s_addr, host_ip, INET_ADDRSTRLEN*sizeof(host_ip));
	InetNtop(serverClientStruct.sin_family, &serverClientStruct.sin_addr.s_addr, target_ip, INET_ADDRSTRLEN*sizeof(target_ip));*/
#if _DEBUG
	wprintf(L"Host IP: %s at Port: %d \n", ip, ntohs(serverClientStruct.sin_port));
	printf("Sending data...\n");
#endif
}

udpCom::~udpCom(void)
{
#if defined( WIN32 ) || defined( WIN64 )	
	closesocket(commSocket);
	WSACleanup();
#else	
	close(clientSocket);
#endif	
#ifdef _DEBUG
		printf(" Socket closed\n ");
#endif
}

int udpCom::bindCom(void)
{
	int res = bind(commSocket, (struct sockaddr*)&serverClientStruct, sizeof(serverClientStruct));
	if (res== -1) 
	{
#ifdef _DEBUG
		printf(" --> Bind failed ");

#if defined( WIN32 ) || defined( WIN64 )
		printf("%d\n", WSAGetLastError());
#else
		perror("Bind \n");
#endif
#endif
	}
	return res;
}

int udpCom::send(const char& val)
{
	int nSent;
	if (nSent = sendto(commSocket, &val, 1, 0, (struct sockaddr*)&serverClientStruct, addrlen) == -1)
	{
#ifdef _DEBUG
		printf("ERROR --> Sending buffer failed ");
#endif
	}
	//gestire errori cmunicazione
	return nSent;
}

int udpCom::send(const float& val)
{
	int nSent= sendto(commSocket, (const char*)&val, 4, 0, (struct sockaddr*)&serverClientStruct, addrlen);
	
	if (nSent == -1 || nSent != 4)
	{
#ifdef _DEBUG
		printf("ERROR --> Sending buffer failed ");
#endif
	}
	//gestire errori cmunicazione
	return nSent;

}

int udpCom::send(std::vector<char>& val)
{
	int nSent = sendto(commSocket, (const char*)&val[0], val.size(), 0, (struct sockaddr*)&serverClientStruct, addrlen);
	if (nSent == -1 || nSent != val.size())
	{
#ifdef _DEBUG
		printf("ERROR --> Sending buffer failed ");
#endif
	}

	return nSent;
}

int udpCom::rec(char& val)
{
	int nReceived = recvfrom(commSocket, (char*)&val, 1, 0, (struct sockaddr*)&serverClientStruct, &addrlen);
	if (nReceived != 1)
	{
#ifdef _DEBUG
		printf("ERROR --> receiving char \n ");
		
#endif
	}
	return nReceived;
}

int udpCom::rec(float& val)
{
	int nReceived = recvfrom(commSocket, (char*)&val, 4, 0, (struct sockaddr*)&serverClientStruct, &addrlen);
	if (nReceived != 4)
	{
#ifdef _DEBUG
		printf("ERROR --> receiving float\n");
#endif
	}
	return nReceived;
}

int udpCom::rec(std::vector<char>& val, int bytesNumb)
{

	int nReceived = recvfrom(commSocket, (char*)&val[0], bytesNumb, 0, (struct sockaddr*)&serverClientStruct, &addrlen);
	
	if (nReceived != bytesNumb)
	{
#ifdef _DEBUG
		printf("ERROR --> receiving char \n ");

#endif
	}
	printf("Expected BYtes:%d - received bytes :%d\n ", bytesNumb, nReceived);
	return nReceived;
}

int udpCom::connectToServer(void)
{
	int res = connect(commSocket, (struct sockaddr*)&serverClientStruct, sizeof(serverClientStruct));
	if (res < 0)
	{
		printf("ERROR --> connect server\n ");

	}
	return res;
}


