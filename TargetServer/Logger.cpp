#include "Logger.h"

#pragma once

std::wstring Logger::GetTimestamp(std::wstring format) {
	auto t = std::time(nullptr);
	tm s;
	localtime_s(&s, &t);

	std::wstringstream transTime;
	transTime << std::put_time(&s, format.c_str());
	return transTime.str();
}

Logger::Logger(std::wstring directory = L"", std::wstring filename = L"", int bufSize = 512) {	
	this->bufSize = bufSize;
	filename = filename.empty() ? GetTimestamp(L"%Y-%m-%d") + L".log" : filename;
	if (CreateDirectory(directory.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
		
	{
		this->output.open(directory.append(filename), std::ofstream::out | std::ofstream::app);
	}
	
}

Logger::~Logger() {
	Flush();
	this->output.close();
}

void Logger::Append(std::wstring message) {
	this->output << message;
	this->written += (int)message.length();
	if (this->written >= bufSize)
		Flush();		
}

void Logger::Append(wchar_t symbol) {
	this->output << symbol;
	this->written++;
	if (this->written >= bufSize)
		Flush();	
}

void Logger::Flush() {
	this->output.flush();
	this->written = 0;
}
