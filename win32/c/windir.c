#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

int main(int argc, char **argv)
{
  char cBuf[MAX_PATH] = "";

  if (argv[1] ? stricmp(argv[1], "/SYS") == 0: FALSE)
    GetSystemDirectory(cBuf, sizeof cBuf);
  else
    GetWindowsDirectory(cBuf, sizeof cBuf);

  fputs(cBuf, stdout);
  return 0;
}
