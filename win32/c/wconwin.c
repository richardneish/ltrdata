#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wconsole.h>

#if _WIN32_WINNT < 0x0500
static BOOL CALLBACK GetConsoleWindowProc(HWND hWnd, LPARAM lParam)
{
   const char cConsoleClassName[] = "ConsoleWindowClass";
   DWORD dwPid = (DWORD)-1;
   char cBuf[sizeof(cConsoleClassName)] = "";

   GetWindowThreadProcessId(hWnd, &dwPid);
   if (dwPid != GetCurrentProcessId())
      return TRUE;

   if (GetClassName(hWnd, cBuf, sizeof cBuf) != sizeof(cConsoleClassName)-1)
      return TRUE;

   if (strcmp(cConsoleClassName, cBuf) != 0)
      return TRUE;

   *(HWND*)lParam = hWnd;
   return FALSE;
}

HWND WINAPI GetConsoleWindow()
{
   HWND hWnd = NULL;
   EnumWindows((WNDENUMPROC)GetConsoleWindowProc, (LPARAM)&hWnd);
   return hWnd;
}
#endif

