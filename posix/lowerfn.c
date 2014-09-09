#include <stdio.h>
#include <string.h>

#ifndef _WIN32
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

