// ClientCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include <fstream>
#include <String>
#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <memory>
#include <locale>
#include <codecvt>

#include "wintoastlib.h"

using namespace WinToastLib;


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"



class CustomHandler : public IWinToastHandler {
public:
	void toastActivated() const {
		std::wcout << L"The user clicked in this toast" << std::endl;
		exit(0);
	}

	void toastActivated(int actionIndex) const {
		std::wcout << L"The user clicked on action #" << actionIndex << std::endl;
		exit(16 + actionIndex);
	}

	void toastDismissed(WinToastDismissalReason state) const {
		/*switch (state) {
		case UserCanceled:
			
			break;
		case TimedOut:
			
			break;
		case ApplicationHidden:
			
			break;
		default:
			
			break;
		}*/
	}

	void toastFailed() const {
		std::wcout << L"Error showing current toast" << std::endl;
		exit(5);
	}
};

void printMessage(char *recvbuf)
{
	std::string message(recvbuf);
	message.shrink_to_fit();
	std::cout << message << std::endl;
}


///function for second thread that checks for input from the user for sending to the server
void thread_function(int iResult, const char* sendbuf, SOCKET ConnectSocket)
{
	std::string a = "buffer_placeholder";

	while (a != "shutdown")
	{
		std::getline(std::cin, a);
		sendbuf = a.c_str();
		//Checks to see if shutdown command should be sent.
		if (a != "shutdown") {

			
			iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ConnectSocket);
				WSACleanup();
			}

			//printf("Bytes Sent: %ld\n", iResult);
		}
		else {
			iResult = send(ConnectSocket, "shutdown", (int)strlen(sendbuf), 0);
			printf("Sending shutdown command \n");
			closesocket(ConnectSocket);
		}
	}
}

int __cdecl main(int argc, char **argv)
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	const char *sendbuf = "";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	//check if wintoast notifications compatible
	if (!WinToast::isCompatible())
	{
		std::cout << "Error, your system is not supported!" << std::endl;
	}

	WinToast::instance()->setAppName(L"MessageApp");
	const auto aumi = WinToast::configureAUMI(L"Austen", L"Messager", L"MessagerApp", L"1.0");
	WinToast::instance()->setAppUserModelId(aumi);

	if (!WinToast::instance()->initialize())
	{
		std::cout << "Error, could not init the lib!" << std::endl;
	}

	WinToastTemplate templ = WinToastTemplate(WinToastTemplate::Text01);
	templ.setTextField(L"Notification", WinToastTemplate::FirstLine);
	templ.setAudioOption(WinToastTemplate::AudioOption::Silent);


	// Validate the parameters
	if (argc != 2) {
		printf("usage: %s server-name\n", argv[0]);
		return 1;
	}

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	//Initializes the second thread
	

	std::thread t(&thread_function, iResult, sendbuf, ConnectSocket);

	// shutdown the connection since no more data will be sent
	/*iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
*/
	// Receive until the peer closes the connection
	

	while (recvbuf != "Shutdown") 
	{
		
		
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		

		if (iResult > 0)
		{
			std::unique_ptr<CustomHandler> customHandler(new CustomHandler);
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring wide = converter.from_bytes(recvbuf);
			templ.setTextField(wide, WinToastTemplate::FirstLine);
			if (!WinToast::instance()->showToast(templ, customHandler.get()))
			{
				std::cout << "Error: Could not lauch notification!" << std::endl;
			}
			printf("\33[2K\r");
			printMessage(recvbuf);
			//printf("Bytes received: %d\n", iResult);
		}
		else if (iResult == 0) {
			printf("Connection closed\n");
			break;
		}
		else
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			break;
		}
		
		for (int i = 0; i < sizeof(recvbuf); i++)
		{
			recvbuf[i] = NULL;
		}


	} 
	
	// cleanup
	t.join();
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}

