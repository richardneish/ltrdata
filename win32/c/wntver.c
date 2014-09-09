#define WIN32_LEAN_AND_MEAN
#include <stdio.h>

#include <windows.h>
#include <winbase.h>
#include <winuser.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ntdllp.lib")

#pragma comment(linker, "/subsystem:windows")
#pragma comment(linker, "/entry:main")

int main()
{
  OSVERSIONINFO oi;
  char *osname;
  char buf[260];

  oi.dwOSVersionInfoSize = sizeof oi;
  GetVersionEx(&oi);

   switch (oi.dwPlatformId)
     {
     case VER_PLATFORM_WIN32s:
       osname = "Win32s";
       break;
     case VER_PLATFORM_WIN32_WINDOWS:
       if (oi.dwMinorVersion < 10)
	 osname = "Windows 95";
       else
	 osname = "Windows 98";
       break;
     case VER_PLATFORM_WIN32_NT:
       osname = "Windows NT";
       break;
     default:
       osname = "Unknown OS";
     }

   _snprintf(buf, sizeof buf,
	   "%s version %u.%02u\r\nBuild %u %s",
	   osname,
	   oi.dwMajorVersion,
	   oi.dwMinorVersion,
	   oi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ?
	   oi.dwBuildNumber & 0x0000FFFF : oi.dwBuildNumber,
	   oi.szCSDVersion);
   buf[sizeof(buf)-1] = 0;

   MessageBox(NULL, buf, "Win32 version", MB_ICONINFORMATION |
	      MB_SETFOREGROUND);

   ExitProcess(0);
}
