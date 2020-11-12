#include "Networking.h"
#pragma once

TcpListener::TcpListener(std::wstring port) {
	WSADATA wsaData;
	int error = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (error) {
		throw std::exception("WSAstartup failed");
	}

	struct addrinfoW hints = {0};
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

	serverListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (serverListenSocket == INVALID_SOCKET) {
		int errCode = WSAGetLastError();
		FreeAddrInfoW(result);
		WSACleanup();
		throw std::exception("socket failed with error: %ld",errCode);
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
}
