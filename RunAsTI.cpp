#include <iostream>

#include "Utils.h"

BOOL ExecuteCommand(std::wstring& appName, std::wstring& cmdArgs);

int main()
{
	int nArgs;
	int ret = 1;
	LPWSTR* lpArgs = CommandLineToArgvW(GetCommandLineW(), &nArgs);

	if (lpArgs != NULL)
	{
		if (nArgs > 1)
		{
			std::wstring cmdApp;
			std::wstring cmdArgs;
			LPWSTR* lpCommandLine = (lpArgs + 1);
			cmdApp = *lpCommandLine;
			lpCommandLine++;
			while (*lpCommandLine != NULL) {
				if (!cmdArgs.empty()) cmdArgs += L" ";
				cmdArgs += *lpCommandLine;
				lpCommandLine++;
			}

			if(nArgs > 2)
				std::wcout << "Executing '" << cmdApp.c_str() << "' with args '" << cmdArgs.c_str() << "'..." << std::endl;
			else
				std::wcout << "Executing '" << cmdApp.c_str() << "'..." << std::endl;

			if (ExecuteCommand(cmdApp, cmdArgs))
			{
				std::wcout << "SUCCESS!" << std::endl;
				ret = 0;
			}
			else {
				std::wcout << "FAILED!" << std::endl;
			}
		}
		else {
			std::wcout << "Usage: " << *lpArgs << " <cmdline>" << std::endl;
		}
		LocalFree(lpArgs);
	}

	return ret;
}

BOOL ExecuteCommand(std::wstring& appName, std::wstring& cmdArgs)
{
	BOOL success = FALSE;

	if (!Utils::ImpersonateAsSystem())
	{
		return FALSE;
	}

	DWORD dwPID = Utils::StartTIServiceAndGetPID();
	if (dwPID == 0)
	{
		return FALSE;
	}

	HANDLE hToken = Utils::CreateAccessTokenFromPID(dwPID);
	if (hToken != INVALID_HANDLE_VALUE)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		GetStartupInfo(&si);
		success = CreateProcessWithTokenW(hToken, LOGON_WITH_PROFILE, (LPWSTR)appName.c_str(), (LPWSTR)cmdArgs.c_str(), CREATE_UNICODE_ENVIRONMENT, NULL, NULL, &si, &pi);
		if (!success) {
			std::cout << "CreateProcessWithTokenW: " << std::system_category().message(GetLastError()) << std::endl;
		}
	}
	CloseHandle(hToken);

	return success;
}


