#include "stdafx.h"
#include "printMessage.h"


printMessage::printMessage(char *recvbuf)
{
	message = recvbuf;
	message.shrink_to_fit();
	print(message);
}


printMessage::~printMessage()
{
}

void printMessage::print(std::string message)
{
	std::cout << message << std::endl;

	/*
	for (int i = 0; i < message.length(); i++)
	{
		using namespace std;

		switch (message[i])
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
	printf("\n");*/
}
