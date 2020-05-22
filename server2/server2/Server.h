#pragma once
#ifndef SERVER
#define SERVER
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996) 
#include <winsock2.h>
#include "ServerException.h"
#include "ClientConnection.h"
#include "Query.h"
#include <algorithm>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <ostream>
#include <iostream>
#define POLL_SIZE 100

class Server
{
private:
	WSAData wsaData;
	WORD DLLVersion;
	SOCKADDR_IN serverAdress;
	SOCKET serverSocket;
	std::vector<ClientConnection> clientConnections;
	std::queue<Query> queryQueue;
	unsigned long nnblck = 1;
	std::mutex mtx;
	std::mutex outputMutex;
	std::mutex vectorMutex;
	std::condition_variable cond;
	std::ostream& defaultOuput = std::cout;
	bool queueStatus();
	bool queueReady = false;
	void queueThread();
public:
	Server();
	void Start();
};
#endif

