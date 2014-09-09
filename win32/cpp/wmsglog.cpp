#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <werrlog.h>
#include <wtime.h>

EXTERN_C void
LogMsg(LPCSTR szMsgStr, ...)
{
  va_list args;
  va_start(args, szMsgStr);

  WSystemTime st;
  st.GetUTC();
  st.PrintShortDateAndTime();
  fputs(": ", stdout);
  vprintf(szMsgStr, args);
  fflush(stdout);
}

EXTERN_C void
LogErr(LPCSTR szMsgStr, ...)
{
  va_list args;
  va_start(args, szMsgStr);

  WSystemTime st;
  st.GetUTC();
  st.PrintShortDateAndTime(stderr);
  fputs(": ", stderr);
  vfprintf(stderr, szMsgStr, args);
  fflush(stderr);
}
