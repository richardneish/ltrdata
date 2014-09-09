#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma comment(lib, "kernel32")

#pragma comment(linker, "/subsystem:console")
#pragma comment(linker, "/entry:main")

unsigned char cBuffer[8192];

int main()
{
  HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD dwReadLen = sizeof cBuffer;
   
  unsigned char *ptr;
  
  while (ReadFile(hIn, cBuffer, dwReadLen, &dwReadLen, NULL))
    {
      if (dwReadLen == 0)
	ExitProcess(0);
      
      for (ptr = cBuffer; ptr < cBuffer + dwReadLen; ptr++)
	if (*ptr >= 'a')
	  {
            if (*ptr <= ('z' - 13))
	      *ptr += 13;
            else if (*ptr <= 'z')
	      *ptr -= 13;
	  }
	else if (*ptr >= 'A')
	  {
            if (*ptr <= ('Z' - 13))
	      *ptr += 13;
            else if (*ptr <= 'Z')
	      *ptr -= 13;
	  }
      
      Sleep(0);
      
      if (!WriteFile(hOut, cBuffer, dwReadLen, &dwReadLen, NULL))
	break;
    }

  ExitProcess(1);
}

