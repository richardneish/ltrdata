#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <winstrct.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "nullcrt")
#endif

int main()
{
  char *lpData;

  if (!OpenClipboard(NULL))
    {
      win_perror("Cannot open clipboard");
      return 1;
    }

  lpData = (char*)GetClipboardData(CF_OEMTEXT);
  if (lpData == NULL)
    {
      if (win_errno == ERROR_PROC_NOT_FOUND)
	fputs("Clipboard data is not text.\r\n", stderr);
      else
	win_perror("Unable to read data from clipboard");
      return 2;
    }

  fputs(lpData, stdout);

  return 0;
}

