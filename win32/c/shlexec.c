#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

#ifdef __BORLANDC__
#  include <minwcrtlib.h>
#  include <dos.h>
#  define __argc _argc
#  define __argv _argv
#endif

#pragma comment(lib, "user32")
#pragma comment(lib, "shell32")

#if defined(_DLL) && !defined(_WIN64)
#include <minwcrtlib.h>
#pragma comment(lib, "minwcrt")
#else
#include <stdlib.h>
#endif

int WINAPI WinMain(HINSTANCE hCurInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  int nShowCmd = SW_SHOWNORMAL;
  HINSTANCE iRetVal;

  if (__argv[1] == NULL)
    {
      MessageBox(NULL,
		 "Syntax: ShlExec filename parameters operation windowstyle",
		 "ShlExec",
		 MB_ICONEXCLAMATION);
      return -1;
    }

  if (__argc > 4)
    {
      if (stricmp(__argv[4], "/HIDE") == 0)
      	nShowCmd = SW_HIDE;
      else if (stricmp(__argv[4], "/MAXIMIZE") == 0)
      	nShowCmd = SW_MAXIMIZE;
      else if (stricmp(__argv[4], "/MINIMIZE") == 0)
      	nShowCmd = SW_MINIMIZE;
      else if (stricmp(__argv[4], "/RESTORE") == 0)
      	nShowCmd = SW_RESTORE;
      else if (stricmp(__argv[4], "/SHOW") == 0)
      	nShowCmd = SW_SHOW;
      else if (stricmp(__argv[4], "/SHOWDEFAULT") == 0)
      	nShowCmd = SW_SHOWDEFAULT;
      else if (stricmp(__argv[4], "/SHOWMAXIMIZED") == 0)
      	nShowCmd = SW_SHOWMAXIMIZED;
      else if (stricmp(__argv[4], "/SHOWMINIMIZED") == 0)
      	nShowCmd = SW_SHOWMINIMIZED;
      else if (stricmp(__argv[4], "/SHOWMINNOACTIVE") == 0)
      	nShowCmd = SW_SHOWMINNOACTIVE;
      else if (stricmp(__argv[4], "/SHOWNA") == 0)
      	nShowCmd = SW_SHOWNA;
      else if (stricmp(__argv[4], "/SHOWNOACTIVATE") == 0)
      	nShowCmd = SW_SHOWNOACTIVATE;
      else if (stricmp(__argv[4], "/SHOWNORMAL") == 0)
      	nShowCmd = SW_SHOWNORMAL;
      else
	{
	  MessageBox(NULL, "Unknown window style.", "ShlExec",
		     MB_ICONEXCLAMATION);
	  return -1;
	}
    }
   
  iRetVal = ShellExecute(NULL, __argc > 3 ? __argv[3] : NULL, __argv[1],
			 __argc > 2 ? __argv[2] : NULL, NULL, nShowCmd);

  if (iRetVal > (HINSTANCE)(INT_PTR)32)
    return 0;

  if (iRetVal == 0)
    return -1;

  return (int)(INT_PTR)iRetVal;
}
