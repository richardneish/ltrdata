#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <winioctl.h>
#include <stdio.h>

#ifdef _DLL
#pragma comment(lib, "minwcrt")
#endif

int main(int argc, char **argv)
{
  struct
  {
    DWORD ReparseTag;
    WORD ReparseDataLength;
    WORD Reserved;
    WORD NameOffset;
    WORD NameLength;
    WORD DisplayNameOffset;
    WORD DisplayNameLength;
    BYTE Data[65536];
  } ReparseData = { 0 };

  if ((argc < 2) | (argc > 3))
    {
      fputs("Syntax to create a junction point:\r\n"
	    "junc DIRECTORY TARGET\r\n"
	    "Where DIRECTORY is an empty directory on an NTFS volume and TARGET is a native\r\n"
	    "path to a target directory, e.g. \"\\??\\C:\\Windows\", \"\\??\\D:\\\" or\r\n"
	    "\"\\Device\\Harddisk0\\Partition1\". (Quotes are optional unless the path\r\n"
	    "contains spaces.)\r\n"
	    "\n"
	    "Syntax to display where a junction point points:\r\n"
	    "junc JUNCTIONPOINT\r\n"
	    "Where JUNCTIONPOINT is a junction point.\r\n", stderr);
      return 5;
    }

  SetFileApisToOEM();

  CharToOem(argv[1], argv[1]);

  HANDLE h;
  if (argc == 2)
    h = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ |
		   FILE_SHARE_DELETE, NULL, OPEN_EXISTING,
		   FILE_FLAG_BACKUP_SEMANTICS |
		   FILE_FLAG_OPEN_REPARSE_POINT, NULL);
  else
    {
      h = CreateFile(argv[1], GENERIC_READ | GENERIC_WRITE,
		     FILE_SHARE_DELETE, NULL, OPEN_EXISTING,
		     FILE_FLAG_BACKUP_SEMANTICS |
		     FILE_FLAG_OPEN_REPARSE_POINT, NULL);
      if ((h == INVALID_HANDLE_VALUE) &&
	  (GetLastError() == ERROR_FILE_NOT_FOUND))
	{
	  CreateDirectory(argv[1], NULL);
	  h = CreateFile(argv[1], GENERIC_READ | GENERIC_WRITE,
			 FILE_SHARE_DELETE, NULL, OPEN_EXISTING,
			 FILE_FLAG_BACKUP_SEMANTICS |
			 FILE_FLAG_OPEN_REPARSE_POINT, NULL);
	}
    }

  if (h == INVALID_HANDLE_VALUE)
    {
      win_perror(argv[1]);
      return 1;
    }

  DWORD dw;
  if (argc > 2)
    {
      WORD wSize =
	(MultiByteToWideChar(CP_ACP, 0, argv[2], -1, NULL, 0)-1) << 1;

      if ((wSize + 6 > sizeof(ReparseData.Data)) | (wSize == 0))
	{
	  fprintf(stderr, "Name is too long: '%ws'\n", argv[2]);
	  return 4;
	}

      ReparseData.ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;
      ReparseData.ReparseDataLength = 8 + wSize + 4;
      ReparseData.NameLength = wSize;
      ReparseData.DisplayNameOffset = (wSize + 2);
      MultiByteToWideChar(CP_ACP, 0, argv[2], -1, (LPWSTR)ReparseData.Data,
			  sizeof(ReparseData.Data) - 6);
      CharToOem(argv[2], argv[2]);

      if (!DeviceIoControl(h, FSCTL_SET_REPARSE_POINT, &ReparseData,
			   8 + 8 + wSize + 4, NULL, 0, &dw, NULL))
	{
	  switch (GetLastError())
	    {
	    case ERROR_INVALID_REPARSE_DATA:
	      fprintf(stderr, "Invalid target path: '%s'\n", argv[2]);
	      return 1;
	    case ERROR_INVALID_PARAMETER:
	      fputs("This OS does not support reparse points.\r\n",
		    stderr);
	      break;
	    case ERROR_INVALID_FUNCTION: case ERROR_NOT_A_REPARSE_POINT:
	      fputs("This filesystem does not support reparse points.\r\n",
		    stderr);
	      break;
	    case ERROR_DIRECTORY: case ERROR_DIR_NOT_EMPTY:
	      fputs("Reparse points can only created on empty "
		    "directories.\r\n", stderr);
	      break;
	    default:
	      WErrMsg errmsg;
	      CharToOem(errmsg, errmsg);
	      fprintf(stderr,
		      "Error joining '%s' to '%s'\n"
		      "%u: %s", argv[2], argv[1], GetLastError(),
		      errmsg);
	    }

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
	      fprintf(stderr, "The reparse data on '%s' is invalid\n",
		      argv[1]);
	      return 1;
	    case ERROR_INVALID_PARAMETER:
	      fputs("This OS does not support reparse points.\r\n",
		    stderr);
	      break;
	    case ERROR_INVALID_FUNCTION:
	      fputs("This filesystem does not support reparse points.\r\n",
		    stderr);
	      break;
	    case ERROR_NOT_A_REPARSE_POINT:
	    case ERROR_DIRECTORY:
	    case ERROR_DIR_NOT_EMPTY:
	      fprintf(stderr, "Not a reparse point: '%s'\n", argv[1]);
	      break;
	    default:
	      WErrMsg errmsg;
	      CharToOem(errmsg, errmsg);
	      fprintf(stderr,
		      "Error getting reparse data from '%s'\n"
		      "%u: %s",
		      argv[2], GetLastError(), errmsg);
	    }

	  return 1;
    }

  if (ReparseData.ReparseTag != IO_REPARSE_TAG_MOUNT_POINT)
    {
      fputs("This reparse point is not a junction.\r\n", stderr);
      return 2;
    }

  printf("%s -> %*ws\n",
	 argv[1],
	 ReparseData.NameLength >> 1,
	 ReparseData.Data + ReparseData.NameOffset);

  return 0;
}
