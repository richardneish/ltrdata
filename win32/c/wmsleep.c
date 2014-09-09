#include <unistd.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int msleep(unsigned long msec)
{
  Sleep(msec);
  return 0;
}
