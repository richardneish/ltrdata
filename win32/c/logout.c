#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

#pragma comment(lib, "advapi32")

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

int WINAPI WinMain(HINSTANCE hCurInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  UINT uFlags = 0;
  BOOL displayhelp = FALSE;
  HANDLE hToken;
  DWORD dwReason = SHTDN_REASON_FLAG_PLANNED;
  TOKEN_PRIVILEGES TP;

  // Nice argument parse loop :)
  if (lpCmdLine ? lpCmdLine[0] ? (lpCmdLine[0]|0x02) == '/' : FALSE : FALSE)
    while ((++lpCmdLine)[0])
      switch (lpCmdLine[0]|0x20)
	{
	case 'f':
	  uFlags |= EWX_FORCE;
	  break;
	case 'l':
	  uFlags |= EWX_LOGOFF;
	  break;
	case 'o':
	  uFlags |= EWX_POWEROFF;
	  break;
	case 'r':
	  uFlags |= EWX_REBOOT;
	  break;
	case 'h':
	  uFlags |= EWX_SHUTDOWN;
	  break;
	default:
	  displayhelp = TRUE;
	}

  if (displayhelp)
    {
      MessageBox(NULL,
		 "LOGOUT. User interface for the Win32 API function ExitWindowsEx.\r\n"
		 "\r\n"
		 "/L\tLog off (default)\r\n"
		 "/H\tShut down\r\n"
		 "/R\tReboot\r\n"
		 "/F\tForce processes to terminate\r\n"
		 "/O\tPower off\r\n"
		 "\r\n", "Logout", MB_ICONINFORMATION | MB_TASKMODAL);
      return 0;
    }

  // I Windows NT måste vi aktivera rättigheten SE_SHUTDOWN_NAME.
  if (WinVer_WindowsNT)
    {
      if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES,
			    &hToken))
	{
	  char *errmsg = win_error;
	  MessageBox( NULL, errmsg, "Logout", MB_ICONSTOP | MB_TASKMODAL);
	  LocalFree(errmsg);
	  return 1;
	}

      TP.PrivilegeCount = 1;
      LookupPrivilegeValue("", SE_SHUTDOWN_NAME, &(TP.Privileges[0].Luid));
      TP.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

      if (!AdjustTokenPrivileges(hToken, FALSE, &TP, 0, NULL, NULL))
	{
	  char *errmsg = win_error;
	  MessageBox(NULL, errmsg, "Logout", MB_ICONSTOP | MB_TASKMODAL);
	  LocalFree(errmsg);
	  return 1;
	}
    }

  // I Win32s kan man inte välja olika utloggningsmetoder. Bara 0 är
  // tillåtet.
  if (WinVer_Win32s)
    {
      uFlags = 0;
      dwReason = 0;
    }

  // Prova att logga ut och se vad som händer. Visa felmeddelande om det
  // inte fungerade.
  if (!ExitWindowsEx(uFlags, dwReason))
    {
      MessageBox(NULL, win_error, "Logout", MB_ICONSTOP | MB_TASKMODAL);
      return 1;
    }
  else
    return 0;
}

