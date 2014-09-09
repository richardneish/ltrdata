#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <stdlib.h>
#include <wio.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

int
main(int argc, char **argv)
{
  unsigned int buffersize = 1024;

  if (argc > 1)
    buffersize = strtoul(argv[1], NULL, 0);

  char *buffer = new char[buffersize];

  if (!LineRecv(hStdIn, buffer, buffersize))
    return 0;

  puts(buffer);

  return (int) strlen(buffer);
}
