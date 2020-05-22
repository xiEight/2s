#include "ServerException.h"

ServerException::ServerException(Exceptions errorType)
{
	switch (errorType)
	{
	case WSAERROR:
		message = const_cast<char*>("WinSock cannot be included");
		break;
	case CLIENTCONNECTERROR:
		message = const_cast<char*>("Client cannot be conncted");
		break;
	case SERVERCONNERROR:
		message = const_cast<char*>("Connection with server lost");
	default:
		break;
	}
}

const char* ServerException::what() const
{
	return message;
}
