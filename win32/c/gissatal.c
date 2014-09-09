#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "nullcrt")
#endif

int main()
{
  // Variablerna HighestGuessedValue/Lowest... lagrar det
  // h�gsta och det l�gsta v�rdet som anv har gissat p�.
  // Anv�nds f�r att tala om f�r anv att han/hon gissar p�
  // ett tal som han/hon borde veta sedan tidigare
  // gissningar att det �r f�r h�gt eller l�gt.

  int
    HighestGuessedValue = 99,
    LowestGuessedValue = 1,
    Guessed,    // Lagrar det gissade talet
    Try = 0,    // Antal f�rs�k hittills
    Value;

  srand((unsigned int)time(NULL));     // Initiera slumptalsgeneratorn
  Value = (rand() % 99) + 1;   // Ett nytt slumptal

  // En loop d�r allt h�nder
  for (;;)
   {
     printf("F�rs�k nr %i\nGissa heltal mellan 1 och 99:\n", ++Try);
     scanf("%i", &Guessed);

     // Om anv gissar p� ett tal som �r mindre �n 1 eller
     // st�rre �n 99, protestera och hoppa tillbaka till
     // b�rjan av loopen.
     if ((Guessed < 1) | (Guessed > 99))
       {
	 puts("Mellan 1 och 99 var det best�mt. F�rs�k igen!");
	 continue;
       }

     // Om anv gissade r�tt, visa meddelande och hoppa ut
     // ur loopen.
     if (Guessed == Value)
       {
	 printf("R�tt! Du beh�vde %i f�rs�k!\n", Try);
	 break;
       }

     // Om det gissade talet �r st�rre �n det st�rsta anv
     // gissat p� som var f�r stort, protestera och k�r
     // om loopen.
     if (Guessed >= HighestGuessedValue)
       {
	 puts("Jag har ju sagt att talet �r f�r h�gt!");
         continue;
       }

     // Om det gissade talet �r mindre �n det minsta anv
     // gissat p� som var f�r litet, protestera och k�r
     // om loopen.
     if (Guessed <= LowestGuessedValue)
       {
	 puts("Jag har ju sagt att talet �r f�r l�gt!");
         continue;
       }

     // Om det gissade talet �r f�r stort, meddela anv och
     // k�r om loopen.
     if (Guessed > Value)
       {
	 HighestGuessedValue = Guessed;
	 puts("Talet �r f�r h�gt!");
         continue;
       }

     // Om det gissade talet �r f�r litet, meddela anv och
     // k�r om loopen.
     if (Guessed < Value)
       {
	 LowestGuessedValue = Guessed;
	 puts("Talet �r f�r l�gt!");
         continue;
       }
   }

  return 0;
}
