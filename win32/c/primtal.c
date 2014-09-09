#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#  include <wimath.h>
#  pragma comment(lib, "winstrct")
#  if defined(_DLL) && !defined(_WIN64)
#    pragma comment(lib, "nullcrt")
#  endif
#endif

#pragma warning(disable: 4100)

int factorprinted = 0;

int printfactor(void *u, int f)
{
  printf(factorprinted ? " * %i" : "%i", f);
  factorprinted = 1;
  return 0;
}

int
main()
{
  int no;

  for (;;)
    {
      no = 0;

      puts("Vilket tal vill du primfaktoruppdela?");
      scanf("%i", &no);

      if (no <= 0)
	break;

      factorprinted = 0;
      getprimefactors(no, NULL, printfactor);
      printf(" = %i\n", no);
    }

  return 0;
}
