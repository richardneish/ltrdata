#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

int
main(int argc, char **argv)
{
  while (argc-- > 1)
    puts(strerror(strtoul((argv++)[1], NULL, 0)));
}
