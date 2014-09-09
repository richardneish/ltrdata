/* LightRepeater Statistics view utility
 *
 * LightRepeater v 1.01 by SM6XMK, Olof Lagerkvist
 * Send comments and suggestions to sm6xmk@ssa.se
 *
 * Latest version available at http://www.qsl.net/sm6xmk/apps/LightRepeater/
 *
 * Use this utiltiy to view the statistics in the LightRep_stat.dat statistics
 * file. If that file exists, LightRepeater Engine continously updates its
 * data members.
 *
 *  SM6XMK LightRepeater, Repeater control software
 *  Copyright (C) 2001  SM6XMK, Olof Lagerkvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#define WINVER          0x0400
#define _WIN32_WINDOWS  0x0400
#define _WIN32_WINNT    0x0400
#define WIN32_LEAN_AND_MEAN

#include <winstrct.h>
#include <wfilever.h>

#include <stdio.h>

#include "lightrep_log.h"

#pragma comment(lib, "version")
#pragma comment(lib, "advapi32")
#pragma comment(lib, "llmath")

inline void
PrintSystemTime(SYSTEMTIME & st, char *pcMsg = "  ")
{
  fputs(pcMsg, stdout);
  if (st.wDay == 0)
    puts("N/A");
  else
    {
      char cBuf[80];
      GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, NULL, cBuf,
		    sizeof(cBuf));
      CharToOem(cBuf, cBuf);
      printf("%s  ", cBuf);
      GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, cBuf, sizeof(cBuf));
      puts(cBuf);
    }
}

inline void
PrintMilliseconds(QWORD qwMsec, char *pcMsg = NULL)
{
  fputs(pcMsg ? pcMsg : "  ", stdout);

  if (!qwMsec)
    {
      puts("0");
      return;
    }

  DWORD dwDays = (DWORD) (qwMsec / 86400000);
  qwMsec %= 86400000;
  DWORD dwHours = (DWORD) (qwMsec / 3600000);
  qwMsec %= 3600000;
  DWORD dwMinutes = (DWORD) (qwMsec / 60000);
  qwMsec %= 60000;
  DWORD dwSeconds = (DWORD) (qwMsec / 1000);
  qwMsec %= 1000;
  DWORD dwMsec = (DWORD) qwMsec;

  if (dwDays)
    printf("%u day%s ", dwDays, dwDays > 1 ? "s" : "");
  if (dwHours)
    printf("%u hour%s ", dwHours, dwHours > 1 ? "s" : "");
  if (dwMinutes)
    printf("%u minute%s ", dwMinutes, dwMinutes > 1 ? "s" : "");
  if (dwSeconds)
    printf("%u second%s ", dwSeconds, dwSeconds > 1 ? "s" : "");
  if (dwMsec)
    printf("%u ms", dwMsec);

  puts("");
}

int main()
{
  LRStatistics *plrs = new LRStatistics;
  char cBuf[MAX_PATH] = "";
  GetModuleFileName(GetModuleHandle(NULL), cBuf, sizeof cBuf);

  WFileVerInfo fvi(cBuf);
  char *pcDescr = (char *)fvi.QueryValue();

  if (!pcDescr)
    pcDescr = "LightRepeater statistics";

  if (!plrs->IsFileView())
    {
      delete plrs;
      win_perror("Error opening statistics file "
		 LR_STATISTICS_FILE_NAME);
      return 1;
    }

  puts("Current statistics from LightRepeater logfile:\n");
  PrintSystemTime(plrs->stLastOpenedTime, "Last opened time:\t\t");
  PrintSystemTime(plrs->stLastClosedTime, "Last closed time:\t\t");
  puts("");
  PrintSystemTime(plrs->stLongestSessionOpenTime,
		  "Longest session's open time:\t");
  PrintSystemTime(plrs->stLongestSessionCloseTime,
		  "Longest session's close time:\t");
  puts("");
  PrintSystemTime(plrs->stShortestSessionOpenTime,
		  "Shortest session's open time:\t");
  PrintSystemTime(plrs->stShortestSessionCloseTime,
		  "Shortest session's close time:\t");
  puts("");
  PrintMilliseconds(plrs->dwLongestSessionTime, "Longest session TX time:\t");
  PrintMilliseconds(plrs->dwShortestSessionTime,
		    "Shortest session TX time:\t");
  puts("");
  PrintMilliseconds((QWORD) plrs->dwTotalTXTime * 1000, "Total TX time:\t\t");
  printf("\n"
	 "QSO count:\t\t%u\n"
	 "Gummitumme count:\t%u\n"
	 "Long speaker timeouts:\t%u\n",
	 plrs->dwQSOCount,
	 plrs->dwGummitummeCount,
	 plrs->dwLongSpeakerCount);

  delete plrs;
  puts("\r\nPress <Enter> to continue...");
  getchar();

  return 0;
}
