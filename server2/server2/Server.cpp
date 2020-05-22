#include "Server.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>
#include <codecvt>

std::wstring currentTime()
{
	time_t local = time(NULL);
	tm* commandTime = localtime(&local);
	std::wstringstream currTime;
	currTime << commandTime->tm_hour << ":" << commandTime->tm_min << ":" << commandTime->tm_sec << ": ";
	return currTime.str();
}

std::wstring stringToW(std::string s)
{
	return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(s.c_str());
}


void print(std::wstring event)
{
	std::wcout<<currentTime()<< event.c_str() << std::endl;
}

std::wstring XMLTag(std::wstring tag, std::wstring filePath = L"tags.xml")
{
	std::wifstream file(filePath);
	std::wstring str;
	while (getline(file, str))
	{
		if (str.find(tag) != std::wstring::npos)
		{
			file.close();
			std::wstring ss = str.substr(str.find_first_of(L">") + 1, str.find_last_of(L"<") - str.find_first_of(L">") - 1);
			return ss;
		}
	}
	file.close();
	return std::wstring(L"Wrong Tag!");
}

bool Server::queueStatus()
{
	queueReady = !queryQueue.empty();
	return queueReady;
}

void Server::queueThread()
{
	outputMutex.lock();
	print(std::wstring(L"Queue Thread started"));
	outputMutex.unlock();

	while (true)
	{
		std::unique_lock<std::mutex> ulm(mtx);
		cond.wait(ulm, [this]() {
			return !queryQueue.empty(); });
		queryQueue.front().setAnswer(XMLTag(queryQueue.front().getCommand()));
		try {
			queryQueue.front().sendAnswer();
			print(L"Sending: " + queryQueue.front().getAnswer());
		}
		catch (ServerException const& ex)
		{
			print(stringToW(ex.what()));
			vectorMutex.lock();
			clientConnections.erase(clientConnections.begin() + queryQueue.front().getIndex());
			vectorMutex.unlock();
		}
		queryQueue.pop();
	}
}

Server::Server()
{
	DLLVersion = MAKEWORD(2, 2);
	if (WSAStartup(DLLVersion, &wsaData) != 0)
		throw ServerException(ServerException::WSAERROR);
	serverAdress.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAdress.sin_port = htons(5555);
	serverAdress.sin_family = AF_INET;
	serverSocket = socket(AF_INET, SOCK_STREAM, NULL);
	ioctlsocket(serverSocket, FIONBIO, &nnblck);
	bind(serverSocket, (SOCKADDR*)&serverAdress, sizeof(serverAdress));
}

void Server::Start()
{
	std::thread queuetrd([this]()
		{
			queueThread();
		}
	);
	queuetrd.detach();
	listen(serverSocket, POLL_SIZE);
	outputMutex.lock();
	print(std::wstring(L"Server started"));
	outputMutex.unlock();
	struct pollfd Set[POLL_SIZE];
	Set[0].fd = serverSocket;
	Set[0].events = POLLIN;
	int size = sizeof(serverAdress);
	SOCKET newClientConnection;
	unsigned index, setSize;
	//POLL
	while (true)
	{
		index = 1;
		for (auto x : clientConnections)
		{
			Set[index].fd = x.fd();
			Set[index].events = POLLIN;
			index++;
		}
		setSize = 1 + clientConnections.size();
		WSAPoll(Set, setSize, 0);
		for (unsigned i = 0; i < setSize; i++)
		{
			if (Set[i].revents & POLLHUP)
			{
				//DISCONNECT
				outputMutex.lock();
				print(std::wstring(L"Client disconnected"));
				outputMutex.unlock();
				shutdown(Set[i].fd, SD_BOTH);
				closesocket(Set[i].fd);
				clientConnections.erase(clientConnections.begin() + i - 1);
			}

			if (Set[i].revents & POLLIN)
			{
				size_t* size = new size_t;
				if (i)
				{
					//RECIVING MESSAGE
					int RecvSize = recv(Set[i].fd, reinterpret_cast<char*>(size), sizeof(size_t), 0);
					if ((RecvSize < 0) && errno != EAGAIN)
					{
						// DISCONNECTING
						outputMutex.lock();
						print(std::wstring(L"Client disconnected"));
						outputMutex.unlock();
						shutdown(Set[i].fd, SD_BOTH);
						closesocket(Set[i].fd);
						clientConnections.erase(clientConnections.begin() + i);
					}
					else if (RecvSize > 0)
					{
						std::wstring strng = L"";
						int recived = 0;
						std::this_thread::sleep_for(std::chrono::milliseconds(20));
						while (recived != -1 && recived < *size)
						{
							std::wstring* w = new std::wstring;
							recived += recv(Set[i].fd, reinterpret_cast<char*>(w), *size, 0);
							strng += *w;
						}
						//SENDING ANSWER
						outputMutex.lock();
						print(L"Received: " + strng);
						outputMutex.unlock();
						queryQueue.push(Query(Set[i].fd,
							std::wstring(strng), i - 1));
						cond.notify_all();
					}
				}
				else
				{
					//CREATING NEW CONNECTION
					newClientConnection = accept(serverSocket, 0, 0);
					vectorMutex.lock();
					clientConnections.emplace_back(newClientConnection);
					vectorMutex.unlock();
					outputMutex.lock();
					print(std::wstring(L"Client connected!"));
					outputMutex.unlock();
				} 
			}
		}
	}
}