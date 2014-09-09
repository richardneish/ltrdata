#ifdef _WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <winstrct.h>
#       if defined(_DLL) && !defined(_WIN64)
#               pragma comment(lib, "minwcrt")
#       endif
#       define unlink(f) (!DeleteFile(f))
#       define perror(m) win_perror(m)
#else
#	include <stdio.h>
#	include <errno.h>
#	include <unistd.h>
#endif

int main(int argc, char **argv)
{
  if (argc < 1)
    {
      puts("No file to delete.");
      return 0;
    }

  while ((++argv)[0])
    {
      if (unlink(argv[0]))
	perror(argv[0]);
      else
	printf("Deleted: %s\n", argv[0]);
    }
}
