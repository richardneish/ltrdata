#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

#pragma comment(linker, "/subsystem:console")
#pragma comment(linker, "/entry:main")

int main()
{
  UCHAR cBuf;
  DWORD dwReadLen;
  HANDLE hIn = hStdIn;
  HANDLE hOut = hStdOut;

  for(;;)
    {
      do
	{
	  if (!ReadFile(hIn, &cBuf, 1, &dwReadLen, NULL))
            ExitProcess(1);

	  if (dwReadLen == 0)
            return 0;
	} while (cBuf < 0x20);

      do
	{
	  if (!WriteFile(hOut, &cBuf, 1, &dwReadLen, NULL))
            ExitProcess(1);

	  if (dwReadLen == 0)
            ExitProcess(1);

	  if (!ReadFile(hIn, &cBuf, 1, &dwReadLen, NULL))
            ExitProcess(1);

	  if (dwReadLen == 0)
            ExitProcess(0);
	} while (cBuf >= 0x20);

      Sleep(0);
      WriteFile(hOut, "\r\n", 2, &dwReadLen, NULL);
   }
}

