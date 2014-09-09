#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ntdll.h>
#include <winstrct.h>

LPWSTR nt_errmsgW(NTSTATUS stat)
{
  LPWSTR errmsg = NULL;

  if (FormatMessageW(FORMAT_MESSAGE_MAX_WIDTH_MASK |
		     FORMAT_MESSAGE_FROM_SYSTEM |
		     FORMAT_MESSAGE_FROM_HMODULE |
		     FORMAT_MESSAGE_ALLOCATE_BUFFER,
		     GetModuleHandleW(L"NTDLL"), stat,
		     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		     (LPWSTR)&errmsg, 0, NULL))
    return errmsg;
  else
    return NULL;
}
