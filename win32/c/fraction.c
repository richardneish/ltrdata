#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <wimath.h>

#pragma comment(lib, "winstrct")
#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "nullcrt")
#endif

#else

#include "imathhlp.h"

#endif

int gcd(int, int);

int
main()
{
  // Buffert f�r inmatad text
  char buffer[261];
  char *cn;
  int t, n, f;

  // En loop d�r allt h�nder
  for (;;)
    {
      // Fr�ga efter br�ktal

      puts("Ange br�ktal p� formen t/n");
      fgets(buffer, sizeof buffer, stdin);

      // Ta fram en pekare till br�kstrecket. Om det inte
      // finns n�got br�kstreck avslutas programmet.
      if ((cn = strchr(buffer, '/')) == NULL)
	return 0;

      // Terminera d�r br�kstrecket �r, och flytta fram
      // pekaren ett tecken. D� kommer buffer att peka p�
      // t�ljaren och cn p� n�mnaren.
      *(cn++) = 0;

      // Omvandla buffer och cn till int-typer och ta fram
      // h�gsta m�jliga f�rkortningsfaktorn med funktionen
      // gcd()
      t = strtoul(buffer, NULL, 0);
      n = strtoul(cn, NULL, 0);
      f = gcd(t, n);

      // Om vi fick en faktor f�rkortar vi med den och
      // skriver resultatet p� konsolen.
      if (f)
	printf("Br�ket �r f�renklat: %i/%i. "
	       "(F�rkortningsfaktor: %i).\n", t / f, n / f, f);
      // Om vi inte fick n�gon faktor m�ste det blivit fel.
      else
	puts("Hmm... Det blev visst fel...");
    }
}

/*
// Den h�r funktionen r�knar ut h�gsta m�jliga
// f�rkortningsfaktor f�r ett br�k med t�ljaren t och
// n�mnaren n. Uppst�r ett fel returneras 0.
int gcd(int t, int n)
{
   // Vi utg�r fr�n att faktorn �r 1 f�r att vi ska kunna
   // multiplicera med alla primfaktorer i matrisen som
   // funktionen primefactorexpress() genererar.
   // Om b�de t�ljare och n�mnare �r negativa s� ska
   // den h�r funktionen returnera ett negativt tal.
   int f = 1;
   if( n < 0 )
   {
      if( t < 0 )
         f = -1;

      n = -n;
   }
   if( t < 0 )
      t = -t;

   // Vi allokerar ocks� minne f�r tv� primfaktormatriser
   // med maximalt 33 element. 31 �r maximalt antal
   // element som kan f�rekomma. Jag vill ha lite marginal.
   int *t_primefactors = (int*)malloc(33 * sizeof(int));
   int *n_primefactors = (int*)malloc(33 * sizeof(int));

   if ((t_primefactors == NULL) | (n_primefactors == NULL))
      throw;

   // Anv�nd funktionen primefactorexpress() f�r att f�
   // en primfaktormatris av t och n. Skulle det uppst� fel
   // i funktionerna returnerar dom false och f�r den h�r
   // funktionen att returnera 0.
   if( primefactorexpress( t_primefactors, t ) == false )
      return 0;

   if( primefactorexpress( n_primefactors, n ) == false )
      return 0;

   // Primfaktormatriserna termineras med talet 1.
   // Loopa d�rf�r inte l�ngre �n till en terminering.
   while( (t_primefactors[0] > 1) & (n_primefactors[0] > 1) )
   {
      Sleep(0);    // Sov lite s� att vi inte �ter CPU-kraft

      // Om det finns tv� lika faktorer i matrisen...
      while( t_primefactors[0] == n_primefactors[0] )
      {
         Sleep(0); // Sov lite s� att vi inte �ter CPU-kraft

         // Om vi st�ter p� en terminering (1) s� m�ste vi
         // sluta.
         if( (t_primefactors[0] <= 1) | (n_primefactors[0] <= 1) )
            break;

         // ...s� multiplicerar vi h�gsta f�rkortnings-
         // faktorn med denna gemensamma faktor...
         f *= t_primefactors[0];

         // ...och flyttar fram till n�sta tal i matriserna.
         ++t_primefactors;
         ++n_primefactors;
      }

      // Sedan m�ste vi f�rs�ka hitta tv� gemensamma
      // primfaktorer igen.
      do
      {
         Sleep(0); // Sov lite s� att vi inte �ter CPU-kraft

         // Om vi st�ter p� en terminering (1) s� m�ste vi
         // sluta.
         if( (t_primefactors[0] <= 1) | (n_primefactors[0] <= 1) )
            break;

         // Om vi befinner oss p� en t�ljarfaktor som �r
         // st�rre �n den n�mnarfaktor vi befinner oss p�
         // s� flyttar vi fram till n�sta n�mnarfaktor, som
         // d� kanske �r gemensam.
         if( t_primefactors[0] > n_primefactors[0] )
         {
            ++n_primefactors;
            continue;
         }

         // Om vi befinner oss p� en n�mnarfaktor som �r
         // st�rre �n den t�ljarfaktor vi befinner oss p�
         // s� flyttar vi fram till n�sta t�ljarfaktor, som
         // d� kanske �r gemensam.
         if( t_primefactors[0] < n_primefactors[0] )
         {
            ++t_primefactors;
            continue;
         }
      // K�r den h�r loopen tills vi hittar en ny gemensam
      // primfaktor.
      } while( t_primefactors[0] != n_primefactors[0] );
   }

   // Returnera den h�gsta f�rkortningsfaktorn vi har f�tt
   // fram.
   return f;
}
*/
