#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ntdll.h>
#include <winstrct.h>

LPSTR pdh_errmsgA(DWORD dwErrorCode)
{
  LPSTR errmsg = NULL;

  if (FormatMessageA(FORMAT_MESSAGE_MAX_WIDTH_MASK |
		     FORMAT_MESSAGE_FROM_SYSTEM |
		     FORMAT_MESSAGE_FROM_HMODULE |
		     FORMAT_MESSAGE_ALLOCATE_BUFFER,
		     GetModuleHandleA(PDH_MODULE), dwErrorCode,
		     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		     (LPSTR)&errmsg, 0, NULL))
    return errmsg;
  else
    return NULL;
}
