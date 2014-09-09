#include <stdio.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "nullcrt")
#endif

int main()
{
  char c;
  for (c = 0x20; c; c++)
    putchar(c);
  return 0;
}
