#define WIN32_LEAN_AND_MEAN
#include <winsock.h>
#include <winstrct.h>

#include <werrlog.h>
#include <wtime.h>

EXTERN_C void
LogSockErr(LPCSTR szMsgStr, ...)
{
  va_list args;
  va_start(args, szMsgStr);

  DWORD dwErrCode = WSAGetLastError();

  WSockErrMsg errmsg;
  CharToOem(errmsg, errmsg);

  WSystemTime st;
  st.GetUTC();
  st.PrintShortDateAndTime(stderr);
  fputs(": ", stderr);
  vfprintf(stderr, szMsgStr, args);
  fprintf(stderr, ": Winsock error %i: %s", dwErrCode, errmsg);
  fflush(stderr);
}
