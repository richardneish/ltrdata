#include <wtime.h>
#include <stdio.h>

EXTERN_C VOID CDECL PrintAbsoluteTime(QWORD qwTime)
{
  double dblMilliseconds;
  DWORD dwSeconds;
  DWORD dwMinutes;
  DWORD dwHours;
  DWORD dwDays;
  DWORD dwWeeks;

  dwWeeks = (DWORD)(qwTime / 6048000000000); qwTime %= 6048000000000;
  dwDays = (DWORD)(qwTime / 864000000000); qwTime %= 864000000000;
  dwHours = (DWORD)(qwTime / 36000000000); qwTime %= 36000000000;
  dwMinutes = (DWORD)(qwTime / 600000000); qwTime %= 600000000;
  dwSeconds = (DWORD)(qwTime / 10000000); qwTime %= 10000000;
  dblMilliseconds = (double)qwTime / 10000;

  if (dwWeeks)
    printf("%u weeks, ", dwWeeks);

  if (dwDays)
    printf("%u days, ", dwDays);

  printf("%u:%.2u:%.2u %f",
	 dwHours, dwMinutes, dwSeconds, dblMilliseconds);

}
