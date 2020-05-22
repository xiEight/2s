#ifndef QUERY
#define QUERY
#include "ClientConnection.h"
#include "ServerException.h"
class Query
{
private:
	SOCKET fd;
	std::wstring command;
	std::wstring answer;
	size_t index;
public:
	Query(SOCKET _fd, std::wstring command, size_t index);
	SOCKET getfd();
	std::wstring getCommand();
	std::wstring getAnswer();
	void sendAnswer();
	void setAnswer(std::wstring answer);
	size_t getIndex();
};

#endif // !QUERY
