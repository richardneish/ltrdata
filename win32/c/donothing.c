#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma comment(lib, "kernel32")

#pragma comment(linker, "/entry:main")
#pragma comment(linker, "/subsystem:windows")

int main()
{
  ExitProcess(0);
}
