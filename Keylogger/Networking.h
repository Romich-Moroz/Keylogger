#pragma once
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")


class TcpClient {
private:
	SOCKET connection = INVALID_SOCKET;
	int bufSize = 512;
	int written = 0;
	std::wstring output;

public:
	TcpClient(int bufSize);
	bool Connect(std::wstring ip, std::wstring port);
	void Disconnect();
	bool Send(std::wstring data);
	bool Send(wchar_t symbol);
	bool Flush();
};