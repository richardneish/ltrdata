#include <wimath.h>

/*
 * Compute the greatest common divisor of a and b.
 */
int CDECL
gcd(int a,
    int b)
{
  int c = a % b;

  while (c != 0)
    {
      a = b;
      b = c;
      c = a % b;
    }
  
  return b;
}

/*
 * Return each prime factor for i to callback function f. If f returns != 0
 * the calculation is cancelled and this function returns 1. Otherwise it
 * returns 0. The u parameter can be any user data returned in each call to
 * callback function.
 */
int CDECL
getprimefactors(int i,
		void *u,
		int (CDECL *f)(void *, int))
{
  int c;

  while (i > 0)
    {
      if ((i & 1) == 0)
	if (f(u, 2))
	  return 1;
	else
	  {
	    i >>= 1;
	    continue;
	  }

      for (c = 3; i % c; c += 2)
	if (c > i)
	  return 0;

      if (f(u, c))
	return 1;

      i /= c;
    }

  return -1;
}
