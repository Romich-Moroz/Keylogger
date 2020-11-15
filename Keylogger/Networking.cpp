#include "Networking.h"

TcpClient::TcpClient(std::wstring ip, std::wstring port, int flushThreshold) {
	this->defaultIp = ip;
	this->defaultPort = port;
	this->flushThreshold = flushThreshold;
}

bool TcpClient::Connect(std::wstring ip = L"", std::wstring port = L"") {
	if (ip.empty())
		ip = this->defaultIp;
	if (port.empty())
		port = this->defaultPort;

	WSADATA wsaData;
	struct addrinfoW* result = NULL, * ptr = NULL, hints = {0};

	int error = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (error) {
		return false;
	}
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	error = GetAddrInfoW(ip.c_str(), port.c_str(), &hints, &result);
	if (error) {
		WSACleanup();
		return false;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		this->connection = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (this->connection == INVALID_SOCKET) {
			WSACleanup();
			return false;
		}

		error = connect(this->connection, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (error == SOCKET_ERROR) {
			closesocket(this->connection);
			this->connection = INVALID_SOCKET;
			continue;
		}
		break;
	}

	FreeAddrInfoW(result);

	if (this->connection == INVALID_SOCKET) {
		WSACleanup();
		return false;
	}

	return true;
}

void TcpClient::Disconnect() {
	closesocket(this->connection);
	WSACleanup();
}

bool TcpClient::Send(std::wstring data) {	
	if (written >= flushThreshold - 1)
		return Flush();
	this->output << data;
	written += (int)data.length();
	return true;
}

bool TcpClient::Send(wchar_t symbol) {
	if (written >= flushThreshold - 1)
		return Flush();
	this->output << symbol;
	written++;	
	return true;
}

bool TcpClient::Flush() {
	this->output << L"\0";
	written++;
	int sendBytes = send(this->connection, (char*)this->output.str().c_str(), written * sizeof(wchar_t), 0);
	if (sendBytes == SOCKET_ERROR) {
		closesocket(this->connection);
		WSACleanup();
		return false;
	}
	written = 0;
	output.str(L"");
	return true;
}
