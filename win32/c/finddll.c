#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <stdio.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

int
main(int argc, char **argv)
{
  HINSTANCE hLib;
  char cLibName[MAX_PATH+1] = "";
  FARPROC fp;

  if (argv[1] == NULL)
    {
      puts("Usage: FINDDLL dllfile [functionname]");
      return 0;
    }

  SetErrorMode(SEM_FAILCRITICALERRORS |
	       SEM_NOGPFAULTERRORBOX |
	       SEM_NOOPENFILEERRORBOX);

  hLib = LoadLibrary(argv[1]);
  if (hLib == NULL)
    {
      win_perror(argv[1]);
      return 1;
    }

  GetModuleFileName(hLib, cLibName, sizeof cLibName);
  printf("Successfully loaded: %s\n", cLibName);

  if (argv[2] == NULL)
    return 0;

  fp = GetProcAddress(hLib, argv[2]);
  if (fp == NULL)
    {
      win_perror(argv[2]);
      return 2;
    }

  printf("Procedure address is: %p\n", fp);

  return 0;
}
