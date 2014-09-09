#include <time.h>
#include <stdio.h>

#ifdef _WIN32

#include <conio.h>
#include <wtypes.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "nullcrt")
#endif

#else

#include <curses.h>

#endif

int
main()
{
  // Den h�r variabeln m�ste deklareras utanf�r loopen f�r
  // att kunna anv�ndas senare.
  clock_t begintk;
  double diff;

  for (;;)
    {
      puts("Efter pipet ska du v�nta 10 sek och sedan trycka p� mellanslag.\r\n"
	   "Tryck p� en tangent n�r du k�nner dig redo.");

      getchar();
      putchar('\a');		// Pip

      begintk = clock();	// Spara tiden

      // V�nta p� tangenttryckning. Om annan tangent �n
      // mellanslag, protestera!
      if (getch() != ' ')
	puts("Sa jag inte till dig att det var mellanslag du skulle trycka p�?\r\n"
	     "Nu f�rs�ker vi igen ;)");
      else
	break;			// Annars hoppar vi ut ur loopen
    }

  // Ta tidsdifferensen, konvertera till double och dela
  // med konstanten CLK_TCK.
  diff = (double)(clock() - begintk) / CLK_TCK;

  // Olika meddelanden beroende p� om anv var f�r l�ngsam,
  // exakt eller f�r snabb.
  if (diff > 10)
    printf("HA! Du var %g sek f�r l�ngsam!\n", diff - 10);
  else if (diff < 10)
    printf("HA! Du var %g sek f�r snabb!\n", 10 - diff);
  else
    puts("Du tryckte vid exakt r�tt tid!!!");

  getch();

  return 0;
}
