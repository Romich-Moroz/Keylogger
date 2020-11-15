#pragma once
#include <string>
#include <sstream>
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")


class TcpClient {
private:
	SOCKET connection = INVALID_SOCKET;
	int flushThreshold = 512;
	int written = 0;
	std::wstringstream output;
	std::wstring defaultIp, defaultPort;

public:
	TcpClient(std::wstring ip, std::wstring port, int flushThreshold);
	bool Connect(std::wstring ip, std::wstring port);
	void Disconnect();
	bool Send(std::wstring data);
	bool Send(wchar_t symbol);
	bool Flush();
};