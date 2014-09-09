#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>

#pragma comment(linker, "/defaultlib:crtdll")
#pragma comment(linker, "/subsystem:windows")

LPSTR WINAPI StrRev(LPSTR str)
{
  return _strrev(str);
}
