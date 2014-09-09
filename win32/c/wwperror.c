#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <stdio.h>

#pragma comment(lib, "user32")

void win_perrorW(LPCWSTR __errmsg)
{
  LPSTR errmsg = NULL;
  int __win_errno = win_errno;

  FormatMessageA(FORMAT_MESSAGE_MAX_WIDTH_MASK |
		 FORMAT_MESSAGE_FROM_SYSTEM |
		 FORMAT_MESSAGE_ALLOCATE_BUFFER,
		 NULL,
		 __win_errno,
		 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		 (LPSTR) &errmsg,
		 0,
		 NULL);

  if (__errmsg ? !!*__errmsg : FALSE)
    if (errmsg != NULL)
      oem_printf(stderr, "%1!ws!: %2%%n", __errmsg, errmsg);
    else
      oem_printf(stderr, "%1!ws!: Win32 error %2!u!%%n",
		 __errmsg, (DWORD) __win_errno);
  else
    if (errmsg != NULL)
      oem_printf(stderr, "%1%%n", errmsg);
    else
      fprintf(stderr, "Win32 error %u\n", (DWORD) __win_errno);

  if (errmsg != NULL)
    LocalFree(errmsg);
}
