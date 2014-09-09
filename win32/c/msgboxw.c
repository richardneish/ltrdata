#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef _DLL
#define _DLL
#endif

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>

#pragma comment(lib, "ntdllp.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(linker, "/subsystem:windows")

int
wmain(int argc, LPWSTR *argv)
{
  return MessageBox(NULL,
		    argc > 1 ? argv[1] : L"",
		    argc > 3 ? argv[3] : NULL,
		    argc > 2 ? wcstoul(argv[2], NULL, 0) : 0);
}

int
WinMainCRTStartup()
{
  int iArgc;
  LPWSTR *wszArgv = CommandLineToArgvW(GetCommandLineW(), &iArgc);
  if (wszArgv == NULL)
    {
      MessageBoxA(NULL,
		  "This program requires Windows NT.",
		  "msgboxW",
		  MB_ICONSTOP);
      ExitProcess((DWORD) -1);
    }

  ExitProcess(wmain(iArgc, wszArgv));
}
