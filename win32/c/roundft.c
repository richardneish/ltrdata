#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _DLL
#define _DLL
#endif

#include <winstrct.h>

#ifndef _WIN64
#pragma comment(lib, "minwcrt.lib")
#endif

int
main(int argc, char **argv)
{
  SetOemPrintFLineLength(GetStdHandle(STD_ERROR_HANDLE));

  while (argc-- > 1)
    {
      union
      {
	FILETIME FileTime;
	DWORDLONG QuadPart;
      } CreationTime, LastAccessTime, LastWriteTime;

      HANDLE hFile;

      hFile = CreateFile((++argv)[0],
			 FILE_READ_ATTRIBUTES |
			 FILE_WRITE_ATTRIBUTES,
			 FILE_SHARE_READ |
			 FILE_SHARE_WRITE,
			 NULL,
			 OPEN_EXISTING,
			 FILE_FLAG_BACKUP_SEMANTICS,
			 NULL);

      if (hFile == INVALID_HANDLE_VALUE)
	{
	  LPSTR errmsg = win_error;
	  oem_printf(stderr, "Open failed on '%1': %2%%n", argv[0], errmsg);
	  LocalFree(errmsg);
	  continue;
	}

      if (!GetFileTime(hFile,
		       &CreationTime.FileTime,
		       &LastAccessTime.FileTime,
		       &LastWriteTime.FileTime))
	{
	  LPSTR errmsg = win_error;
	  CloseHandle(hFile);
	  oem_printf(stderr,
		     "Query time failed on '%1': %2%%n", argv[0], errmsg);
	  LocalFree(errmsg);
	  continue;
	}

      CreationTime.QuadPart += 9999999;
      CreationTime.QuadPart /= 10000000;
      CreationTime.QuadPart *= 10000000;

      LastAccessTime.QuadPart += 9999999;
      LastAccessTime.QuadPart /= 10000000;
      LastAccessTime.QuadPart *= 10000000;

      LastWriteTime.QuadPart += 9999999;
      LastWriteTime.QuadPart /= 10000000;
      LastWriteTime.QuadPart *= 10000000;

      if (!SetFileTime(hFile,
		       &CreationTime.FileTime,
		       &LastAccessTime.FileTime,
		       &LastWriteTime.FileTime))
	{
	  LPSTR errmsg = win_error;
	  oem_printf(stderr,
		     "Set time failed on '%1': %2%%n", argv[0], errmsg);
	  LocalFree(errmsg);
	}

      CloseHandle(hFile);
    }

  return 0;
}
