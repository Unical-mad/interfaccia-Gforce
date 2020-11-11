#pragma once

#define WIN64

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifdef _LINUX //#include for LINUX OS
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#endif

#if defined( WIN32 ) || defined( WIN64 ) //#include for WINDOWS OS
#include<stdio.h>
#include<winsock2.h>
#include<Ws2tcpip.h>
#include<ws2def.h>
#include <iostream>
#include <vector>
#pragma comment(lib,"ws2_32.lib")
#endif


class udpCom
{
private:
	struct sockaddr_in serverClientStruct;	/* Struct to access server data */
	int commSocket;					/* Socket for client */
	int addrlen;						/* Size of struct sockaddr_in, required by sendto */


public:
	udpCom(u_short port, PWSTR  ip, u_long blockMode);
	~udpCom(void);
	int bindCom(void);
	int send(const char& val);
	int send(const float& val);
	int send(std::vector<char>& val);
	int rec(char& val);
	int rec(float& val);
	int rec(std::vector<char>& val, int bytesNumb);
	int connectToServer(void); 

};