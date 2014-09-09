#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "nullcrt")
#endif

#define NUMBER_OF_NUMBERS 39

int main()
{
  int nummer[NUMBER_OF_NUMBERS];	// En lista med alla nummer
  int count, nr;

  srand((unsigned int)time(NULL));			// Ny slumptalserie

  do
    {
      // St�ll alla nummer i listan till false (= inte med)
      memset(nummer, 0, sizeof nummer);

      // Ta fram 7 nummer
      for (count=0; count < 7; count++)
	{
	  // Ta fram ett nytt nummer tills vi f�r ett nummer
	  // som vi inte f�tt tidigare.
	  while (nummer[nr = (rand() % NUMBER_OF_NUMBERS)]);

	  // S�tt detta unika nummer i listan till true.
	  nummer[nr] = 1;
   	}

      // G� igenom listan och skriv ut alla nummer som �r
      // flaggade.
      for (count=0; count < NUMBER_OF_NUMBERS; ++count)
	if (nummer[count])
	  printf("%i\t", count+1);
      
      // K�r tills Esc trycks.
    } while (fgetc(stdin) != EOF);

  return 0;
}
