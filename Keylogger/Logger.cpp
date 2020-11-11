#include "Logger.h"

#pragma once

std::wstring Logger::GetTimestamp()
{
	auto t = std::time(nullptr);
	tm s;
	localtime_s(&s, &t);

	std::wstringstream transTime;
	transTime << std::put_time(&s, L"%Y-%m-%d.log");
	return transTime.str();
}

Logger::Logger(std::wstring directory, std::wstring filename = L"", int bufSize = 512)
{
	this->bufSize = bufSize;
	this->output.open(directory.append(L"\\" + (filename.empty() ? GetTimestamp() : filename)), std::ofstream::out | std::ofstream::app);
}

Logger::~Logger()
{
	this->output.flush();
	this->output.close();
}

void Logger::Append(std::wstring message)
{
	this->output << message;
	this->written += message.length();
	if (this->written >= bufSize) {
		this->output.flush();
		this->written = 0;
	}
		
}

void Logger::Append(wchar_t symbol)
{
	this->output << symbol;
	this->written++;
	if (this->written >= bufSize) {
		this->output.flush();
		this->written = 0;
	}
		
}
