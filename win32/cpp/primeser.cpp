#include <stdio.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <malloc.h>
#else
#include <unistd.h>
#include <stdlib.h>
#define Sleep(n) usleep(n)
#endif

#include "primeser.h"

#pragma comment(lib, "kernel32")

// Den här funktionen tar fram en matris med alla
// primfaktorer som talet no kan delas upp i. Matrisen
// kommer att termineras med 1. Uppstår ett fel returneras
// false.
bool primefactorexpress( int primefactors[], int no )
{
   // Om talet är mindre än 2 försöker vi inte ens. Vi
   // returnerar en terminerande etta som enda innehåll i
   // matrisen.
   if( no < 2 )
   {
      primefactors[0] = 1;
      return true;  
   }

   // Om det är ett jämnt tal delar vi med 2 tills det blir
   // udda. För varje tvåa lägger vi en tvåa i matrisen och
   // flyttar till nästa element i den.
   while( (no & 1) == 0 )
   {
      *(primefactors++) = 2;

      // Flytta alla bitar ett steg höger ger division med
      // 2.
      no >>= 1;
      Sleep(0);   // Sov lite så att vi inte äter CPU-kraft
   }

   // Om talet nu är mindre än 3 (kan då egentligen bara
   // vara 1 eller negativt) fortsätter vi inte. Vi
   // terminerar matrisen här och returnerar.
   if( no < 3 )
   {
      primefactors[0] = 1;
      return true;
   }

   // Antal udda tal som ska undersökas är antalet tal
   // delat med 2.
   int serieslength = no >> 1;

   // Allokera minne för primtalserien som ska genereras för
   // att undersöka om tal är primtal.
   int *primeseries = (int*)alloca(serieslength * sizeof(int));

   // Om alloca()-funktionen returnerade NULL så fanns inte
   // tillräckligt med minne ledigt.
   if( primeseries == NULL )
     {
       fputs("Det finns inte tillr„ckligt minne.\r\n", stderr);
       return false;
     }

   // Vi börjar på tal 3. Längden på primtalserien är från
   // början 0.
   int    currentno = 3,
         currentserieslength = 0;
   // verbose-variabeln är true om vi har kommit fram till
   // talet som ska undersökas.
   bool    verbose = false;
   // Ta fram max så många primtal som får plats i primtal-
   // serie-matrisen som allokerades.
   for( int counter = 0; counter < serieslength; counter++ )
   {
      // Om vi är på sista talet (det som ska undersökas)
      // sätter vi variabeln verbose till true.
      verbose = counter + 1 == serieslength;

      if( verbose )
         verbose = true;

      // Använd isprimeno()-funktionen för att ta reda på om
      // talet vi är på är ett primtal.
      if( isprimeno( currentno, primefactors, primeseries, currentserieslength, verbose ) )
      {
         // Om det nu är ett primtal, så lägger vi till det
         // i vår primtalsmatris och ökar längden på den.
         primeseries[currentserieslength++] = currentno;
         // Om vi är på sista talet...
         if( verbose )
         {
            // ...så har vi upptäckt att det var ett
            //   primtal. Primfaktorserien blir alltså bara
            // talet självt. Vi terminerar också med en
            // etta.
            primefactors[0] = no;
            primefactors[1] = 1;
            return true;
         }
      }

      // Då går vi vidare till nästa udda tal.
      currentno += 2;
      Sleep(0);   // Sov lite så att vi inte äter CPU-kraft
   }

   return true;
}

// Den här funktionen tar reda på om talet no är ett
// primtal med hjälp av primtalserien primeseries med
// längden currentserieslength. Om verbose är true placeras
// talets primfaktorer i primefactors.
// Funktionen returnerar true om no är ett primtal, annars
// false.
bool isprimeno( int no, int primefactors[], int* &primeseries, const int &currentserieslength, const bool &verbose )
{
   int quote;

   // Vi söker igenom hela primtalserien för att hitta
   // något som talet no kan delas med.
   for( int seekcou = 0; seekcou < currentserieslength; seekcou ++ )
      // Om det här talet är talet vi är på i matrisen
      if( no == primeseries[seekcou] )
      {
         // Så är det givetvis ett primtal
         primefactors[0] = primeseries[seekcou];
         primefactors[1] = 1;
         Sleep(0); // Sov lite så att vi inte äter CPU-kraft
         return true;
      }
      // Om no är delbart med det här talet (Kvoten sparas i en variabel)
      else if( (quote = no / primeseries[seekcou]) * primeseries[seekcou] == no )
      {
         // Om vi ska göra en primfaktorlista...
         if( verbose )
         {
            // ...så är ju för det första det här talet en
            // primfaktor. Vi sätter sedan en terminering
            // eftersom vi inte vet om det finns fler prim-
            // faktorer.
            primefactors[0] = primeseries[seekcou];
            primefactors[1] = 1;

            // Vi använder också den här funktionen för att
            // ta reda på om kvoten är ett primtal. Vi
            // skickar då en pekare till ett element längre
            // fram i matrisen primefactors, eftersom den
            // kapslade funktionen ska behandla nästa
            // primfaktor.
            isprimeno( quote, &(primefactors[1]), primeseries, currentserieslength, verbose );
         }
         Sleep(0); // Sov lite så att vi inte äter CPU-kraft
         return false;
      }

   Sleep(0);   // Sov lite så att vi inte äter CPU-kraft
   return true;
}

