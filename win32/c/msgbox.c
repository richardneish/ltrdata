#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef __BORLANDC__
#  include <dos.h>
#  define __argc _argc
#  define __argv _argv
#elif defined(_DLL) && !defined(_WIN64)
#  include <minwcrtlib.h>
#  pragma comment(lib, "user32")
#  pragma comment(lib, "minwcrt")
#else
#  include <stdlib.h>
#  pragma comment(lib, "user32")
#endif

int WINAPI WinMain(HINSTANCE hCurInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  return MessageBox(NULL, __argc > 1 ? __argv[1] : "",
		    __argc > 3 ? __argv[3] : NULL,
		    __argc > 2 ? strtoul(__argv[2], NULL, 0) : 0);
}

