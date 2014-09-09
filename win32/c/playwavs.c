#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

#ifdef __BORLANDC__
#  include <dos.h>
#  define __argc _argc
#  define __argv _argv
#else
#  if defined(_DLL) && !defined(_WIN64)
#    include <minwcrtlib.h>
#    pragma comment(lib, "minwcrt.lib")
#  else
#    include <stdlib.h>
#  endif
#  pragma comment(lib, "winmm.lib")
#endif

int WINAPI
WinMain(HINSTANCE hCurInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
   while ((++__argv)[0] != NULL)
      PlaySound(__argv[0], NULL, SND_NODEFAULT | SND_SYNC);

   return 0;
}
