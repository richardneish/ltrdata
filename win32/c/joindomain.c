#define UNICODE
#define _UNICODE
#ifndef _DLL
#define _DLL
#endif
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <minwcrtlib.h>

#include <winstrct.h>
#include <lm.h>
#include <shellapi.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "netapi32.lib")

int
wmain(int iArgc, LPWSTR *lpwArgv)
{
  DWORD dwErrRet;
  LPWSTR wczAccountOU;
  LPWSTR wczComputerName;

  SetOemPrintFLineLength(GetStdHandle(STD_ERROR_HANDLE));

  if ((iArgc < 3) | (iArgc > 5))
    {
      fputs("Syntax:\r\n"
	    "joindomain [compaccountou\\]computer domain[\\dc] [admaccount [admpassword]]\r\n",
	    stderr);
      return 1;
    }

  wczAccountOU = wcstok(lpwArgv[1], L"\\");
  wczComputerName = wcstok(NULL, L"");
  if (wczComputerName == NULL)
    {
      wczComputerName = wczAccountOU;
      wczAccountOU = NULL;
    }

  dwErrRet = NetJoinDomain(wczComputerName, lpwArgv[2], wczAccountOU,
			   (iArgc > 3) ? lpwArgv[3] : NULL,
			   (iArgc > 4) ? lpwArgv[4] : NULL,
			   NETSETUP_JOIN_DOMAIN |
			   NETSETUP_ACCT_CREATE |
			   NETSETUP_DOMAIN_JOIN_IF_JOINED);

  if (dwErrRet == NERR_Success)
    {
      oem_printf
	(stdout,
	 "The computer '%1!ws!' was successfully joined to '%2!ws!'.%%n",
	 wczComputerName, lpwArgv[2]);
      return 0;
    }
  else
    {
      LPSTR errmsg = win_errmsgA(dwErrRet);
      oem_printf(stderr,
		 "Error joining computer '%1!ws!' to '%2!ws!': %3%%n",
		 wczComputerName, lpwArgv[2], errmsg);
    }

  return 0;
}

__declspec(noreturn)
void
wmainCRTStartup()
{
  int argc;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
  if (argv == NULL)
    {  
      MessageBoxA(NULL, "This program requires Windows NT.",
		  "Incompatible Windows version",
		  MB_ICONSTOP);
      ExitProcess((UINT) -1);
    }
  else
    exit(wmain(argc, argv));
}
