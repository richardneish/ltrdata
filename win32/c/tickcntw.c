#ifndef _DLL
#define _DLL
#endif
#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

#ifndef _WIN64
#pragma comment(lib, "llmath.lib")
#pragma comment(lib, "crtdll.lib")
#endif

#pragma comment(linker, "/subsystem:windows")
#pragma comment(linker, "/entry:startup")

int
startup()
{
  LARGE_INTEGER liFreq, liTime = { 0 };
  LONGLONG llTime, llWeeks;
  DWORD dwDays, dwHours, dwMinutes, dwSeconds;
  char cBuff[260];
  BOOL bRet = QueryPerformanceFrequency(&liFreq);
  if (bRet)
    bRet = QueryPerformanceCounter(&liTime);

  if (!bRet)
    {
      MessageBox(NULL, win_error, "Performance counter", MB_ICONSTOP);
      ExitProcess(0);
    }

  llTime = liTime.QuadPart /= (liFreq.QuadPart / 1000);
  
  llWeeks     = llTime / 604800000;           llTime %= 604800000;
  dwDays      = (DWORD) (llTime / 86400000);   llTime %= 86400000;
  dwHours     = (DWORD) (llTime / 3600000);    llTime %= 3600000;
  dwMinutes   = (DWORD) (llTime / 60000);      llTime %= 60000;
  dwSeconds   = (DWORD) (llTime / 1000);       llTime %= 1000;

  sprintf(cBuff,
	  "%.f week%s "
	  "%u day%s "
	  "%u hour%s "
	  "%u minute%s "
	  "%u second%s and "
	  "%.f millisecond%s.\n",
	  (double) llWeeks, (llWeeks==1 ? "":"s"),
	  dwDays, (dwDays==1 ? "":"s"),
	  dwHours, (dwHours==1 ? "":"s"),
	  dwMinutes, (dwMinutes==1 ? "":"s"),
	  dwSeconds, (dwSeconds==1 ? "":"s"),
	  (double) llTime, (llTime==1 ? "":"s"));

   MessageBox(NULL, cBuff, "System up time", MB_ICONINFORMATION);

   ExitProcess((UINT) liTime.QuadPart);
}
