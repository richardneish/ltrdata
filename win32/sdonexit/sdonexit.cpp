#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include "sdonexit.rc.h"

#pragma comment(lib, "advapi32")

#ifdef _DLL
#pragma comment(lib, "minwcrt")
#endif

BOOL CALLBACK
DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
    {
    case WM_COMMAND:
      EndDialog(hWnd, LOWORD(wParam));
      return TRUE;

    case WM_INITDIALOG:
      return TRUE;

    default:
      return FALSE;
    }
}

int WINAPI
WinMain(HINSTANCE hCurInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
  PROCESS_INFORMATION pi;
  HANDLE hToken;

  if (lpCmdLine == NULL)
    return 0;

  if (*lpCmdLine == 0)
    return 0;

  for (;;)
    {
      STARTUPINFO si = { 0 };

      // MSTSC (eller annan lämplig TS-klient) startas.
      si.dwFlags = STARTF_USESHOWWINDOW;
      si.wShowWindow = (WORD) nCmdShow;

      if (!CreateProcess(NULL, lpCmdLine, NULL, NULL, FALSE, 0, NULL, NULL,
			 &si, &pi))
	return 1;

      // Vänta tills TS-klienten har avslutats.
      WaitForSingleObject(pi.hProcess, INFINITE);

      // Fråga användaren om klientmaskinen ska stängas av.
      switch (DialogBox(hCurInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL,
			DialogProc))
	{
	case IDOK:
	  // Stäng av klientmaskinen.
	  if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES,
			       &hToken))
	    {
	      struct
	      {
		DWORD PrivilegeCount;
		LUID_AND_ATTRIBUTES Privileges[1];
	      } TP;
	      TP.PrivilegeCount = 1;

	      LookupPrivilegeValue("", SE_SHUTDOWN_NAME,
				   &(TP.Privileges[0].Luid));
	      TP.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	      AdjustTokenPrivileges(hToken, FALSE, (PTOKEN_PRIVILEGES) & TP,
				    0, NULL, NULL);
	    }

	  if (!ExitWindowsEx(WinVer_Win2korXP ? EWX_POWEROFF : EWX_SHUTDOWN,
			     0))
	    {
	      char *errmsg = win_error;
	      MessageBox(NULL, errmsg, "Det går inte att stänga av datorn.",
			 MB_ICONSTOP | MB_TASKMODAL);
	      LocalFree(errmsg);
	      continue;
	    }

	  return 0;

	case IDHELP:
	  if (!ExitWindowsEx(EWX_LOGOFF, 0))
	    {
	      char *errmsg = win_error;
	      MessageBox(NULL, errmsg, "Det går inte att logga ut.",
			 MB_ICONSTOP | MB_TASKMODAL);
	      LocalFree(errmsg);
	      return 1;
	    }

	  return 0;

	case IDCANCEL:
	  continue;
	}
    }
}
