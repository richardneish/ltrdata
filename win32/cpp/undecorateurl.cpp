#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

char buffer[4096];

unsigned int
undecorateurlname(char *buffer, size_t sourcelength)
{
  char *writeptr = buffer;
  int targetlength = 0;

  for (size_t done = 0; done < sourcelength; done++)
    {
      ++targetlength;

      if (buffer[done] == '%')
	if (isxdigit(buffer[done + 1]))
	  if (isxdigit(buffer[done + 2]))
	    {
	      char hexstr[] = { buffer[done+1], buffer[done+2], 0 };
	      *(writeptr++) = (char)strtoul(hexstr, NULL, 16);
	      done += 2;
	      continue;
	    }

      *(writeptr++) = buffer[done];
    }

  return targetlength;
}

int
main(int argc, char **argv)
{
  if (argc == 1)
    for (;;)
      {
	size_t readsize = fread(buffer, 1, sizeof buffer, stdin);
	if (readsize == 0)
	  {
	    if (ferror(stdin))
	      perror(NULL);

	    break;
	  }

	unsigned writesize = undecorateurlname(buffer, readsize);

	if (fwrite(buffer, writesize, 1, stdout) != writesize)
	  {
	    if (ferror(stdout))
	      perror(NULL);

	    break;
	  }	
      }
  else
    while (--argc)
      {
	++argv;

	char *newname = strdup(*argv);
	unsigned newlength = undecorateurlname(newname, strlen(newname));
	newname[newlength] = 0;
	if (rename(*argv, newname) != 0)
	  perror(*argv);
	free(newname);
      }

  return 0;
}
