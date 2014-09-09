#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma comment(lib, "kernel32")

#pragma comment(linker, "/subsystem:console")
#pragma comment(linker, "/entry:main")

int main()
{
  ExitProcess(GetOEMCP());
}

