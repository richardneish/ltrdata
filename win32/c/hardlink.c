#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>

#include <winstrct.h>
#include <shellapi.h>

#pragma comment(lib, "shell32")

#ifdef _DLL
#pragma comment(lib, "tinycrt")
#endif

int main(int argc, char **argv)
{
  if (argc != 3)
    {
      fputs("Usage: hardlink sourcefile targetfile\r\n", stderr);
      return 1;
    }

  if (CreateHardLink(argv[2], argv[1], NULL))
    return 0;

  win_perror(NULL);
  return 1;
}
