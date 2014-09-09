#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

#pragma comment(lib, "user32.lib")

#if defined(_DLL) && !defined(_WIN64)
#include <minwcrtlib.h>
#pragma comment(lib, "minwcrt.lib")
#else
#include <stdlib.h>
#endif

typedef BOOL (WINAPI *tWow64DisableWow64FsRedirection)(PVOID*);

int
WINAPI
WinMain(HINSTANCE hCurInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
	int nCmdShow)
{
  STARTUPINFO startup_info = { sizeof(startup_info) };
  PROCESS_INFORMATION process_info;
  PVOID flag;
  DWORD dwExitCode;
  DWORD dwCreationFlags = 0;
  BOOL hInheritHandles = TRUE;

  if (lpCmdLine == NULL ? TRUE : *lpCmdLine == 0)
    {
      MessageBox(NULL,
		 "Syntax:\r\n"
		 "\r\n"
		 "runwait [/64] [/detach] [/hide|/min|/max] cmdline\r\n"
		 "\r\n"
		 "/64 - Disable WOW64 filesystem redirections. Use this to "
		 "run 64 bit system files when running in WOW64.\r\n"
		 "\r\n"
		 "/detach - Run console application detached from console "
		 "window.\r\n"
		 "\r\n"
		 "/hide, /min or /max - Window style for new process. Works "
		 "for both console and GUI applications.\r\n",
		 "runwait", MB_ICONEXCLAMATION);

      return -1;
    }

  startup_info.dwFlags |= STARTF_USESHOWWINDOW;
  startup_info.wShowWindow = (WORD) nCmdShow;

  if (strnicmp(lpCmdLine, "/64 ", 4) == 0)
    {
      tWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection =
	(tWow64DisableWow64FsRedirection)
	GetProcAddress(GetModuleHandle("kernel32.dll"),
		       "Wow64DisableWow64FsRedirection");

      if (Wow64DisableWow64FsRedirection != NULL)
	Wow64DisableWow64FsRedirection(&flag);

      lpCmdLine += 4;
    }

  if (strnicmp(lpCmdLine, "/detach ", 8) == 0)
    {
      dwCreationFlags |= DETACHED_PROCESS;

      lpCmdLine += 8;
    }

  if (strnicmp(lpCmdLine, "/hide ", 6) == 0)
    {
      startup_info.dwFlags |= STARTF_USESHOWWINDOW;
      startup_info.wShowWindow = SW_HIDE;
      
      lpCmdLine += 6;
    }
  else if (strnicmp(lpCmdLine, "/min ", 5) == 0)
    {
      startup_info.dwFlags |= STARTF_USESHOWWINDOW;
      startup_info.wShowWindow = SW_MINIMIZE;
      
      lpCmdLine += 5;
    }
  else if (strnicmp(lpCmdLine, "/max ", 5) == 0)
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

      MessageBox(NULL, mprintf("Error %1!i!: %2%n'%3'.", dwLastError,
			       win_errmsg(dwLastError), lpCmdLine),
		 "runwait", MB_ICONEXCLAMATION);

      return -1;
    }

  CloseHandle(process_info.hThread);

  if (WaitForSingleObject(process_info.hProcess, INFINITE) != WAIT_OBJECT_0)
    return -1;

  if (!GetExitCodeProcess(process_info.hProcess, &dwExitCode))
    return -1;

  return dwExitCode;
}
