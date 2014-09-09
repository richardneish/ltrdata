#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

#include <werrlog.h>
#include <wtime.h>

EXTERN_C void
LogWinErr(LPCSTR szMsgStr, ...)
{
  va_list args;
  va_start(args, szMsgStr);

  DWORD dwErrCode = GetLastError();

  WErrMsg errmsg;
  CharToOem(errmsg, errmsg);

  WSystemTime st;
  st.GetUTC();
  st.PrintShortDateAndTime(stderr);
  fputs(": ", stderr);
  vfprintf(stderr, szMsgStr, args);
  fprintf(stderr, ": Win32 error %i: %s", dwErrCode, errmsg);
  fflush(stderr);
}
