// ServerCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <typeinfo>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define MAX_CONNECTIONS 5

void thread_Function(SOCKET ClientSocket, int iSendResult, int iResult, char* recvbuf, int recvbuflen, bool &shutdown)
{
	
	char *stringSearch;
	bool thread_shutdown = false;

	// Receive until the peer shuts down the connection
	while ((shutdown == false) and (thread_shutdown == false))
	{
		for (int i = 0; i < DEFAULT_BUFLEN; i++)
		{
			recvbuf[i] = NULL;
		}

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			for (int i = 0; i < DEFAULT_BUFLEN; i++)
			{
				using namespace std;

				switch (recvbuf[i])
				{
				case 'a': cout << "a";
					break;
				case 'b': cout << "b";
					break;
				case 'c': cout << "c";
					break;
				case 'd': cout << "d";
					break;
				case 'e': cout << "e";
					break;
				case 'f': cout << "f";
					break;
				case 'g': cout << "g";
					break;
				case 'h': cout << "h";
					break;
				case 'i': cout << "i";
					break;
				case 'j': cout << "j";
					break;
				case 'k': cout << "k";
					break;
				case 'l': cout << "l";
					break;
				case 'm': cout << "m";
					break;
				case 'n': cout << "n";
					break;
				case 'o': cout << "o";
					break;
				case 'p': cout << "p";
					break;
				case 'q': cout << "q";
					break;
				case 'r': cout << "r";
					break;
				case 's': cout << "s";
					break;
				case 't': cout << "t";
					break;
				case 'u': cout << "u";
					break;
				case 'v': cout << "v";
					break;
				case 'w': cout << "w";
					break;
				case 'x': cout << "x";
					break;
				case 'y': cout << "y";
					break;
				case 'z': cout << "z";
					break;
				case ' ': cout << " ";
					break;
				case '_': cout << " ";
				}

			}
			printf("\n");
			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				/*closesocket(ClientSocket);
				WSACleanup();*/
				break;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult != 0 and iResult < 1) {
			printf("recv failed with error: %d\n", WSAGetLastError());
			/*closesocket(ClientSocket);
			WSACleanup();*/
			break;
		}

		stringSearch = strstr(recvbuf, "shutdown");
		if (stringSearch != NULL)
		{
			iSendResult = send(ClientSocket, "shutdown", iResult, 0);
			WSACleanup();
			closesocket(ClientSocket);
			shutdown = true;
			break;
		}

	}

	iSendResult = send(ClientSocket, "shutdown", iResult, 0);

	//if thread gets stuck in loop due to client not disconnecting cleanly
	thread_shutdown = true;

	printf("Connection closing...\n");

	// shutdown the connection since we're done
	/*iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
	}*/

	// cleanup
	closesocket(ClientSocket);
	//WSACleanup();
	
	
}

void network_Init(int &iResult, int &iSendResult, SOCKET &ListenSocket, SOCKET &ClientSocket)
{
	WSADATA wsaData;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

}

int __cdecl main(void)
{
	
	int iResult;
	int iSendResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;


	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	//Init WSA and the network sockets
	network_Init(iResult, iSendResult, ListenSocket, ClientSocket);
	
	// Accept a client socket

	std::vector<std::thread> threads;

	bool shutdown = false;
	while (shutdown == false)
	{
		if (ClientSocket = accept(ListenSocket, NULL, NULL))
		{
			threads.push_back(std::thread (&thread_Function, ClientSocket, iSendResult, iResult, recvbuf, recvbuflen, std::ref(shutdown)));
			
		}
		/*if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}*/
		
	} 

	for (int i = 0; i < threads.size(); i++)
	{
		if (threads[i].joinable())
			threads[i].join();

	}
	
	return 0;
}


