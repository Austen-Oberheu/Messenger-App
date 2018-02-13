#pragma once
#include <string>
#include <iostream>
class printMessage
{
public:
	printMessage(char *recvbuf);
	~printMessage();

private:
	void print(std::string message);

private:
	std::string message;
};

