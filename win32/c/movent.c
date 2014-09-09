#define _WIN32_WINNT 0x0500
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
  DWORD dwFlags = 0;
  BOOL bDisplayHelp = FALSE;

  // Nice argument parse loop :)
  while (argc > 1 ? argv[1][0] ? (argv[1][0] | 0x02) == '/' : FALSE : FALSE)
    {
      while ((++argv[1])[0])
	switch (argv[1][0] | 0x20)
	  {
	  case 'c':
	    dwFlags |= MOVEFILE_COPY_ALLOWED;
	    break;
	  case 'h':
	    dwFlags |= MOVEFILE_CREATE_HARDLINK;
	    break;
	  case 'b':
	    dwFlags |= MOVEFILE_DELAY_UNTIL_REBOOT;
	    break;
	  case 'r':
	    dwFlags |= MOVEFILE_REPLACE_EXISTING;
	    break;
	  case 't':
	    dwFlags |= MOVEFILE_FAIL_IF_NOT_TRACKABLE;
	    break;
	  case 's':
	    dwFlags |= MOVEFILE_WRITE_THROUGH;
	  default:
	    bDisplayHelp = TRUE;
	  }

      --argc;
      ++argv;
    }

  if (bDisplayHelp | (argc < 3))
    {
      puts("Command line access to the MoveFileEx() API function.\r\n"
	   "\n"
	   "Written by Olof Lagerkvist 2002-2005\r\n"
	   "http://www.ltr-data.se      olof@ltr-data.se\r\n"
	   "\n"
	   "Usage:\r\n"
	   "MOVENT [options] sourcefile destfile\r\n"
	   "\n"
	   "-c     If the file is to be moved to a different volume, the move is simulated\r\n"
	   "       by copying and deleting.\r\n"
	   "\n"
	   "       This flag cannot be used with the -b flag.\r\n"
	   "\n"
	   "-h     If moving within the same filesystem, create a new hard link instead of\r\n"
	   "       moving file. This flag requires Windows 2000 or later, it is ignored on\r\n"
	   "       earlier versions of Windows NT.\r\n"
	   "\n"
	   "-b     The file is not moved until the operating system is restarted. The\r\n"
	   "       system moves the file immediately after AUTOCHK is executed, but before\r\n"
	   "       creating any paging files. Consequently, this parameter enables deleting\r\n"
	   "       paging files from previous startups.\r\n"
	   "\n"
	   "       This flag can only be used by a user who belongs to the administrators\r\n"
	   "       group or the LocalSystem account.\r\n"
	   "\n"
	   "       This flag cannot be used with the -c flag.\r\n"
	   "\n"
	   "-t     The move fails if the source file is a link source, but the file cannot\r\n"
	   "       be tracked after the move. This situation can occur if the destination\r\n"
	   "       is a volume formatted with the FAT file system. This flag requires\r\n"
	   "       Windows 2000 or later.\r\n"
	   "\n"
	   "-r     If a file of the name specified by destfile already exists, the system\r\n"
	   "       replaces its contents with those specified by sourcefile.\r\n"
	   "\n"
	   "-s     The program does not exit until the file has actually been moved on the\r\n"
	   "       disk.\r\n"
	   "\n"
	   "       Setting this flag guarantees that a move performed as a copy and delete\r\n"
	   "       operation is flushed to disk before the program exits. The flush occurs\r\n"
	   "       at the end of the copy operation.\r\n"
	   "\n" 
	   "       This flag has no effect if the -b flag is set.");

      return 0;
    }

  if (MoveFileEx(argv[1], argv[2], dwFlags))
    return 0;

  win_perror(NULL);
  return 1;
}

int
wmainCRTStartup()
{
  int argc = 0;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
  if (argv == NULL)
    {
      MessageBoxA(NULL, "This program requires Windows NT.",
		  "Incompatible Windows version", MB_ICONEXCLAMATION);
      ExitProcess((UINT) -1);
    }
  else
    exit(wmain(argc, argv));
}
