#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define SLEEP_INTERVAL_MS 200

#pragma comment(lib, "user32.lib")

#if defined(_DLL) && !defined(_WIN64)
#include <minwcrtlib.h>
#pragma comment(lib, "minwcrt.lib")
#else
#include <stdlib.h>
#endif

int
WINAPI
WinMain(HINSTANCE hCurInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
	int nCmdShow)
{
  LPCSTR szClassName;
  LPCSTR szWindowTitle;
  UINT_PTR uiTimer = SetTimer(NULL, 0, SLEEP_INTERVAL_MS, NULL);

  if (__argc == 2)
    {
      szClassName = NULL;
      szWindowTitle = __argv[1];
    }
  else if (__argc == 3)
    {
      szClassName = __argv[1];
      szWindowTitle = __argv[2];
    }
  else
    {
      MessageBox(NULL, "Send as parameter the name of the window you want "
		 "automatically closed.", "DenyWin", MB_ICONINFORMATION);
      return 0;
    }

  for(;;)
    {
      MSG msg;

      HWND hWnd = FindWindow(szClassName, szWindowTitle);
      if (hWnd != NULL)
	PostMessage(hWnd, WM_CLOSE, 0, 0);

      if (uiTimer != 0)
	GetMessage(&msg, NULL, WM_TIMER, WM_TIMER);
      else
	Sleep(SLEEP_INTERVAL_MS);
    }
}
