#define WIN32_LEAN_AND_MEAN
#include <stdio.h>

#include <winstrct.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")

int WINAPI
WinMain(HINSTANCE hCurInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
  OSVERSIONINFO oi;
  char *osname = NULL;
  char *msg;

  oi.dwOSVersionInfoSize = sizeof(oi);
  GetVersionEx(&oi);

   switch (oi.dwPlatformId)
     {
     case VER_PLATFORM_WIN32s:
       osname = "Win32s";
       break;
     case VER_PLATFORM_WIN32_WINDOWS:
       if (oi.dwMinorVersion < 10)
	 osname = "Windows 95";
       else if (oi.dwMinorVersion > 10)
	 osname = "Windows ME";
       else
	 osname = "Windows 98";
       break;
     case VER_PLATFORM_WIN32_NT:
       if (oi.dwMajorVersion < 5)
	 osname = "Windows NT";
       else if (oi.dwMajorVersion == 5)
	 {
	   if (oi.dwMinorVersion < 1)
	     osname = "Windows 2000";
	   else if (oi.dwMinorVersion == 1)
	     osname = "Windows XP";
	   else if (oi.dwMinorVersion == 2)
	     osname = "Windows Server 2003";
	 }
       else if (oi.dwMajorVersion == 6)
	 {
	   if (oi.dwMinorVersion < 1)
	     osname = "Windows Vista or Server 2008";
	   else if (oi.dwMinorVersion == 1)
	     osname = "Windows 7 or Server 2008 R2";
	   else if (oi.dwMinorVersion == 2)
	     osname = "Windows 8 or Server 2012";
	 }

       if (osname == NULL)
	 osname = "Windows";
       break;
     default:
       osname = "Unknown OS";
     }

   msg = mprintf("%1 version %2!u!.%3!u!%nBuild %4!u! %5!s!",
		 osname,
		 oi.dwMajorVersion,
		 oi.dwMinorVersion,
		 oi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ?
		 oi.dwBuildNumber & 0x0000FFFF : oi.dwBuildNumber,
		 oi.szCSDVersion);
   if (msg == NULL)
     msg = win_error;

   MessageBox(NULL, msg, "Win32 version", MB_ICONINFORMATION);
   LocalFree(msg);

   return 0;
}

int
WinMainCRTStartup()
{
  ExitProcess(WinMain(NULL, NULL, NULL, 0));
}
