#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <stdlib.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

int main(int argc, char **argv)
{
  if (argc < 2)
    {
      puts("Syntax 1 (any process):\r\n"
	   "KILL pid\r\n"
	   "Forcefully terminates the process with the given id.\r\n"
	   "\n"
	   "Syntax 2 (processes with windows):\r\n"
	   "KILL [/CLOSE|/DESTROY|/QUIT] [/TERMINATE [/TIMEOUT:nn]] [classname] windowtitle\r\n"
	   "\n"
	   "Syntax 3 (console processes):\r\n"
	   "KILL [/CLOSE|/BREAK|/C] [/TERMINATE [/TIMEOUT:nn]] groupid\r\n"
	   "Sends CTRL_CLOSE_EVENT, CTRL_BREAK_EVENT or CTRL_C_EVENT to\r\n"
	   "a console process group.\r\n"
	   "\n"
	   "If /TERMINATE is given, the first process in the process group\r\n"
	   "given 10 seconds to exit after the message is sent. If the\r\n"
	   "process has not terminated within that time, the process is\r\n"
	   "forcefully terminated. Use the /TIMEOUT switch to give the\r\n"
	   "process longer or shorter time (in seconds) to exit.\r\n"
	   "\n"
	   "Syntax 4 (processes with windows):\r\n"
	   "KILL /INFO [classname] windowtitle\r\n"
	   "Displays information about the window and the process and\r\n"
	   "thread it belongs to. This syntax does not attempt to\r\n"
	   "terminate the process.");

      return 0;
    }

  bool bByWindow = false;
  UINT Msg = WM_CLOSE;
  DWORD dwCtrlType = (DWORD)-1;
  DWORD dwTimeout = 10000;
  bool bForceTerminate = false;
  bool bInfo = false;

  while (argc > 1)
    {
      Sleep(0);

      if (strnicmp(argv[1], "/TIMEOUT:", 9) == 0)
	{
	  bByWindow = true;
	  strtok(argv[1], ":");
	  char *pOpt = strtok(NULL, "");
	  dwTimeout = strtoul(pOpt, NULL, 0) * 1000;
	}
      else if (stricmp(argv[1], "/CLOSE") == 0)
	{
	  bByWindow = true;
	  Msg = WM_CLOSE;
	  dwCtrlType = CTRL_CLOSE_EVENT;
	}
      else if (stricmp(argv[1], "/DESTROY") == 0)
	{
	  bByWindow = true;
	  Msg = WM_DESTROY;
	}
      else if (stricmp(argv[1], "/QUIT") == 0)
	{
	  bByWindow = true;
	  Msg = WM_QUIT;
	}
      else if (stricmp(argv[1], "/TERMINATE") == 0)
	{
	  bByWindow = true;
	  bForceTerminate = true;
	}
      else if (stricmp(argv[1], "/INFO") == 0)
	{
	  bByWindow = true;
	  bInfo = true;
	}
      else if (stricmp(argv[1], "/C") == 0)
	{
	  dwCtrlType = CTRL_C_EVENT;
	}
      else if (stricmp(argv[1], "/BREAK") == 0)
	{
	  dwCtrlType = CTRL_BREAK_EVENT;
	}
      else if (argv[1][0] == '/')
	fprintf(stderr, "Unknown switch: %s\n", argv[1]);
      else break;

      ++argv;
      --argc;
    }

  DWORD pid = strtoul(argv[argc-1], NULL, 0);

  if (pid == 0)
    {
      char *szClassName = NULL;
      if (argc > 2)
	if (argv[argc-2][0] != '/')
	  szClassName = argv[argc-2];

      char *szTitle = argv[argc-1];
      if (strcmp(szTitle, "*") == 0)
	szTitle = NULL;

      HWND hWnd = FindWindow(szClassName, szTitle);
      if (hWnd == NULL)
	{
	  fprintf(stderr, "Can't find window with title %s.\n", argv[argc-1]);
	  return 1;
	}

      DWORD tid = GetWindowThreadProcessId(hWnd, &pid);

      if (bInfo)
	{
	  printf("Process id: %u\nThread id: %u\n", pid, tid);

	  char cName[MAX_PATH+1];
	  if (GetClassName(hWnd, cName, sizeof(cName)))
            printf("Window class name: %s\n", cName);

	  DWORD dwWinVer = GetProcessVersion(pid);
	  if (dwWinVer)
            printf("Expected Windows version: %u.%u\n", dwWinVer >> 16, 
		   dwWinVer & 0xFFFF);

	  return 0;
	}

      if (Msg == WM_QUIT)
	{
	  if (!PostThreadMessage(tid, WM_QUIT, (WPARAM)-1, (LPARAM)0))
            win_perror("Can't post window thread WM_QUIT message");
	}
      else
      {
	if (!PostMessage(hWnd, Msg, 0, 0))
	  win_perror("Can't post window message");
      }

      if (!bForceTerminate)
	return 0;
    }
  else
    bByWindow = false;

  if ((dwCtrlType != (DWORD)-1) & !bByWindow)
    {
      if (!GenerateConsoleCtrlEvent(dwCtrlType, pid))
	win_perror("Error sending control event");
    }
  else
    dwTimeout = 0;


  HANDLE ph = OpenProcess(PROCESS_TERMINATE|SYNCHRONIZE, false, pid);
  if (!ph)
    {
      win_perror("Open process failed");
      return 1;
    }

  switch (WaitForSingleObject(ph, dwTimeout))
    {
    case WAIT_OBJECT_0:
      printf("Process %u terminated gracefully.", pid);
      return 0;
    case WAIT_TIMEOUT:
      break;
    default:
      win_perror("Can't wait on this process");
    }

  if (!TerminateProcess(ph, 0))
    {
      win_perror("Terminate process failed");
      return 2;
    }

  printf("Process %i forcefully terminated.\n", pid);
  return 0;
}

