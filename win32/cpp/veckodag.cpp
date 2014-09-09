#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <wtime.h>

#pragma comment(lib, "kernel32")

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "nullcrt")
#endif

int main()
{
  // En buffert för cin-inmatning
  char buffer[MAX_PATH+1];

  // Meddelande vad anv ska göra
  puts("Skriv in datum p† formen ††††-mm-dd.");
  fgets(buffer, sizeof buffer, stdin);
  buffer[strlen(buffer)-1] = 0;

  // Plocka ut år, dag och månad ur inbufferten.
  // Terminera vid tecknen - så att vi slipper kopiera
  // strängarna någon annanstans.
  if (strtok(buffer, "-") == NULL)
    return 0;

  char *mnthptr = strtok(NULL, "-");
  if (mnthptr == NULL)
    return 0;
  
  char *dayptr = strtok(NULL, "-");
  if (dayptr == NULL)
    return 0;
  
  // Fyll SYSTEMTIME-strukturen t med informationen
  WSystemTime t;
  t.wYear = (WORD)strtoul(buffer, NULL, 0);
  t.wMonth = (WORD)strtoul(mnthptr, NULL, 0);
  t.wDay = (WORD)strtoul(dayptr, NULL, 0);
  t.wDayOfWeek = 7;
  t.wHour = 0;
  t.wMinute = 0;
  t.wSecond = 0;
  t.wMilliseconds = 1;

  // Konvertera nu SYSTEMTIME-strukturen till en FILETIME och tillbaka igen.
  t = (WFileTime)t;

  // Matris med alla veckodagar
  const char *wday[] =
    {
      "S”ndag", "M†ndag", "Tisdag", "Onsdag", "Torsdag", "Fredag",
      "L”rdag"
    };

  // Skriv resultat på konsolen.
  if (t.wDayOfWeek > 6)
    fputs("Felaktigt datum.\r\n", stderr);
  else
    printf("Veckodag: %s\n", wday[t.wDayOfWeek]);
  getchar();

  return 0;
}

