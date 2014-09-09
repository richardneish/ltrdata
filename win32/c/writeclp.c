#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <winstrct.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "nullcrt")
#endif

int main()
{
  DWORD dwFileSizeHigh;
  DWORD dwFileSize;
  LPVOID pStorage;
  char *ptr;
  DWORD dwSize;
  HANDLE hIn = hStdIn;

  if (!OpenClipboard(NULL))
    {
      win_perror("Cannot open clipboard");
      return 1;
    }

  dwFileSizeHigh = 0;
  dwFileSize = GetFileSize(hStdIn, &dwFileSizeHigh);
  if (dwFileSizeHigh != 0)
    {
      fputs("Too big input file\r\n", stderr);
      return 2;
    }
  if (dwFileSize == 0)
    {
      fputs("Input file is either empty or unreadable.\r\n", stderr);
      return 2;
    }
  if (dwFileSize > (DWORD)0x7FFFFFFD)
    {
      fputs("Either the input file is too big or is not a seekable file object.\r\n", stderr);
      return 2;
    }

  pStorage = LocalAlloc(LPTR, dwFileSize+1);
  if (!pStorage)
    {
      win_perror("Memory allocation error");
      return 3;
    }

  ptr = pStorage;
  for (; dwFileSize; dwFileSize -= dwSize)
    if (!ReadFile(hIn, ptr, dwFileSize, &dwSize, NULL))
      {
	win_perror("File read error");
	return 2;
      }
    else
      ptr += dwSize;

  if (!EmptyClipboard())
    win_perror("Cannot empty clipboard");

  if (SetClipboardData(CF_OEMTEXT, pStorage) == NULL)
    {
      win_perror("Clipboard error");
      return 1;
    }

  return 0;
}
