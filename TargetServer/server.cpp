#include <WinSock2.h>
#include <string>
#include <ws2tcpip.h>
#include <process.h>
#include <vector>
#include <sstream>
#include <iostream>
#include "Logger.h"

#pragma comment (lib, "Ws2_32.lib")

constexpr int wcharFlushThreashold = 128;

std::wstring gLogDirectory;

SOCKET InitServer(std::wstring address, std::wstring port, int maxConnections) {
	WSADATA wsaData;
	int error = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (error) {
		throw std::exception("WSAstartup failed");
	}

	struct addrinfoW hints = { 0 };
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfoW* result = NULL;

	error = GetAddrInfoW(address.c_str(), port.c_str(), &hints, &result);
	if (error) {
		WSACleanup();
		throw std::exception("GetAddrInfoW failed");
	}

	SOCKET serverListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (serverListenSocket == INVALID_SOCKET) {
		int errCode = WSAGetLastError();
		FreeAddrInfoW(result);
		WSACleanup();
		throw std::exception("socket failed with error: %ld", errCode);
	}

	error = bind(serverListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (error == SOCKET_ERROR) {
		int errCode = WSAGetLastError();
		FreeAddrInfoW(result);
		closesocket(serverListenSocket);
		WSACleanup();
		throw std::exception("bind failed with error : % d\n", errCode);
	}
	FreeAddrInfoW(result);

	

	error = listen(serverListenSocket, maxConnections);
	if (error == SOCKET_ERROR) {
		int errCode = WSAGetLastError();
		closesocket(serverListenSocket);
		WSACleanup();
		throw std::exception("listen failed with error: %d\n", errCode);
	}

	return serverListenSocket;
}

static unsigned int _stdcall ClientHandler(void* socket) {
	auto clientSocket = (SOCKET)socket;
	Logger logger(gLogDirectory, std::to_wstring(clientSocket) + L".log", wcharFlushThreashold);

	wchar_t buf[wcharFlushThreashold + 1] = {0};
	int bytesRecieved = 0;
	while ((bytesRecieved = recv(clientSocket, (char*)buf, wcharFlushThreashold*2, 0)) > 0) {
		logger.Append(std::wstring((wchar_t*)buf));
		ZeroMemory(buf, wcharFlushThreashold + 1);
	}
	std::wcout << L"Client (id = " << clientSocket << L") disconnected" << std::endl;
	closesocket(clientSocket);

	logger.Flush();
	return 0;
}

SOCKET gServerSocket;

static unsigned int _stdcall ConnectionManager(void* dummy) {
	std::vector<HANDLE> threads;
	std::wcout << L"Listening for connections..." << std::endl;
	SOCKET clientSocket = INVALID_SOCKET;
	while ((clientSocket = accept(gServerSocket, NULL, NULL)) != INVALID_SOCKET) {
		std::wcout << L"Client (id = "<< clientSocket <<L") connected"  << std::endl;
		threads.push_back((HANDLE)_beginthreadex(0, 0, ClientHandler, (void*)clientSocket, 0, 0));
	}	
	if (threads.size() != 0) {
		WaitForMultipleObjects(threads.size(), &threads[0], TRUE, INFINITE);
		for (int i = 0; i < threads.size(); i++)
			CloseHandle(threads[i]);
	}
		
	return 0;
}


std::wstring GetCurrentDirectory(std::wstring executable) {	
	return executable.substr(0, executable.find_last_of(L'\\'));
}

int wmain(int argc, wchar_t* argv[]) {
	if (argc < 4) {
		MessageBox(NULL, L"Pass address, port and max connections limit as params to this executable", NULL, 0);
		return 0;
	}
	std::locale::global(std::locale("ru_RU.UTF-8"));		
	std::wistringstream ss(argv[3]);
	int maxConnections = 1;
	ss >> maxConnections;
	gLogDirectory = GetCurrentDirectory(argv[0]) + L"\\ClientLogs\\";

	gServerSocket = InitServer(argv[1], argv[2], maxConnections);
	std::wcout << L"Server running at " << argv[1] << L":" << argv[2] << std::endl;

	HANDLE hConnectionManager = (HANDLE)_beginthreadex(0, 0, ConnectionManager, 0, 0, 0);
	if (hConnectionManager) {
		std::wstring command;
		while (true) //command loop
		{
			std::wcin >> command;
			if (command == L"exit") {
				shutdown(gServerSocket, SD_RECEIVE);
				closesocket(gServerSocket);
				WSACleanup();
				WaitForSingleObject(hConnectionManager, INFINITE);				
				CloseHandle(hConnectionManager);
				break;
			}
		}
	}

	return 0;
}