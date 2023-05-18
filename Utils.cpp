#include "Utils.h"

#include <iostream>
#include <TlHelp32.h>

namespace Utils {
	HANDLE CreateAccessTokenFromPID(DWORD dwProcessId)
	{
		HANDLE hDupToken = INVALID_HANDLE_VALUE;
		HANDLE hProc = OpenProcess(MAXIMUM_ALLOWED, FALSE, dwProcessId);
		if (hProc)
		{
			HANDLE hToken = INVALID_HANDLE_VALUE;
			if (OpenProcessToken(hProc, TOKEN_DUPLICATE, &hToken))
			{
				SECURITY_ATTRIBUTES attrs = {
					sizeof(SECURITY_ATTRIBUTES),
					NULL,
					FALSE,
				};

				if (!DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, &attrs, SecurityImpersonation, TokenImpersonation, &hDupToken))
				{
					std::cout << "DuplicateTokenEx: " << std::system_category().message(GetLastError()) << std::endl;
					hDupToken = INVALID_HANDLE_VALUE;
				}
			}
			else {
				std::cout << "OpenProcessToken: " << std::system_category().message(GetLastError()) << std::endl;
			}
			CloseHandle(hToken);
			CloseHandle(hProc);
		}
		else {
			std::cout << "OpenProcess: " << std::system_category().message(GetLastError()) << std::endl;
		}

		return hDupToken;
	}

	DWORD GetPIDFromProcessName(LPCWSTR lpProcessName)
	{
		PROCESSENTRY32 pe32 = {};
		DWORD pid = 0;

		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (hSnap != INVALID_HANDLE_VALUE)
		{
			pe32.dwSize = sizeof(PROCESSENTRY32);

			if (Process32First(hSnap, &pe32))
			{
				do
				{
					if (!_wcsicmp(lpProcessName, pe32.szExeFile))
					{
						pid = pe32.th32ProcessID;
						break;
					}
				} while (Process32Next(hSnap, &pe32));
			}
			CloseHandle(hSnap);
		}
		else {
			std::cout << "CreateToolhelp32Snapshot: " << std::system_category().message(GetLastError()) << std::endl;
		}

		return pid;
	}

	BOOL ImpersonateAsSystem()
	{
		BOOL success = FALSE;

		DWORD dwPID = GetPIDFromProcessName(L"winlogon.exe");
		if (dwPID != 0)
		{
			HANDLE hToken = CreateAccessTokenFromPID(dwPID);
			if (hToken != INVALID_HANDLE_VALUE)
			{
				success = ImpersonateLoggedOnUser(hToken);
				if (!success) {
					std::cout << "ImpersonateLoggedOnUser: " << std::system_category().message(GetLastError()) << std::endl;
				}
				CloseHandle(hToken);
			}
		}
		else {
			std::cout << "Impersonation Helper Process could not be found!" << std::endl;
		}

		return success;
	}

	DWORD StartTIServiceAndGetPID()
	{
		const DWORD SLEEP_INTERVAL = 100;
		DWORD dwLoopCount = 50; //50 * 100ms = 5s
		DWORD dwPID = 0;

		SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
		if (hSCM)
		{
			SC_HANDLE hService = OpenService(hSCM, L"TrustedInstaller", SERVICE_START | SERVICE_QUERY_STATUS);
			if (hService)
			{
				SERVICE_STATUS_PROCESS ssp = {};
				DWORD dwBytesNeeded;

				do
				{
					if (QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
					{
						if (ssp.dwCurrentState == SERVICE_RUNNING)
						{
							dwPID = ssp.dwProcessId;
							break;
						}
						else if (ssp.dwCurrentState == SERVICE_STOPPED)
						{
							if (!StartService(hService, 0, NULL))
							{
								std::cout << "StartService: " << std::system_category().message(GetLastError()) << std::endl;
								break;
							}
						}
					}
					else {
						std::cout << "QueryServiceStatusEx: " << std::system_category().message(GetLastError()) << std::endl;
					}
					Sleep(SLEEP_INTERVAL);
				} while (--dwLoopCount > 0);

				CloseServiceHandle(hService);
			}
			else {
				std::cout << "OpenService: " << std::system_category().message(GetLastError()) << std::endl;
			}
			CloseServiceHandle(hSCM);
		}
		else {
			std::cout << "OpenSCManager: " << std::system_category().message(GetLastError()) << std::endl;
		}

		return dwPID;
	}
}