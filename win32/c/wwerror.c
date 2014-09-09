#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

LPWSTR win_errmsgW(DWORD dwErrNo)
{
  LPWSTR errmsg = NULL;

  if (FormatMessageW(FORMAT_MESSAGE_MAX_WIDTH_MASK |
		     FORMAT_MESSAGE_FROM_SYSTEM |
		     FORMAT_MESSAGE_ALLOCATE_BUFFER,
		     NULL,
		     dwErrNo,
		     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		     (LPWSTR) &errmsg,
		     0,
		     NULL))
    return errmsg;
  else
    return NULL;
}
