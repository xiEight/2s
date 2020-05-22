#include "Client.h"
#include <iostream>
#include <thread>
#include <codecvt>
#include <chrono>
#include <fstream>
#include <locale>
#include <sstream>

std::wstring currentTime()
{
	time_t local = time(NULL);
	tm* commandTime = localtime(&local);
	std::wstringstream currTime;
	currTime << commandTime->tm_hour << ":" << commandTime->tm_min << ":" << commandTime->tm_sec << ": ";
	return currTime.str();
}

void print(std::wstring event)
{
	std::wcout << currentTime() << event.c_str() << std::endl;
}

Client::Client()
{
	DLLVersion = MAKEWORD(2, 2);
	if (WSAStartup(DLLVersion, &wsaData) != 0)
		throw ServerException(ServerException::WSAERROR);
	int size = sizeof(clientAddres);
	clientAddres.sin_addr.s_addr = inet_addr("127.0.0.1");
	clientAddres.sin_port = htons(5555);
	clientAddres.sin_family = AF_INET;
	clientSocket = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(clientSocket, (SOCKADDR*)&clientAddres, sizeof(clientAddres)) != 0)
		throw ServerException(ServerException::CLIENTCONNECTERROR);
	print(L"Connected to Server");
}

void Client::Start()
{
	std::wifstream file("tags.txt");
	std::wstring message;
	size_t* size;
	int recived;
	while (true)
	{
		if (file.eof())
		{
			file.clear();
			file.seekg(0);
		}

		std::getline(file, message);
		size = new size_t;
		*size = sizeof(message);
		print(L"Sending: " + message);
		if (send(clientSocket, reinterpret_cast<char*>(size), sizeof(size_t), 0) < 0)
			throw ServerException(ServerException::SERVERCONNERROR);
		if (send(clientSocket, reinterpret_cast<char*>(&message), *size, 0) < 0)
			throw ServerException(ServerException::SERVERCONNERROR);
		size = new size_t;
		recv(clientSocket, reinterpret_cast<char*>(size), sizeof(size_t), 0);
		recived = 0;
		std::wstring r = L"";
		while (recived != -1 && recived < *size)
		{
			std::wstring* result = new std::wstring;
			recived += recv(clientSocket, reinterpret_cast<char*>(result), *size, 0);
			r += *result;
		}
		print(L"Received: " + r);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}