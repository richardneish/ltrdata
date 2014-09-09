#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "user32")

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

int main(int argc, char **argv)
{
  int nCmdShow = SW_SHOW;
  HWND hWnd;
  DWORD pid, tid, dwProcessWinVer;
  char cName[MAX_PATH+1];

  if (!argv[1])
    {
      puts("Syntax:\r\n"
	   "SHOWWIN [/SHOW|/HIDE|/MAXIMIZE|/MINIMIZE|/RESTORE] [classname] windowtitle\r\n");
      
      return 0;
    }

  while (argc > 1)
    {
      Sleep(0);

      if (stricmp(argv[1], "/HIDE") == 0)
        nCmdShow = SW_HIDE;
      else if (stricmp(argv[1], "/MAXIMIZE") == 0)
        nCmdShow = SW_MAXIMIZE;
      else if (stricmp(argv[1], "/MINIMIZE") == 0)
        nCmdShow = SW_MINIMIZE;
      else if (stricmp(argv[1], "/RESTORE") == 0)
        nCmdShow = SW_RESTORE;
      else if (stricmp(argv[1], "/SHOW") == 0)
        nCmdShow = SW_SHOW;
      else if (stricmp(argv[1], "/SHOWDEFAULT") == 0)
        nCmdShow = SW_SHOWDEFAULT;
      else if (stricmp(argv[1], "/SHOWMAXIMIZED") == 0)
        nCmdShow = SW_SHOWMAXIMIZED;
      else if (stricmp(argv[1], "/SHOWMINIMIZED") == 0)
        nCmdShow = SW_SHOWMINIMIZED;
      else if (stricmp(argv[1], "/SHOWMINNOACTIVE") == 0)
        nCmdShow = SW_SHOWMINNOACTIVE;
      else if (stricmp(argv[1], "/SHOWNA") == 0)
        nCmdShow = SW_SHOWNA;
      else if (stricmp(argv[1], "/SHOWNOACTIVATE") == 0)
        nCmdShow = SW_SHOWNOACTIVATE;
      else if (stricmp(argv[1], "/SHOWNORMAL") == 0)
        nCmdShow = SW_SHOWNORMAL;
      else break;

      ++argv;
      --argc;
    }

  hWnd = FindWindow((argc > 2) ? argv[1] : NULL, argv[argc-1]);
  if (hWnd == NULL)
    {
      fprintf(stderr, "Can't find window with title %s.\n", argv[argc-1]);
      return 1;
    }

  tid = GetWindowThreadProcessId(hWnd, &pid);

  printf("Process id: %u\nThread id: %u\n", pid, tid);

  if (GetClassName(hWnd, cName, sizeof(cName)))
    printf("Window class name: %s\n", cName);

  dwProcessWinVer = GetProcessVersion(pid);
  if (dwProcessWinVer)
    printf("Expected Windows version: %u.%u\n", (dwProcessWinVer >> 16), 
	   (dwProcessWinVer & 0xFFFF));

  ShowWindow(hWnd, nCmdShow);
}

