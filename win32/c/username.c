#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <stdio.h>

#pragma comment(lib, "mpr")

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

int main(int argc, char **argv)
{
  char cUserName[MAX_PATH];
  DWORD dwBufSize = sizeof(cUserName);
  
  if (WNetGetUser(argv[argc-1], cUserName, &dwBufSize) != NO_ERROR)
    win_perror(NULL);
  else
    {
      if (argc > 2)
      	fputs(argv[1], stdout);
      
      puts(CharLower(cUserName));
    }

  return 0;
}
