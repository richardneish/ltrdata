#include <wtime.h>
#include <stdio.h>

EXTERN_C VOID CDECL PrintShortDate(SYSTEMTIME *st)
{
  char cBuf[160];
  if (GetDateFormat(LOCALE_USER_DEFAULT, 0, st, NULL, cBuf, sizeof cBuf))
    {
      CharToOem(cBuf, cBuf);
      fputs(cBuf, stdout);
    }
}

EXTERN_C VOID CDECL PrintLongDate(SYSTEMTIME *st)
{
  char cBuf[160];
  if (GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, st, NULL, cBuf,
		    sizeof cBuf))
    {
      CharToOem(cBuf, cBuf);
      fputs(cBuf, stdout);
    }
}

EXTERN_C VOID CDECL PrintTime(SYSTEMTIME *st)
{
  char cBuf[160];
  if (GetTimeFormat(LOCALE_USER_DEFAULT, 0, st, NULL, cBuf, sizeof cBuf))
    {
      CharToOem(cBuf, cBuf);
      fputs(cBuf, stdout);
    }
}

EXTERN_C VOID CDECL PrintShortDateAndTime(SYSTEMTIME *st)
{
  char cBuf[160];
  if (GetDateFormat(LOCALE_USER_DEFAULT, 0, st, NULL, cBuf, sizeof cBuf))
    {
      CharToOem(cBuf, cBuf);
      printf("%s ", cBuf);
    }

  if (GetTimeFormat(LOCALE_USER_DEFAULT, 0, st, NULL, cBuf, sizeof cBuf))
    {
      CharToOem(cBuf, cBuf);
      fputs(cBuf, stdout);
    }
}

EXTERN_C VOID CDECL PrintLongDateAndTime(SYSTEMTIME *st)
{
  char cBuf[160];
  if (GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, st, NULL, cBuf,
		    sizeof cBuf))
    {
      CharToOem(cBuf, cBuf);
      printf("%s ", cBuf);
    }

  if (GetTimeFormat(LOCALE_USER_DEFAULT, 0, st, NULL, cBuf, sizeof cBuf))
    {
      CharToOem(cBuf, cBuf);
      fputs(cBuf, stdout);
    }
}
