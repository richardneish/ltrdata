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

// Den h�r funktionen tar fram en matris med alla
// primfaktorer som talet no kan delas upp i. Matrisen
// kommer att termineras med 1. Uppst�r ett fel returneras
// false.
bool primefactorexpress( int primefactors[], int no )
{
   // Om talet �r mindre �n 2 f�rs�ker vi inte ens. Vi
   // returnerar en terminerande etta som enda inneh�ll i
   // matrisen.
   if( no < 2 )
   {
      primefactors[0] = 1;
      return true;  
   }

   // Om det �r ett j�mnt tal delar vi med 2 tills det blir
   // udda. F�r varje tv�a l�gger vi en tv�a i matrisen och
   // flyttar till n�sta element i den.
   while( (no & 1) == 0 )
   {
      *(primefactors++) = 2;

      // Flytta alla bitar ett steg h�ger ger division med
      // 2.
      no >>= 1;
      Sleep(0);   // Sov lite s� att vi inte �ter CPU-kraft
   }

   // Om talet nu �r mindre �n 3 (kan d� egentligen bara
   // vara 1 eller negativt) forts�tter vi inte. Vi
   // terminerar matrisen h�r och returnerar.
   if( no < 3 )
   {
      primefactors[0] = 1;
      return true;
   }

   // Antal udda tal som ska unders�kas �r antalet tal
   // delat med 2.
   int serieslength = no >> 1;

   // Allokera minne f�r primtalserien som ska genereras f�r
   // att unders�ka om tal �r primtal.
   int *primeseries = (int*)alloca(serieslength * sizeof(int));

   // Om alloca()-funktionen returnerade NULL s� fanns inte
   // tillr�ckligt med minne ledigt.
   if( primeseries == NULL )
     {
       fputs("Det finns inte tillr�ckligt minne.\r\n", stderr);
       return false;
     }

   // Vi b�rjar p� tal 3. L�ngden p� primtalserien �r fr�n
   // b�rjan 0.
   int    currentno = 3,
         currentserieslength = 0;
   // verbose-variabeln �r true om vi har kommit fram till
   // talet som ska unders�kas.
   bool    verbose = false;
   // Ta fram max s� m�nga primtal som f�r plats i primtal-
   // serie-matrisen som allokerades.
   for( int counter = 0; counter < serieslength; counter++ )
   {
      // Om vi �r p� sista talet (det som ska unders�kas)
      // s�tter vi variabeln verbose till true.
      verbose = counter + 1 == serieslength;

      if( verbose )
         verbose = true;

      // Anv�nd isprimeno()-funktionen f�r att ta reda p� om
      // talet vi �r p� �r ett primtal.
      if( isprimeno( currentno, primefactors, primeseries, currentserieslength, verbose ) )
      {
         // Om det nu �r ett primtal, s� l�gger vi till det
         // i v�r primtalsmatris och �kar l�ngden p� den.
         primeseries[currentserieslength++] = currentno;
         // Om vi �r p� sista talet...
         if( verbose )
         {
            // ...s� har vi uppt�ckt att det var ett
            //   primtal. Primfaktorserien blir allts� bara
            // talet sj�lvt. Vi terminerar ocks� med en
            // etta.
            primefactors[0] = no;
            primefactors[1] = 1;
            return true;
         }
      }

      // D� g�r vi vidare till n�sta udda tal.
      currentno += 2;
      Sleep(0);   // Sov lite s� att vi inte �ter CPU-kraft
   }

   return true;
}

// Den h�r funktionen tar reda p� om talet no �r ett
// primtal med hj�lp av primtalserien primeseries med
// l�ngden currentserieslength. Om verbose �r true placeras
// talets primfaktorer i primefactors.
// Funktionen returnerar true om no �r ett primtal, annars
// false.
bool isprimeno( int no, int primefactors[], int* &primeseries, const int &currentserieslength, const bool &verbose )
{
   int quote;

   // Vi s�ker igenom hela primtalserien f�r att hitta
   // n�got som talet no kan delas med.
   for( int seekcou = 0; seekcou < currentserieslength; seekcou ++ )
      // Om det h�r talet �r talet vi �r p� i matrisen
      if( no == primeseries[seekcou] )
      {
         // S� �r det givetvis ett primtal
         primefactors[0] = primeseries[seekcou];
         primefactors[1] = 1;
         Sleep(0); // Sov lite s� att vi inte �ter CPU-kraft
         return true;
      }
      // Om no �r delbart med det h�r talet (Kvoten sparas i en variabel)
      else if( (quote = no / primeseries[seekcou]) * primeseries[seekcou] == no )
      {
         // Om vi ska g�ra en primfaktorlista...
         if( verbose )
         {
            // ...s� �r ju f�r det f�rsta det h�r talet en
            // primfaktor. Vi s�tter sedan en terminering
            // eftersom vi inte vet om det finns fler prim-
            // faktorer.
            primefactors[0] = primeseries[seekcou];
            primefactors[1] = 1;

            // Vi anv�nder ocks� den h�r funktionen f�r att
            // ta reda p� om kvoten �r ett primtal. Vi
            // skickar d� en pekare till ett element l�ngre
            // fram i matrisen primefactors, eftersom den
            // kapslade funktionen ska behandla n�sta
            // primfaktor.
            isprimeno( quote, &(primefactors[1]), primeseries, currentserieslength, verbose );
         }
         Sleep(0); // Sov lite s� att vi inte �ter CPU-kraft
         return false;
      }

   Sleep(0);   // Sov lite s� att vi inte �ter CPU-kraft
   return true;
}

