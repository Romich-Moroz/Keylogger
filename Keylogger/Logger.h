#pragma once
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>

class Logger {
private:
	std::wofstream output;
	std::wstring GetTimestamp();
	int bufSize = 512;
	int written = 0;
public:
	Logger(std::wstring directory, std::wstring filename, int bufSize);
	~Logger();
	void Append(std::wstring message);
	void Append(wchar_t symbol);
};
