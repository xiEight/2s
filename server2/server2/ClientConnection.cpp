#include "ClientConnection.h"

ClientConnection::ClientConnection(SOCKET client) : socketDescriptor(client)
{
	ioctlsocket(socketDescriptor, FIONBIO, &nnblck);
}

SOCKET ClientConnection::fd() const
{
	return socketDescriptor;
}

ClientConnection& ClientConnection::operator=(ClientConnection const& cc)
{
	if (this == &cc)
		return *this;
	socketDescriptor = cc.socketDescriptor;
	return *this;
}
