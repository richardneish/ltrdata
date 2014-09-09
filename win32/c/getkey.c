#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma comment(lib, "kernel32")

#pragma comment(linker, "/subsystem:console")
#pragma comment(linker, "/entry:main")

int
main()
{
  HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
  unsigned char c;
  DWORD dw;

  SetConsoleMode(hIn, 0);

  if (ReadFile(hIn, &c, 1, &dw, NULL))
    if (dw == 1)
      {
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), &c, 1, &dw, NULL);
	ExitProcess(c | ('a' - 'A'));
      }
    else
      ExitProcess(0);
  else
    ExitProcess(0);
}
