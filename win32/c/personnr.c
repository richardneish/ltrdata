#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>

#pragma comment(lib, "kernel32")

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "nullcrt")
#endif

int main()
{
  SYSTEMTIME t;
  char buffer[261];
  int checksum = 0;
  char buf[3];
  int count, cou, checkfig;
  FILETIME ft;

  // Meddelande om vad anv�ndaren ska g�ra
  puts("Skriv in personnummer eller organisationsnummer i direkt f�ljd,\r\n"
       "utan mellanslag eller bindestreck.");

  // Inmatningsbuffert. Inmatning med cin.
  fgets(buffer, sizeof buffer, stdin);
  buffer[strlen(buffer) - 1] = 0;

  // Utr�kning av kontrollsumman. Vandra till tecken 9 i
  // str�ngen och addera till checksum-variabeln.
  for (count = 0; count < 9; count++)
    if (count & 1)
      // Udda offset ger siffran som den �r.
      checksum += buffer[count] & 0xCF;
    else
      {
	// J�mn offset ger siffran * 2. T ex 14 ska d�
	// tolkas som 1+4, s� vi skapar en str�ng som vi
	// genoms�ker och adderar varje siffra i.
	sprintf(buf, "%u", (DWORD)((buffer[count] & 0xCF) << 1));
	for (cou = 0; buf[cou]; cou++)
	  checksum += buf[cou] & 0xCF;
      }

  // Nu ska vi kolla om det �r ett personnr eller orgnr.
  // Vi g�r detta genom att l�ta Windows kontrollera om
  // personnumret representerar ett korrekt f�delsedatum.
  // Fyll en SYSTEMTIME-struktur med f�delsedatum.
  t.wYear = (WORD) (strtoul(strncpy(buf, buffer, 2), NULL, 0) + 1900);
  t.wMonth = (WORD) strtoul(strncpy(buf, &(buffer[2]), 2), NULL, 0);
  t.wDay = (WORD) strtoul(strncpy(buf, &(buffer[4]), 2), NULL, 0);
  t.wHour = 0;
  t.wMinute = 0;
  t.wSecond = 0;
  t.wMilliseconds = 1;

  // Konvertera nu SYSTEMTIME-strukturen till en FILETIME.
  // Om den inte rapporterar n�got fel �r det ett giltigt
  // datum. D� �r det ett personnummer och inte orgnr,
  // skriv d� ut k�net p� personen.
  if (SystemTimeToFileTime(&t, &ft))
    printf("K�n: %s\n", buffer[8] & 1 ? "Man" : "Kvinna");
  else
    puts("Organisationsnummer.");

  // Konvertera kontrollsumman till en str�ng.
  sprintf(buffer, "%i", checksum);

  // Ber�kna 10 minus sista siffran i kontrollsumman.
  checkfig = 10 - (buffer[strlen(buffer) - 1] & 0xCF);

  // Om resultatet blev 10, s�tt 0.
  if (checkfig == 10)
    checkfig = 0;

  // Kontrollera att denna kontrollsiffra �r samma som den
  // inmatade.
  if (checkfig == (buffer[9] & 0xCF))
    puts("Korrekt kontrollsiffra.");
  else
    printf("Felaktigt. Kontrollsiffran ska vara %i.\n", checkfig);

  fgetc(stdin);

  return 0;
}
