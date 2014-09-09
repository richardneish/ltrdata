#define WIN32_LEAN_AND_MEAN
#include <stdlib.h>
#include <windows.h>

#pragma comment(lib, "user32.lib")
#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

int WINAPI WinMain(HINSTANCE hCurInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  SetTimer(NULL, 0, strtoul(lpCmdLine, NULL, 0) * 1000, NULL);
  WaitMessage();
  return 0;
}
