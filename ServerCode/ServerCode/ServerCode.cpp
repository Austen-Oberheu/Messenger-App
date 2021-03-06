// ServerCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "printMessage.h"
#include "ChatSession.h"
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include "sqlite3.h"
#include <fstream>
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

std::string Login(std::string, bool&);


void thread_Function(SOCKET ClientSocket, int iSendResult, int iResult, char* recvbuf, int recvbuflen, std::vector<std::pair<std::string, SOCKET>> &userInfo, bool &shutdown)
{

	bool validUser = false;
	char *stringSearch;
	bool thread_shutdown = false;
	std::string sendbuf;
	int maxAttempts = 5;
	int attempts = 0;

	iSendResult = send(ClientSocket, "enter username", 15, 0);
	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);

	std::string username(recvbuf);
	username.shrink_to_fit();
	Login(username, validUser);

	while (validUser == false) {
		if (attempts > maxAttempts)
		{
			return;
		}
		iSendResult = send(ClientSocket, "invalid username, enter username", 33, 0);
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		std::string username(recvbuf);
		username.shrink_to_fit();
		Login(username, validUser);
		attempts++;
	}

	sendbuf = ("Welcome " + username + "!");
	iSendResult = send(ClientSocket, sendbuf.c_str(), sendbuf.length(), 0);

	userInfo.push_back(std::make_pair(username, ClientSocket));

	// Receive until the peer shuts down the connection
	while ((shutdown == false) && (thread_shutdown == false))
	{
		for (int i = 0; i < DEFAULT_BUFLEN; i++)
		{
			recvbuf[i] = NULL;
		}

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);

			printf("%s: ", username.c_str());
			printMessage printmessage(recvbuf);

			//Send the message to the recipent if message proceeded with / followed by username

			if (recvbuf[0] == '/')
			{
				ChatSession chatSession(username, recvbuf, userInfo, ClientSocket);
				
			}
			
			//iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				/*closesocket(ClientSocket);
				WSACleanup();*/
				break;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult != 0 && iResult < 1) {
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


	// cleanup
	closesocket(ClientSocket);
	//WSACleanup();
	return;
	
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

static int callbackPassword(void *validUser_ref, int count, char **data, char **columns)
{
	bool *validUser = static_cast<bool*>(validUser_ref);

	if (count != 0)
	{
		for (int i = 0; i < count; i++)
		{
			printf("%s = %s\n", columns[i], data[i] ? data[i] : "NULL");

		}
		printf("\n");
		*validUser = true;
		return 0;
	}else
	{
		
	}

	return 0;
}

std::string Login(std::string username, bool &validUser_ref/*,TODO std::string password*/)
{
	sqlite3 *db;
	std::string sql;
	int sqlError;
	char *errorMessage = nullptr;
	
	

	sqlError = sqlite3_open("server_database.db", &db);

	if (sqlError) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return NULL;
	}
	else {
		fprintf(stdout, "Opened database successfully\n");
	}

	//TODO SQL table creation move to a first start function 
	/*sql = "CREATE TABLE IF NOT  EXISTS USERS(" \
		"ID INT PRIMARY KEY		NOT NULL," \
		"USERNAME		TEXT	NOT NULL," \
		"PASSWORD		TEXT	NOT NULL);";

	sqlError = sqlite3_exec(db, sql.c_str(), callback, 0, &errorMessage);
	if (sqlError != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
	}
	else {
		fprintf(stdout, "Table created successfully\n");
	}*/


	sql = ("SELECT * FROM USERS WHERE USERNAME = '"+ username +"';");

	sqlError = sqlite3_exec(db, sql.c_str(), callbackPassword, &validUser_ref, &errorMessage);
	if (sqlError != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
	}
	else {
		fprintf(stdout, "Command ran successfully\n");
	}

	sqlite3_close(db);
	
	return username;
}

int __cdecl main(void)
{

	int iResult;
	int iSendResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;


	char recvbuf[DEFAULT_BUFLEN] = {NULL};
	int recvbuflen = DEFAULT_BUFLEN;


	//Init WSA and the network sockets
	network_Init(iResult, iSendResult, ListenSocket, ClientSocket);
	
	// Accept a client socket

	std::vector<std::thread> threads;

	std::vector<std::pair<std::string, SOCKET>> userInfo;

	bool shutdown = false;
	while (shutdown == false)
	{
		if (ClientSocket = accept(ListenSocket, NULL, NULL))
		{
			threads.push_back(std::thread (&thread_Function, ClientSocket, iSendResult, iResult, recvbuf, recvbuflen, std::ref(userInfo),std::ref(shutdown)));
			
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


