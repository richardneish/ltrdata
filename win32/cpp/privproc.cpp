#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <shellapi.h>
#include <ntdll.h>
#include <winstrct.h>
#include <stdlib.h>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")
#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "crtdll.lib")
#pragma comment(lib, "BufferoverflowU.lib")
#endif

int
WINAPI
wWinMain(HINSTANCE hCurInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine,
	 int nCmdShow)
{
  LPWSTR lpCmdOptions = wcstok(lpCmdLine, L" ");
  lpCmdLine = wcstok(NULL, L"");
  while (*lpCmdLine == L' ')
    ++lpCmdLine;

  if ((*lpCmdOptions == 0) | (*lpCmdLine == 0))
    {
      MessageBox(NULL,
		 L"Syntax:\r\n"
		 L"privproc privilege1[,privilege2 ...] commandline [...]\r\n"
		 L"Starts a new process with given privileges enabled.\r\n"
		 L"\n"
		 L"privproc privilege1[,privilege2 ...] pid\r\n"
		 L"Enables given privileges for a running process.\r\n"
		 L"\n"
		 L"privproc /u privilege1[,privilege2 ...] pid\r\n"
		 L"Disables given privileges for a running process.\r\n"
		 L"\n"
		 L"privproc /d pid\r\n"
		 L"Disables all privileges for a running process.",
		 L"privproc",
		 MB_ICONINFORMATION);
      return 2;
    }

  bool bEnableMode = true;
  if (wcsicmp(lpCmdOptions, L"/u") == 0)
    bEnableMode = false;
  
  ULONG pid = wcstoul(lpCmdLine, NULL, 0);
  PROCESS_INFORMATION process_information = { 0 };
  if (pid != 0)
    {
      process_information.hProcess =
	OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);

      if (process_information.hProcess == NULL)
	{
	  MessageBoxA(NULL, win_errmsgA(win_errno), "Cannot open process",
		      MB_ICONEXCLAMATION);
	  return 1;
	}
    }
  else
    {
      STARTUPINFO startupinfo = { 0 };
      startupinfo.cb = sizeof startupinfo;

      if (!CreateProcess(NULL, lpCmdLine, NULL, NULL, FALSE, CREATE_SUSPENDED,
			 NULL, NULL, &startupinfo, &process_information))
	{
	  MessageBox(NULL,
		     mprintf(L"Cannot execute command line:%n%1>%2<",
			     win_errmsg(win_errno), lpCmdLine),
		     L"Cannot create process", MB_ICONEXCLAMATION);
	  return 1;
	}
    }

  HANDLE hToken;
  PTOKEN_PRIVILEGES token_privileges;
  if (!OpenProcessToken(process_information.hProcess, TOKEN_ADJUST_PRIVILEGES,
			&hToken))
    {
      MessageBox(NULL, win_errmsg(win_errno), L"Cannot open process token",
		 MB_ICONEXCLAMATION);
      if (process_information.hThread != NULL)
	TerminateProcess(process_information.hProcess, (UINT) -1);
      return 1;
    }

  BOOL bDisableAll;
  if (wcsicmp(lpCmdOptions, L"/d") == 0)
    {
      bDisableAll = TRUE;
      token_privileges = NULL;
    }
  else
    {
      bDisableAll = FALSE;
      int iPrivCount = wcscnt(lpCmdOptions, L',') + 1;

      token_privileges = (PTOKEN_PRIVILEGES)
	halloc(sizeof(TOKEN_PRIVILEGES) + iPrivCount);
      if (token_privileges == NULL)
	{
	  MessageBox(NULL, win_errmsg(win_errno), L"Out of memory",
		     MB_ICONEXCLAMATION);
	  if (process_information.hThread != NULL)
	    TerminateProcess(process_information.hProcess, (UINT) -1);
	  return 1;
	}

      token_privileges->PrivilegeCount = iPrivCount;
      int iPrivCounter = 0;
      for (LPWSTR lpPrivName = wcstok(lpCmdOptions, L",");
	   lpPrivName != NULL;
	   lpPrivName = wcstok(NULL, L","))
	{
	  if (!LookupPrivilegeValue(NULL, lpPrivName,
				    &token_privileges->
				    Privileges[iPrivCounter].Luid))
	    {
	      LPWSTR errmsg = win_errmsg(win_errno);
	      LPWSTR msg =
		mprintf(L"Privilege lookup for '%1' failed:%n%2",
			lpPrivName, errmsg);
	      MessageBox(NULL, msg, L"Privilege lookup failed",
			 MB_ICONEXCLAMATION);
	      if (process_information.hThread != NULL)
		TerminateProcess(process_information.hProcess, (UINT) -1);
	      return 1;
	    }

	  if (bEnableMode)
	    token_privileges->Privileges[iPrivCounter].Attributes =
	      SE_PRIVILEGE_ENABLED;
	  else
	    token_privileges->Privileges[iPrivCounter].Attributes = 0;

	  iPrivCounter++;
	}
    }

  DWORD dwErrCode;
  if (!AdjustTokenPrivileges(hToken, bDisableAll, token_privileges, 0, NULL,
			     NULL))
    {
      MessageBox(NULL, win_errmsg(win_errno), L"Privilege adjustment failed",
		 MB_ICONEXCLAMATION);
      if (process_information.hThread != NULL)
	TerminateProcess(process_information.hProcess, (UINT) -1);
      return 1;
    }
  else
    dwErrCode = GetLastError();

  if (process_information.hThread != NULL)
    ResumeThread(process_information.hThread);

  if (dwErrCode != ERROR_SUCCESS)
    MessageBox(NULL, win_errmsg(dwErrCode), L"Privilege adjustment incomplete",
	       MB_ICONEXCLAMATION);

  return 0;
}

extern "C" int
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
