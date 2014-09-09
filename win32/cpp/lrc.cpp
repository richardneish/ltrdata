#include <stdio.h>
#include <io.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "nullcrt.lib")
#endif

char buffer[65536];

int
main()
{
  char xor = 0;

  for (;;)
    {
      int readlength = read(0, buffer, sizeof buffer);

      if (readlength < 0)
	{
	  perror(NULL);
	  return 1;
	}

      if (readlength == 0)
	break;

      for (int i = 0; i < readlength; i++)
	xor ^= buffer[i];
    }

  printf("%.2X\n", xor);

  return 0;
}

