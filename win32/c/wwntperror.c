#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ntdll.h>
#include <winstrct.h>
#include <stdio.h>

#pragma comment(lib, "user32")

void nt_perrorW(NTSTATUS stat, LPCWSTR __errmsg)
{
  LPSTR errmsg = NULL;

  FormatMessageA(FORMAT_MESSAGE_MAX_WIDTH_MASK |
		 FORMAT_MESSAGE_FROM_SYSTEM |
		 FORMAT_MESSAGE_FROM_HMODULE |
		 FORMAT_MESSAGE_ALLOCATE_BUFFER,
		 GetModuleHandleA("NTDLL"),
		 stat,
		 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		 (LPSTR)&errmsg,
		 0,
		 NULL);

  if (__errmsg ? !!*__errmsg : FALSE)
    if (errmsg != NULL)
      oem_printf(stderr, "%1!ws!: %2%%n", __errmsg, errmsg);
    else
      oem_printf(stderr, "%1!ws!: Error status %2!#x!%%n",
		 __errmsg, (DWORD) stat);
  else
    if (errmsg != NULL)
      oem_printf(stderr, "%1%%n", errmsg);
    else
      fprintf(stderr, "Error status %#x\n", (DWORD) stat);

  if (errmsg != NULL)
    LocalFree(errmsg);
}
