#include "Networking.h"

TcpClient::TcpClient(int bufSize) {
	this->bufSize = bufSize;
}

bool TcpClient::Connect(std::wstring ip, std::wstring port) {
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
	for (int i = 0; i = (int)data.size(); i++) {
		this->output[written + i] = data[i];
	}
	written += (int)data.length();
	if (written >= bufSize)
		return Flush();
	return true;
}

bool TcpClient::Send(wchar_t symbol)
{
	this->output[written] = symbol;
	written++;
	if (written >= bufSize)
		return Flush();
	return true;
}

bool TcpClient::Flush()
{
	int error = send(this->connection, (char*)this->output.c_str(), this->output.size() * sizeof(wchar_t), 0);
	if (error == SOCKET_ERROR) {
		closesocket(this->connection);
		WSACleanup();
		return false;
	}
	written = 0;
	return true;
}
