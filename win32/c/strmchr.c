#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

#define SLEEP_INTERVAL_BYTES 65536

int main(int argc, char **argv)
{
  unsigned char c;
  char *ptr;
  DWORD dw;
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  int counter = 0;
  int randomchr = 0;

  if (argc > 1)
    {
      c = (char)strtoul(argv[1], &ptr, 0); 
      if ((ptr[0] != 0) | (argc > 2))
	{
	  fputs("STRMCHR by Olof Lagerkvist 2004.\r\n"
		"http://www.ltr-data.se     olof@ltr-data.se\r\n"
		"Syntax:\r\n"
		"STRMCHR [asciivalue]\r\n"
		"\r\n"
		"Writes the character specified by asciivalue parameter\r\n"
		"to stdout until stdout is closed, that is e.g. when\r\n"
		"output device is full or pipe is ended.\r\n"
		"\r\n"
		"If asciivalue parameter is omitted, the program writes\r\n"
		"random characters to stdout.\r\n",
		stderr);
	  return 1;
	}
    }
  else
    {
      srand((unsigned int)time(NULL));
      randomchr = 1;
    }

  for (;;)
    {
      if (randomchr)
	c = (char)rand();

      WriteFile(hOut, &c, 1, &dw, NULL);
      if (dw != 1)
	return 0;
      if (++counter > SLEEP_INTERVAL_BYTES)
	{
	  Sleep(0);
	  counter = 0;
	}
    }
}
