#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#  if defined(_DLL) && !defined(_WIN64)
#     pragma comment(lib, "minwcrt")
#  endif
#else
char *strlwr(char *s);
#endif

int main(int argc, char **argv)
{
  char *newname;

  while ((++argv)[0])
    {
      newname = strdup(argv[0]);
      if (newname == NULL)
	{
	  fprintf(stderr, "%s: too long file name.\n", argv[0]);
	  continue;
	}

      if (rename(argv[0], strlwr(newname)))
	perror(argv[0]);

      free(newname);
    }
}

