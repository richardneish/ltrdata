#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wthread.h>
#include <process.h>

BOOL SafeBeginThread(UINT (WINAPI *codeaddress)(LPVOID), UINT uiStackSize,
		     LPVOID lpArglist)
{
  UINT uiTid;
  HANDLE hThrd = (HANDLE)_beginthreadex(NULL, uiStackSize, codeaddress,
					lpArglist, 0, &uiTid);
  if (hThrd == NULL)
    return FALSE;
  else
    {
      CloseHandle(hThrd);
      return TRUE;
    }
}
