#include "stdafx.h"
#include "ChatSession.h"


ChatSession::ChatSession(std::string user, char* &recvbuf, std::vector<std::pair<std::string, SOCKET>> &userInfo, SOCKET ClientSocket)
{
	SendToUser(user, recvbuf, userInfo, ClientSocket);
}

ChatSession::~ChatSession()
{
}

void ChatSession::SendToUser(std::string username, char* &recvbuf, std::vector<std::pair<std::string, SOCKET>> &userInfo, SOCKET ClientSocket)
{

	int x = 1; //This is probably the issue. Will have to change this to work with both the usersearch and the removal of the command from the sen
	std::string recvbufstr(recvbuf);
	std::vector<char>userSearch;
	recvbufstr[0] = NULL; //Due to X being 1 this is never removed so this line takes care of that.
	while (recvbuf[x] != ' ')
	{
		userSearch.emplace_back(recvbuf[x]);
		recvbufstr[x] = NULL;
		x++;
	}

	std::string message((recvbufstr.begin() + x), recvbufstr.end());
	message.resize((message.size() + username.size() + 1));
	message.insert(0, (username + ":"));

	std::string userSearchString(userSearch.begin(), userSearch.end());
	for (int i = 0; i < userInfo.size(); i++)
	{
		if (userSearchString == userInfo[i].first)
		{

			//std::string sendstr(username + ":", recvbuf);
				send(userInfo[i].second, message.c_str(), message.length(), 0);
			while (recvbuf[0] != '!')
			{
				recv(ClientSocket, recvbuf, 512, 0);
				send(userInfo[i].second, recvbuf, 512, 0);
			}
		}
	}
}
