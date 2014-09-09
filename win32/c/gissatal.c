#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "nullcrt")
#endif

int main()
{
  // Variablerna HighestGuessedValue/Lowest... lagrar det
  // högsta och det lägsta värdet som anv har gissat på.
  // Används för att tala om för anv att han/hon gissar på
  // ett tal som han/hon borde veta sedan tidigare
  // gissningar att det är för högt eller lågt.

  int
    HighestGuessedValue = 99,
    LowestGuessedValue = 1,
    Guessed,    // Lagrar det gissade talet
    Try = 0,    // Antal försök hittills
    Value;

  srand((unsigned int)time(NULL));     // Initiera slumptalsgeneratorn
  Value = (rand() % 99) + 1;   // Ett nytt slumptal

  // En loop där allt händer
  for (;;)
   {
     printf("F”rs”k nr %i\nGissa heltal mellan 1 och 99:\n", ++Try);
     scanf("%i", &Guessed);

     // Om anv gissar på ett tal som är mindre än 1 eller
     // större än 99, protestera och hoppa tillbaka till
     // början av loopen.
     if ((Guessed < 1) | (Guessed > 99))
       {
	 puts("Mellan 1 och 99 var det best„mt. F”rs”k igen!");
	 continue;
       }

     // Om anv gissade rätt, visa meddelande och hoppa ut
     // ur loopen.
     if (Guessed == Value)
       {
	 printf("R„tt! Du beh”vde %i f”rs”k!\n", Try);
	 break;
       }

     // Om det gissade talet är större än det största anv
     // gissat på som var för stort, protestera och kör
     // om loopen.
     if (Guessed >= HighestGuessedValue)
       {
	 puts("Jag har ju sagt att talet „r f”r h”gt!");
         continue;
       }

     // Om det gissade talet är mindre än det minsta anv
     // gissat på som var för litet, protestera och kör
     // om loopen.
     if (Guessed <= LowestGuessedValue)
       {
	 puts("Jag har ju sagt att talet „r f”r l†gt!");
         continue;
       }

     // Om det gissade talet är för stort, meddela anv och
     // kör om loopen.
     if (Guessed > Value)
       {
	 HighestGuessedValue = Guessed;
	 puts("Talet „r f”r h”gt!");
         continue;
       }

     // Om det gissade talet är för litet, meddela anv och
     // kör om loopen.
     if (Guessed < Value)
       {
	 LowestGuessedValue = Guessed;
	 puts("Talet „r f”r l†gt!");
         continue;
       }
   }

  return 0;
}
