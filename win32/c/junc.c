#define UNICODE
#define _UNICODE
#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN
#ifndef _DLL
#define _DLL
#endif
#include <winstrct.h>
#include <winioctl.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "msvcrt.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "shell32.lib")

int
wmain(int argc, LPWSTR *argv)
{
  REPARSE_DATA_MOUNT_POINT ReparseData = { 0 };
  HANDLE h;
  DWORD dw;
  BOOL bDirectoryCreated = FALSE;
  BOOL bRemoval = FALSE;

  SetOemPrintFLineLength(GetStdHandle(STD_ERROR_HANDLE));

  if ((argc < 2) | (argc > 3))
    {
      fputs("Syntax 1 - Create a junction:\r\n"
	    "junc DIRECTORY TARGET\r\n"
	    "\n"
	    "Where DIRECTORY is an empty directory on an NTFS volume and TARGET is a native\r\n"
	    "path to a target directory, e.g. \"\\??\\C:\\Windows\", \"\\??\\D:\\\\\" or\r\n"
	    "\"\\Device\\Harddisk0\\Partition1\\\\\". Double quotes are optional unless the\r\n"
	    "path contains spaces. If the string ends with a backslash and you surround it\r\n"
	    "with quotes, you will need an extra backslash at the end of the string:\r\n"
	    "\\??\\D:\\ is equal to \"\\??\\D:\\\\\".\r\n"
	    "\n"
	    "Syntax 2 - Display where a junction points:\r\n"
	    "junc JUNCTION\r\n"
	    "\n"
	    "Syntax 3 - Remove a junction:\r\n"
	    "junc -r JUNCTION\r\n"
	    "The junction will be converted back to an empty directory.\r\n",
	    stderr);
      return 5;
    }

  if (argc == 3 ? wcscmp(argv[1], L"-r") == 0 : FALSE)
    {
      bRemoval = TRUE;
      argc--;
      argv++;
    }

  if (bRemoval)
    h = CreateFile(argv[1], GENERIC_WRITE, FILE_SHARE_READ, NULL,
		   OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS |
		   FILE_FLAG_OPEN_REPARSE_POINT, NULL);
  else if (argc == 2)
    h = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		   FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
		   NULL);
  else
    {
      h = CreateFile(argv[1], GENERIC_WRITE, FILE_SHARE_READ, NULL,
		     OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS |
		     FILE_FLAG_OPEN_REPARSE_POINT, NULL);
      if ((h == INVALID_HANDLE_VALUE) &&
	  (GetLastError() == ERROR_FILE_NOT_FOUND))
	{
	  if (CreateDirectory(argv[1], NULL))
	    bDirectoryCreated = TRUE;
	  else
	    {
	      win_perror(argv[1]);
	      return 3;
	    }

	  h = CreateFile(argv[1], GENERIC_READ | GENERIC_WRITE, 0, NULL,
			 OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS |
			 FILE_FLAG_OPEN_REPARSE_POINT, NULL);
	}
    }

  if (h == INVALID_HANDLE_VALUE)
    {
      win_perror(argv[1]);
      return 3;
    }

  if (bRemoval)
    {
      ReparseData.ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;
      ReparseData.ReparseDataLength = 0;

      if (!DeviceIoControl(h, FSCTL_DELETE_REPARSE_POINT, &ReparseData,
			   REPARSE_GUID_DATA_BUFFER_HEADER_SIZE, NULL, 0, &dw,
			   NULL))
	{
	  switch (GetLastError())
	    {
	    case ERROR_INVALID_REPARSE_DATA:
	      oem_printf(stderr,
			 "The reparse data on '%1!ws!' is invalid%%n",
			 argv[1]);
	      break;

	    case ERROR_INVALID_PARAMETER:
	      fputs("This OS does not support reparse points.\r\n"
		    "Windows 2000 or higher is required.\r\n",
		    stderr);
	      break;

	    case ERROR_INVALID_FUNCTION:
	      fputs("Reparse points are only supported on NTFS volumes.\r\n",
		    stderr);
	      break;

	    case ERROR_NOT_A_REPARSE_POINT:
	    case ERROR_DIRECTORY:
	    case ERROR_DIR_NOT_EMPTY:
	      oem_printf(stderr,
			 "Not a reparse point: '%1!ws!'%%n", argv[1]);
	      break;

	    default:
	      oem_printf(stderr,
			 "Error getting reparse data from '%1!ws!': %2%%n",
			 argv[1], win_errmsgA(GetLastError()));
	    }

	  return 1;
	}

      return 0;
    }

  if (argc > 2)
    {
      int iSize = (int) wcslen(argv[2]) << 1;

      if ((iSize + 6 > sizeof(ReparseData.Data)) | (iSize == 0))
	{
	  oem_printf(stderr, "Name is too long: '%1!ws!'%%n", argv[2]);
	  return 4;
	}

      ReparseData.ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;
      ReparseData.ReparseDataLength = (WORD) (8 + iSize + 2 + iSize + 2);
      ReparseData.NameLength = (WORD) iSize;
      ReparseData.DisplayNameOffset = (WORD) (iSize + 2);
      ReparseData.DisplayNameLength = (WORD) iSize;
      wcscpy((LPWSTR) ReparseData.Data, argv[2]);
      wcscpy((LPWSTR) (ReparseData.Data + ReparseData.DisplayNameOffset),
	     argv[2]);

      if (!DeviceIoControl(h, FSCTL_SET_REPARSE_POINT, &ReparseData,
			   16 + iSize + 2 + iSize + 2, NULL, 0, &dw, NULL))
	{
	  switch (GetLastError())
	    {
	    case ERROR_INVALID_REPARSE_DATA:
	      oem_printf(stderr, "Invalid target path: '%1!ws!'%%n", argv[2]);
	      break;

	    case ERROR_INVALID_PARAMETER:
	      fputs("This OS does not support reparse points.\r\n"
		    "Windows 2000 or higher is required.\r\n",
		    stderr);
	      break;

	    case ERROR_INVALID_FUNCTION:
	    case ERROR_NOT_A_REPARSE_POINT:
	      fputs("Reparse points are only supported on NTFS volumes.\r\n",
		    stderr);
	      break;

	    case ERROR_DIRECTORY:
	    case ERROR_DIR_NOT_EMPTY:
	      fputs("Reparse points can only created on empty "
		    "directories.\r\n", stderr);
	      break;

	    default:
	      oem_printf(stderr,
			 "Error joining '%1!ws!' to '%2!ws!': %3%%n",
			 argv[2], argv[1], win_errmsgA(GetLastError()));
	    }

	  CloseHandle(h);

	  if (bDirectoryCreated)
	    RemoveDirectory(argv[1]);

	  return 1;
	}

      return 0;
    }

  if (!DeviceIoControl(h, FSCTL_GET_REPARSE_POINT, NULL, 0, &ReparseData,
		       sizeof ReparseData, &dw, NULL))
    {
      switch (GetLastError())
	{
	case ERROR_INVALID_REPARSE_DATA:
	  oem_printf(stderr, "The reparse data on '%1!ws!' is invalid.%%n",
		     argv[1]);
	  return 1;

	case ERROR_INVALID_PARAMETER:
	  fputs("This OS does not support reparse points.\r\n"
		"Windows 2000 or higher is required.\r\n",
		stderr);
	  break;

	case ERROR_INVALID_FUNCTION:
	  fputs("Reparse points are only supported on NTFS volumes.\r\n",
		stderr);
	  break;

	case ERROR_NOT_A_REPARSE_POINT:
	case ERROR_DIRECTORY:
	case ERROR_DIR_NOT_EMPTY:
	  oem_printf(stderr, "Not a reparse point: '%1!ws!'%%n", argv[1]);
	  break;

	default:
	  oem_printf(stderr,
		     "Error getting reparse data from '%1!ws!': %2%%n",
		     argv[1], win_errmsgA(GetLastError()));
	}

      return 1;
    }

  if (ReparseData.ReparseTag != IO_REPARSE_TAG_MOUNT_POINT)
    {
      fputs("This reparse point is not a junction.\r\n", stderr);
      return 2;
    }

  oem_printf(stdout,
	     "%1!ws! -> %2!.*ws!%%n",
	     argv[1],
	     ReparseData.NameLength >> 1,
	     ReparseData.Data + ReparseData.NameOffset);

  return 0;
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
