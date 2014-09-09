#define UNICODE
#define _UNICODE
#ifndef _DLL
#define _DLL
#endif
#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <shellapi.h>
#include <stdlib.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "crtdll.lib")
#endif

int
wmain(int argc, LPWSTR *argv)
{
  HANDLE hToken;
  struct
  {
    DWORD PrivilegeCount;
    LUID_AND_ATTRIBUTES Privileges[2];
  } TP;
  LPWSTR lpMachineName = NULL;

  if (argc > 1)
    lpMachineName = argv[1];

  if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
      TP.PrivilegeCount = 2;
      LookupPrivilegeValue(L"", SE_SHUTDOWN_NAME, &(TP.Privileges[0].Luid));
      TP.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
      LookupPrivilegeValue(L"", SE_REMOTE_SHUTDOWN_NAME,
			   &(TP.Privileges[1].Luid));
      TP.Privileges[1].Attributes = SE_PRIVILEGE_ENABLED;

      AdjustTokenPrivileges(hToken, FALSE, (PTOKEN_PRIVILEGES) &TP, 0, NULL,
			    NULL);
    }

  if (InitiateSystemShutdown(lpMachineName, NULL, 0, TRUE, REBOOT_FLAG))
    return 0;

  win_perror(lpMachineName);
  return 1;
}

int
wmainCRTStartup()
{
  int argc;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
  if (argv == NULL)
    {
      fputs("This program requires Windows NT.\r\n", stderr);
      exit(-1);
    }
  else
    exit(wmain(argc, argv));
}
