#pragma once
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "ErrorHandling.h"
#include "Logger.h"


class TcpListener {
private:
	SOCKET serverListenSocket = INVALID_SOCKET;
	Logger* logger;

public:
	TcpListener(std::wstring port);
	~TcpListener();

	

};

class TcpClient{
private:
	SOCKET server;

public:
	TcpClient();
	~TcpClient();
};