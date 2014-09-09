#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wconsole.h>

#pragma comment(lib, "kernel32")

EXTERN_C void clreol()
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE hConOut = GetStdHandle(STD_OUTPUT_HANDLE);
  LPSTR cBlankStr;
  DWORD dwWriteLength;

  if (!GetConsoleScreenBufferInfo(hConOut, &csbi))
    return;

  dwWriteLength = csbi.dwSize.X - csbi.dwCursorPosition.X;
  cBlankStr = (LPSTR)HeapAlloc(GetProcessHeap(), 0, dwWriteLength);
  if (cBlankStr == (LPSTR)NULL)
    return;
  FillMemory(cBlankStr, dwWriteLength, ' ');

  WriteConsoleOutputCharacter(hConOut, cBlankStr, dwWriteLength,
			      csbi.dwCursorPosition, &dwWriteLength);

  HeapFree(GetProcessHeap(), 0, (LPVOID)cBlankStr);
}  

EXTERN_C void gotoxy(SHORT x, SHORT y)
{
  COORD coord;
  coord.X = x-1;
  coord.Y = y-1;

  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}  

EXTERN_C void clrscr()
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE hConOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD dwConSize;
  LPSTR lpStr;
  DWORD c;
  COORD coord;

  if (!GetConsoleScreenBufferInfo(hConOut, &csbi))
    return;

  coord.X = 0;
  coord.Y = 0;

  dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
  lpStr = (LPSTR)HeapAlloc(GetProcessHeap(), 0, dwConSize);
  c = dwConSize;

  while (c--)
    lpStr[c] = ' ';

  WriteConsoleOutputCharacter(hConOut, lpStr, dwConSize, coord, &dwConSize);
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

  HeapFree(GetProcessHeap(), 0, (LPVOID)lpStr);
}
