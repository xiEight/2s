#include "ServerException.h"

ServerException::ServerException(Exceptions errorType)
{
	switch (errorType)
	{
	case WSAERROR:
		message = const_cast<char*>("WinSock cannot be included!");
		break;
	case CLIENTCONNECTERROR:
		message = const_cast<char*>("Client cannot be connected!");
		break;
	case CLIENTDISCONNECT:
		message = const_cast<char*>("Client disconnect!");
	default:
		break;
	}
}

const char* ServerException::what() const
{
	return message;
}
