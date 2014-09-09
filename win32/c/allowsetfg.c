#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <stdlib.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

int
main(int argc, char **argv)
{
  DWORD_PTR dwOld = 0;
  DWORD_PTR dwNew = 0;

  if (SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT,
			   0,
			   &dwOld,
			   0))
    printf("Current timeout value is %u ms\n", dwOld);

  if (argc == 1)
    return 0;

  dwNew = strtoul(argv[1], NULL, 0);

  if (!SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT,
			    0,
			    (LPVOID) dwNew,
			    SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE))
    {
      win_perror("Failed");
      return 1;
    }
  else
    {
      printf("Timeout value successfully set to %u ms\n", dwNew);
      return 0;
    }
}
