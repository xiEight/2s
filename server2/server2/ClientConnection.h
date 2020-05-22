#pragma once
#ifndef CLIENTCONNECTION
#define CLIENTCONNECTION
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996) 
#include <winsock2.h>
#include <string>

class ClientConnection
{
private:
	SOCKET socketDescriptor;
	unsigned long nnblck = 1;
public:
	ClientConnection(SOCKET client);
	SOCKET fd() const;
	ClientConnection& operator=(ClientConnection const& cc);
};
#endif

