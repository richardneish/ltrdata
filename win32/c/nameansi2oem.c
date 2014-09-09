#define UNICODE
#define _UNICODE
#ifndef _DLL
#define _DLL
#endif
#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <shellapi.h>
#include <stdlib.h>

#pragma comment(lib, "shell32.lib")
#ifndef _WIN64
#pragma comment(lib, "crtdll.lib")
#endif

int
wmain(int argc, LPWSTR *argv)
{
  LPSTR oldname;
  LPWSTR newname;
  int strlength;

  while (--argc > 0)
    {
      ++argv;

      strlength = (int) wcslen(argv[0])+1;

      oldname = halloc_seh(strlength*sizeof(*oldname));
      WideCharToMultiByte(CP_ACP, 0, argv[0], -1, oldname, strlength,
			  NULL, NULL);
      newname = halloc_seh(strlength*sizeof(*newname));
      MultiByteToWideChar(CP_OEMCP, 0, oldname, -1, newname, strlength);

      if (MoveFile(argv[0], newname))
	printf("'%ws' -> '%ws', OK.\n", argv[0], newname);
      else
	win_perror(newname);

      hfree(oldname);
      hfree(newname);
    }

  return 0;
}

int
wmainCRTStartup()
{
  int argc = 0;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
  if (argv == NULL)
    {
      fputs("This program requires Windows NT.\r\n", stderr);
      exit(-1);
    }
  else
    exit(wmain(argc, argv));
}
