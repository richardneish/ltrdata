#define _WIN32_WINNT 0x0500

#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <winioctl.h>
#include <stdlib.h>

/*
#ifdef _DLL
#pragma comment(lib, "minwcrt.lib")
#endif
*/

int
main(int argc, char **argv)
{
  LARGE_INTEGER FileSize;
  HANDLE hFile;
  char cSuffix;
  BOOL bSetSparse = FALSE;
  DWORD dw;

  if (argc == 4)
    if (strcmpi(argv[1], "-s") == 0)
      {
	bSetSparse = TRUE;
	argc--;
	argv++;
      }

  if (argc != 3)
    {
      puts("CHSIZE32.EXE - freeware by Olof Lagerkvist.\r\n"
	   "http://www.ltr-data.se      olof@ltr-data.se\r\n"
	   "Utility to change size of an existing file, or create a new file with specified"
	   "size.\r\n"
	   "\n"
	   "Syntax:\r\n"
	   "\n"
	   "CHSIZE32 [-s] file size[K|M|G|T]\r\n"
	   "\n"
	   "-s      Set sparse attribute.");
      return 0;
    }
  
  switch (sscanf(argv[2], "%I64i%c", &FileSize, &cSuffix))
    {
    case 2:
      switch (cSuffix)
	{
	case 0:
	  break;
	case 'T': case 't':
	  FileSize.QuadPart <<= 10;
	case 'G': case 'g':
	  FileSize.QuadPart <<= 10;
	case 'M': case 'm':
	  FileSize.QuadPart <<= 10;
	case 'K': case 'k':
	  FileSize.QuadPart <<= 10;
	  break;
	default:
	  fprintf(stderr, "Unknown size extension: %c\n", cSuffix);
	  return 1;
	}

    case 1:
      break;

    default:
      fprintf(stderr, "Expected file size, got \"%s\"\n", argv[2]);
      return 1;
    }

  hFile = CreateFile(argv[1], GENERIC_READ | GENERIC_WRITE,
		     FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
		     FILE_ATTRIBUTE_NORMAL, NULL);
  
  if (hFile == INVALID_HANDLE_VALUE)
    {
      win_perror(argv[1]);
      return 1;
    }

  if (bSetSparse)
    if (!DeviceIoControl(hFile, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &dw, NULL))
      win_perror("Error setting sparse attribute");

  if ((SetFilePointer(hFile, FileSize.LowPart, &FileSize.HighPart,
		      FILE_BEGIN) == 0xFFFFFFFF) ?
      (GetLastError() != NO_ERROR) : FALSE)
    {
      win_perror(NULL);
      return 1;
    }

  if (SetEndOfFile(hFile))
    return 0;

  win_perror(NULL);
  return 1;
}
