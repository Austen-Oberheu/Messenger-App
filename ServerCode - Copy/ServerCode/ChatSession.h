#pragma once
#define WIN32_LEAN_AND_MEAN

#include "printMessage.h"


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

class ChatSession
{
public:
	//Will take user name of person to message as argument an will get both sockets of each user, the server will act as intermediary and pass along messages.
	ChatSession(std::string user, char* &recvbuf, std::vector<std::pair<std::string, SOCKET>> &userInfo, SOCKET ClientSocket);
	~ChatSession();

	void SendToUser(std::string user, char* &recvbuf, std::vector<std::pair<std::string, SOCKET>> &userInfo, SOCKET ClientSocket);
	
};

