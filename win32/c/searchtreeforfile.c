#include "winstrct.h"

#include <imagehlp.h>

#pragma comment(lib, "imagehlp")

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

char cBuf[MAX_PATH * 33];

int main(int argc, char **argv)
{
  char *pcFileName;
  char *pcPathName;

  if (argc > 1)
    pcFileName = argv[1];
  else
    pcFileName = "*";

  if (argc > 2)
    pcPathName = argv[2];
  else
    pcPathName = ".";

  if (!SearchTreeForFile(pcPathName, pcFileName, cBuf))
    {
      win_perror(pcFileName);
      return 1;
    }
  else
    {
      puts(cBuf);
      return 0;
    }
}
