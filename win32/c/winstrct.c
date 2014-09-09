#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

void win_perror(LPCTSTR errdsc)
{
  LPSTR errmsg = NULL;
  int __win_errno = win_errno;

  if (errdsc)
    if (*errdsc)
      _ftprintf(stderr, _T("%s: "), errdsc);

  if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM |
		     FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, __win_errno,
		     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		     (LPSTR)&errmsg, 0, NULL))
    {
      CharToOemA(errmsg, errmsg);
      fputs(errmsg, stderr);
      LocalFree(errmsg);
    }
  else
    _ftprintf(stderr, _T("Error %i\n"), __win_errno);
}

LPSTR win_errorA(void)
{
  LPSTR errmsg = NULL;
  int __win_errno = win_errno;

  if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM|
		     FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, __win_errno,
		     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		     (LPSTR)&errmsg, 0, NULL))
    return errmsg;
  else
    return NULL;
}

#ifdef COMPILE_WINSTRCT_UNICODE
LPWSTR win_errorW(void)
{
  LPWSTR errmsg = NULL;
  int __win_errno = win_errno;

  if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM|
		     FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, __win_errno,
		     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		     (LPWSTR)&errmsg, 0, NULL))
    return errmsg;
  else
    return NULL;
}
#endif
