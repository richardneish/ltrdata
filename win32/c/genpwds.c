#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

int main(int argc, char **argv)
{
  int i;
  int passwds = 1;
  char randletter;

  srand((unsigned int)time(NULL));

  if (argv[1])
    passwds = strtoul(argv[1], NULL, 0);

  while (passwds--)
    {
      int vokaldags = 0;
      for (i = 1; i <= 4; i++)
   	{
	  while (strchr("CQWXZ",
			randletter = (char)('A' + rand() % ('Z' - 'A' + 1))));

	  if (strchr("AOUEIY", randletter))
	    {
	      if (vokaldags > 0)
		while (strchr("CQWXZAOUEIY",
			      randletter = (char)('A' + rand() %
						  ('Z' - 'A' + 1))));

	      vokaldags = 1;
	    }
	  else
	    vokaldags--;

	  putchar(randletter);
   	}

      printf("    ");
   }
}
