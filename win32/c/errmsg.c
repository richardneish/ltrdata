#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <stdlib.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

#pragma comment(lib, "mswsock.lib")

int
main(int argc, char **argv)
{
  LPTSTR errmsg;
  HMODULE hModule;

  if (argc <= 1)
    {
      puts("Syntax:\r\n"
	   "errmsg messagenumber [dllfile]");
      return 1;
    }

  if (argc > 2)
    {
      hModule = LoadLibrary(argv[2]);
      if (FormatMessage(78 |
			FORMAT_MESSAGE_FROM_HMODULE |
			FORMAT_MESSAGE_IGNORE_INSERTS |
			FORMAT_MESSAGE_ALLOCATE_BUFFER, hModule,
			strtoul(argv[1], NULL, 0),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&errmsg, 0, NULL))
	{
	  CharToOem(errmsg, errmsg);
	  puts(errmsg);
	}
      else
	printf("Failed, error %u.\n", GetLastError());
    }
  else
    {
      if (FormatMessage(78 |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS |
			FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL,
			strtoul(argv[1], NULL, 0),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&errmsg, 0, NULL))
	{
	  CharToOem(errmsg, errmsg);
	  puts(errmsg);
	}
      else
	s_perror("", strtoul(argv[1], NULL, 0));
    }

  return 0;
}
