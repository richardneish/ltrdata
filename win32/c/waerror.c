#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

LPSTR win_errmsgA(DWORD dwErrNo)
{
  LPSTR errmsg = NULL;

  if (FormatMessageA(FORMAT_MESSAGE_MAX_WIDTH_MASK |
		     FORMAT_MESSAGE_FROM_SYSTEM |
		     FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, dwErrNo,
		     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		     (LPSTR)&errmsg, 0, NULL))
    return errmsg;
  else if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			  FORMAT_MESSAGE_FROM_STRING |
			  FORMAT_MESSAGE_ARGUMENT_ARRAY, "Error %1!i!", 0, 0,
			  (LPSTR)&szMsgFmt, 0, &dwErrNo))
    return errmsg;
  else
    return NULL;
}
