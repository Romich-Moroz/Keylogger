#pragma once
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <windows.h>

class Logger {
private:
	std::wofstream output;
	static std::wstring GetTimestamp(std::wstring format);
	int bufSize = 512;
	int written = 0;
public:
	Logger(std::wstring directory, std::wstring filename, int bufSize);
	~Logger();
	void Append(std::wstring message);
	void Append(wchar_t symbol);
	void Flush();
};
