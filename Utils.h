#pragma once

#include <windows.h>

namespace Utils {
	HANDLE CreateAccessTokenFromPID(DWORD dwProcessId);
	DWORD GetPIDFromProcessName(LPCWSTR lpProcessName);
	BOOL ImpersonateAsSystem();
	DWORD StartTIServiceAndGetPID();
}
