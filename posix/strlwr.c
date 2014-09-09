/* strlwr.c:    Convert a string to lower case */

#include <stdlib.h>
#include <ctype.h>

char *strlwr(char *s)
{
   if (s != NULL)
   {
      char *p;

      for (p = s; *p; ++p)
         *p = tolower(*p);
   }
   return s;
}

/* End of File */

