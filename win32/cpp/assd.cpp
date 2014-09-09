#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ntdllp.lib")

#pragma comment(linker, "/subsystem:windows")

int
wmain(int iArgc, LPWSTR *iArgv)
{
  HANDLE hEventQuit = CreateEvent(NULL, true, false, "ASSDQUITEVENT");
  if (hEventQuit == NULL)
    return 1;

  if (iArgc > 1)
    if (wcsicmp(iArgv[1], L"/QUIT") == 0)
      return !SetEvent(hEventQuit);

  HANDLE hToken;
  if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
      struct
      {
	DWORD PrivilegeCount;
	LUID_AND_ATTRIBUTES Privileges[2];
      } TP;
      TP.PrivilegeCount = 2;
      LookupPrivilegeValue("", SE_SHUTDOWN_NAME, &(TP.Privileges[0].Luid));
      TP.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
      LookupPrivilegeValue("", SE_REMOTE_SHUTDOWN_NAME,
			   &(TP.Privileges[1].Luid));
      TP.Privileges[1].Attributes = SE_PRIVILEGE_ENABLED;

      AdjustTokenPrivileges(hToken, false, (PTOKEN_PRIVILEGES) &TP, NULL,
			    NULL, NULL);
    }

  for (;;)
    {
      AbortSystemShutdown(NULL);

      if (WaitForSingleObject(hEventQuit, 0) == WAIT_OBJECT_0)
	return 0;

      WaitForSingleObject(hEventQuit, 600000);
    }
}

extern "C" int
WinMainCRTStartup()
{
  int iArgc;
  LPWSTR *iArgv = CommandLineToArgvW(GetCommandLineW(), &iArgc);
  ExitProcess(wmain(iArgc, iArgv));
}
