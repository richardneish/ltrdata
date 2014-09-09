#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wstring.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

char * memstr(char *mem, const char *fnd, size_t max)
{
  size_t i;
  size_t len = strlen(fnd);

  if (len > max)
    return NULL;

  for (i = 0; i <= max - len; i++)
    {
      size_t r = 0;
      for (;;)
	{
	  if (r >= len)
	    return mem + i;

	  if (mem[i+r] != fnd[r])
	    break;

	  r++;
	}
    }

  return NULL;
}
