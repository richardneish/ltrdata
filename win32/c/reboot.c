#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "shell32.lib")

#pragma comment(linker, "/entry:startup")

int
ShutdownMachine(LPWSTR lpMachineName, BOOL bRebootFlag);

int
wmain(int argc, LPWSTR *argv)
{
  return ShutdownMachine(argc > 1 ? argv[1] : NULL, TRUE);
}

int
startup()
{
  int argc = 0;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
  if (argv == NULL)
    {
      fputs("This program requires Windows NT.\r\n", stderr);
      exit(-1);
    }
  else
    exit(wmain(argc, argv));
}

