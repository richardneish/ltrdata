#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <process.h>
#include <stdlib.h>
#include <stdio.h>

UINT WINAPI
Thrd(HANDLE h)
{
  ((char*)NULL)[0] = 0;
  return 0;
}

int WINAPI
WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int iShowWin)
{
  static char buf[80];
  DWORD dw;
  HANDLE hThread;

  if (MessageBox(NULL, "Proceed?", "Warning", MB_YESNO |
		 MB_ICONEXCLAMATION | MB_TASKMODAL | MB_DEFBUTTON2) != IDYES)
    return 1;

  hThread = (HANDLE) _beginthreadex(NULL, 4096, Thrd, NULL, 0, &dw);

  Sleep(INFINITE);
  return 0;
}
