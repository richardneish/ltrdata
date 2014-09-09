#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

#pragma comment(lib, "advapi32.lib")

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

int WINAPI
WinMain(HINSTANCE hCurInstance, HINSTANCE hPrv, LPSTR lpCmdLine, int nCmdShow)
{
  STARTUPINFO si = { sizeof(si) };
  PROCESS_INFORMATION pi;

  if (lpCmdLine == NULL ? TRUE : *lpCmdLine == 0)
    {
      MessageBox(NULL,
		 "Usage syntax: logonwrap <command line>\r\n\n"
		 "Runs the specified command line and waits for the process "
		 "to terminate and then logs out the user session.",
		 "logonwrap",
		 MB_ICONINFORMATION);
      return 0;
    }

  si.dwFlags = STARTF_USESHOWWINDOW;
  si.wShowWindow = (WORD) nCmdShow;

  if (CreateProcess(NULL, lpCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si,
		    &pi))
    WaitForSingleObject(pi.hProcess, INFINITE);
  else
    {
      DWORD dwLastError = GetLastError();

      MessageBox(NULL, mprintf("Error %1!i!: %2%n'%3'.", dwLastError,
			       win_errmsg(dwLastError), lpCmdLine),
		 "logonwrap", MB_ICONEXCLAMATION);
    }

  if (!ExitWindows(0, 0))
    {
      MessageBox(NULL, win_error, "logonwrap", MB_ICONSTOP);
      return 0;
    }

  return 1;
}
