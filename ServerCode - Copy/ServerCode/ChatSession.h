//#pragma once
//#include "sqlite3.h"
//#include <iostream>
//#include <thread>
#include <string>
//#include <vector>
//#include <typeinfo>
//#include <windows.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#include <stdlib.h>
//#include <stdio.h>
//
//#include "printMessage.h"

class ChatSession
{
public:
	//Will take user name of person to message as argument an will get both sockets of each user, the server will act as intermediary and pass along messages.
	ChatSession(std::string user);
	~ChatSession();
	
};

