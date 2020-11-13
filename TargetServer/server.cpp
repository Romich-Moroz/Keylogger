#include <WinSock2.h>
#include <string>
#include <ws2tcpip.h>
#include <process.h>
#include <vector>
#include <sstream>
#include <iostream>
#include "Logger.h"

#pragma comment (lib, "Ws2_32.lib")

std::wstring gLogDirectory;

SOCKET InitServer(std::wstring port, int maxConnections) {
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

	error = GetAddrInfoW(NULL, port.c_str(), &hints, &result);
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
	Logger logger(gLogDirectory, std::to_wstring(clientSocket), 2048);

	char byteBuf[4096];
	while (int bytesRecieved = recv(clientSocket, byteBuf, 4096, 0) > 0) {
		logger.Append((wchar_t*)byteBuf);
		ZeroMemory(byteBuf, 4096);
	}
	closesocket(clientSocket);
	WSACleanup();

	return 0;
}

std::wstring GetCurrentDirectory(std::wstring executable) {
	return executable.substr(0, executable.find_last_of(L'\\'));
}

int wmain(int argc, wchar_t* argv[]) {
	std::wistringstream ss(argv[2]);
	int maxConnections = 1;
	ss >> maxConnections;

	gLogDirectory = GetCurrentDirectory(argv[0]) + L"\\ClientLogs\\";

	SOCKET serverListenSocket = InitServer(argv[1], maxConnections);

	std::vector<HANDLE> threads;
	std::wcout << L"Listening for connections..." << std::endl;

	while (SOCKET clientSocket = accept(serverListenSocket,NULL,NULL) != INVALID_SOCKET) {
		std::wcout << L"Client connected" << std::endl;
		threads.push_back((HANDLE)_beginthreadex(0, 0, ClientHandler, (void*)clientSocket, 0, 0));
	}

	closesocket(serverListenSocket);
	WSACleanup();
	WaitForMultipleObjects(threads.size(), &threads[0], TRUE, INFINITE);
	return 0;
}