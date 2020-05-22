#include "Query.h"

Query::Query(SOCKET _fd, std::wstring command, size_t index) : fd(_fd), command(command), index(index){}

SOCKET Query::getfd()
{
	return this->fd;
}

std::wstring Query::getCommand()
{
	return command;
}

void Query::sendAnswer()
{
	size_t* length = new size_t;
	*length = sizeof(answer);
	if (send(this->fd, reinterpret_cast<char*>(length), sizeof(size_t), 0) < 0)
		throw ServerException::CLIENTDISCONNECT;
	if (send(this->fd, reinterpret_cast<char*>(&answer), *length, 0) < 0)
		throw ServerException::CLIENTDISCONNECT;
}

void Query::setAnswer(std::wstring answer)
{
	this->answer = answer;
}

size_t Query::getIndex()
{
	return index;
}

std::wstring Query::getAnswer()
{
	return answer;
}