#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#include <winstrct.hpp>

WMsgOEM::WMsgOEM(WMem &mem) : WHeapMem((LPSTR) NULL)
{
  int i = WideCharToMultiByte(CP_OEMCP, 0, (LPWSTR) mem,
			      (int) mem.GetSize() >> 1, NULL, 0, NULL, NULL);

  ptr = halloc(i);
  if (WideCharToMultiByte(CP_OEMCP, 0, (LPWSTR) mem, (int) mem.GetSize() >> 1,
			  (LPSTR)ptr, i, NULL, NULL) == i)
    return;

  Free();
}
