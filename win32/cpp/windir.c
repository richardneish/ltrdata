#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int main(int argc, char **argv)
{
  char cBuf[MAX_PATH] = "";

  if (argv[1] ? stricmp(argv[1], "/SYS") == 0: FALSE)
    GetSystemDirectory(cBuf, sizeof cBuf);
  else
    GetWindowsDirectory(cBuf, sizeof cBuf);

  puts(cBuf);
  return 0;
}
