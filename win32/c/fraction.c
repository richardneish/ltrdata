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
  // Buffert för inmatad text
  char buffer[261];
  char *cn;
  int t, n, f;

  // En loop där allt händer
  for (;;)
    {
      // Fråga efter bråktal

      puts("Ange br†ktal p† formen t/n");
      fgets(buffer, sizeof buffer, stdin);

      // Ta fram en pekare till bråkstrecket. Om det inte
      // finns något bråkstreck avslutas programmet.
      if ((cn = strchr(buffer, '/')) == NULL)
	return 0;

      // Terminera där bråkstrecket är, och flytta fram
      // pekaren ett tecken. Då kommer buffer att peka på
      // täljaren och cn på nämnaren.
      *(cn++) = 0;

      // Omvandla buffer och cn till int-typer och ta fram
      // högsta möjliga förkortningsfaktorn med funktionen
      // gcd()
      t = strtoul(buffer, NULL, 0);
      n = strtoul(cn, NULL, 0);
      f = gcd(t, n);

      // Om vi fick en faktor förkortar vi med den och
      // skriver resultatet på konsolen.
      if (f)
	printf("Br†ket „r f”renklat: %i/%i. "
	       "(F”rkortningsfaktor: %i).\n", t / f, n / f, f);
      // Om vi inte fick någon faktor måste det blivit fel.
      else
	puts("Hmm... Det blev visst fel...");
    }
}

/*
// Den här funktionen räknar ut högsta möjliga
// förkortningsfaktor för ett bråk med täljaren t och
// nämnaren n. Uppstår ett fel returneras 0.
int gcd(int t, int n)
{
   // Vi utgår från att faktorn är 1 för att vi ska kunna
   // multiplicera med alla primfaktorer i matrisen som
   // funktionen primefactorexpress() genererar.
   // Om både täljare och nämnare är negativa så ska
   // den här funktionen returnera ett negativt tal.
   int f = 1;
   if( n < 0 )
   {
      if( t < 0 )
         f = -1;

      n = -n;
   }
   if( t < 0 )
      t = -t;

   // Vi allokerar också minne för två primfaktormatriser
   // med maximalt 33 element. 31 är maximalt antal
   // element som kan förekomma. Jag vill ha lite marginal.
   int *t_primefactors = (int*)malloc(33 * sizeof(int));
   int *n_primefactors = (int*)malloc(33 * sizeof(int));

   if ((t_primefactors == NULL) | (n_primefactors == NULL))
      throw;

   // Använd funktionen primefactorexpress() för att få
   // en primfaktormatris av t och n. Skulle det uppstå fel
   // i funktionerna returnerar dom false och får den här
   // funktionen att returnera 0.
   if( primefactorexpress( t_primefactors, t ) == false )
      return 0;

   if( primefactorexpress( n_primefactors, n ) == false )
      return 0;

   // Primfaktormatriserna termineras med talet 1.
   // Loopa därför inte längre än till en terminering.
   while( (t_primefactors[0] > 1) & (n_primefactors[0] > 1) )
   {
      Sleep(0);    // Sov lite så att vi inte äter CPU-kraft

      // Om det finns två lika faktorer i matrisen...
      while( t_primefactors[0] == n_primefactors[0] )
      {
         Sleep(0); // Sov lite så att vi inte äter CPU-kraft

         // Om vi stöter på en terminering (1) så måste vi
         // sluta.
         if( (t_primefactors[0] <= 1) | (n_primefactors[0] <= 1) )
            break;

         // ...så multiplicerar vi högsta förkortnings-
         // faktorn med denna gemensamma faktor...
         f *= t_primefactors[0];

         // ...och flyttar fram till nästa tal i matriserna.
         ++t_primefactors;
         ++n_primefactors;
      }

      // Sedan måste vi försöka hitta två gemensamma
      // primfaktorer igen.
      do
      {
         Sleep(0); // Sov lite så att vi inte äter CPU-kraft

         // Om vi stöter på en terminering (1) så måste vi
         // sluta.
         if( (t_primefactors[0] <= 1) | (n_primefactors[0] <= 1) )
            break;

         // Om vi befinner oss på en täljarfaktor som är
         // större än den nämnarfaktor vi befinner oss på
         // så flyttar vi fram till nästa nämnarfaktor, som
         // då kanske är gemensam.
         if( t_primefactors[0] > n_primefactors[0] )
         {
            ++n_primefactors;
            continue;
         }

         // Om vi befinner oss på en nämnarfaktor som är
         // större än den täljarfaktor vi befinner oss på
         // så flyttar vi fram till nästa täljarfaktor, som
         // då kanske är gemensam.
         if( t_primefactors[0] < n_primefactors[0] )
         {
            ++t_primefactors;
            continue;
         }
      // Kör den här loopen tills vi hittar en ny gemensam
      // primfaktor.
      } while( t_primefactors[0] != n_primefactors[0] );
   }

   // Returnera den högsta förkortningsfaktorn vi har fått
   // fram.
   return f;
}
*/
