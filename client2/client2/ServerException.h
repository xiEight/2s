#pragma once
#ifndef SERVEREXCEPTION
#define SERVEREXCEPTION
#include <exception>

class ServerException :
	private std::exception
{
private:
	char* message;
public:
	enum Exceptions { WSAERROR, CLIENTCONNECTERROR, SERVERCONNERROR};
	ServerException(Exceptions errorType);
	const char* what() const override final;
};

#endif // !SERVEREXCEPTION
