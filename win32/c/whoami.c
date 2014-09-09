#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

#pragma comment(lib, "advapi32")
#pragma comment(lib, "winstrct")
#pragma comment(lib, "nullcrt")

int main()
{
  char cBuf[MAX_PATH];
  int size = sizeof cBuf;

  if (GetUserName(cBuf, &size))
    puts(cBuf);
  else
    win_perror(NULL);
}
