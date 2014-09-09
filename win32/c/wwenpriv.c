#define WIN32_LEAN_AND_MEAN
#include <wntsecur.h>

EXTERN_C BOOL EnablePrivilegeW(LPCWSTR lpPrivName)
{
  HANDLE hToken;
  TOKEN_PRIVILEGES TP;

  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
    return FALSE;

  TP.PrivilegeCount = 1;
  LookupPrivilegeValueW(L"", lpPrivName, &(TP.Privileges[0].Luid));
  TP.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

  if (!AdjustTokenPrivileges(hToken, FALSE, &TP, 0, NULL, NULL))
    {
      DWORD dwErrNo = GetLastError();
      CloseHandle(hToken);
      SetLastError(dwErrNo);
      return FALSE;
    }

  CloseHandle(hToken);
  return TRUE;
}
