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
#include <winstrct.h>

#ifdef _DLL
#pragma comment(lib, "ntdllp.lib")
#endif
#pragma comment(lib, "user32.lib")

#include <stdlib.h>

typedef BOOL (WINAPI *tWow64DisableWow64FsRedirection)(PVOID*);

int
WINAPI
wWinMain(HINSTANCE hCurInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine,
	 int nCmdShow)
{
  STARTUPINFO startup_info = { sizeof(startup_info) };
  PROCESS_INFORMATION process_info;
  PVOID flag;
  DWORD dwExitCode;
  DWORD dwCreationFlags = 0;
  BOOL hInheritHandles = TRUE;

  if (*lpCmdLine == 0)
    {
      MessageBox(NULL,
		 L"Syntax:\r\n"
		 L"\r\n"
		 L"runwait [/64] [/detach] [/hide|/min|/max] cmdline\r\n"
		 L"\r\n"
		 L"/64 - Disable WOW64 filesystem redirections. Use this to "
		 L"run 64 bit system files when running in WOW64.\r\n"
		 L"\r\n"
		 L"/detach - Run console application detached from console "
		 L"window.\r\n"
		 L"\r\n"
		 L"/hide, /min or /max - Window style for new process. Works "
		 L"for both console and GUI applications.\r\n",
		 L"runwait",
		 MB_ICONEXCLAMATION);

      return -1;
    }

  GetStartupInfo(&startup_info);

  if (wcsnicmp(lpCmdLine, L"/64 ", 4) == 0)
    {
      tWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection =
	(tWow64DisableWow64FsRedirection)
	GetProcAddress(GetModuleHandleA("kernel32.dll"),
		       "Wow64DisableWow64FsRedirection");

      if (Wow64DisableWow64FsRedirection != NULL)
	Wow64DisableWow64FsRedirection(&flag);

      lpCmdLine += 4;
    }

  if (wcsnicmp(lpCmdLine, L"/detach ", 8) == 0)
    {
      dwCreationFlags |= DETACHED_PROCESS;

      lpCmdLine += 8;
    }

  if (wcsnicmp(lpCmdLine, L"/hide ", 6) == 0)
    {
      startup_info.dwFlags |= STARTF_USESHOWWINDOW;
      startup_info.wShowWindow = SW_HIDE;
      
      lpCmdLine += 6;
    }
  else if (wcsnicmp(lpCmdLine, L"/min ", 5) == 0)
    {
      startup_info.dwFlags |= STARTF_USESHOWWINDOW;
      startup_info.wShowWindow = SW_MINIMIZE;
      
      lpCmdLine += 5;
    }
  else if (wcsnicmp(lpCmdLine, L"/max ", 5) == 0)
    {
      startup_info.dwFlags |= STARTF_USESHOWWINDOW;
      startup_info.wShowWindow = SW_MAXIMIZE;
      
      lpCmdLine += 5;
    }

  if (!CreateProcess(NULL, lpCmdLine, NULL, NULL, hInheritHandles,
		     dwCreationFlags, NULL, NULL, &startup_info,
		     &process_info))
    {
      DWORD dwLastError = GetLastError();

      MessageBox(NULL, mprintf(L"Error %1!i!: %2%n'%3'.", dwLastError,
			       win_errmsg(dwLastError), lpCmdLine),
		 L"runwait", MB_ICONEXCLAMATION);

      return -1;
    }

  CloseHandle(process_info.hThread);

  if (WaitForSingleObject(process_info.hProcess, INFINITE) != WAIT_OBJECT_0)
    return -1;

  if (!GetExitCodeProcess(process_info.hProcess, &dwExitCode))
    return -1;

  return dwExitCode;
}

int
wWinMainCRTStartup()
{
  LPWSTR lpCmdLine = GetCommandLineW();

  if (*lpCmdLine == L'"')
    do ++lpCmdLine;
    while ((*lpCmdLine != 0) & (*lpCmdLine != L'"'));

  while (*lpCmdLine > L' ')
    ++lpCmdLine;

  while ((*lpCmdLine != 0) & (*lpCmdLine <= L' '))
    ++lpCmdLine;

  ExitProcess(wWinMain(NULL, NULL, lpCmdLine, 0));
}
