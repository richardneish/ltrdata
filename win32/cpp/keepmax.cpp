#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma comment(lib, "user32.lib")

int
main(int argc, char **argv)
{
  HWND hwndConsole = GetConsoleWindow();

  FreeConsole();

  while (IsWindow(hwndConsole))
    {
      SetActiveWindow(hwndConsole);
      SetFocus(hwndConsole);
      SetForegroundWindow(hwndConsole);
      ShowWindow(hwndConsole, SW_MAXIMIZE);

      Sleep(500);
    }

  return 0;
}
