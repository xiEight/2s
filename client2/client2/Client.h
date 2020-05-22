#pragma once
#ifndef CLIENT
#define CLIENT
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996) 
#include <winsock2.h>
#include "ServerException.h"
#include <string>
class Client
{
private:
	WSAData wsaData;
	WORD DLLVersion;
	SOCKADDR_IN clientAddres;
	SOCKET clientSocket;
	unsigned long nnblck = 1;
public:
	Client();
	void Start();

};
#endif

