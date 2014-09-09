#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

int
main(int argc, char **argv)
{
  HANDLE hIn;
  HANDLE hOut;
  BY_HANDLE_FILE_INFORMATION by_handle_file_information;

  if (argc > 1)
    {
      hIn = CreateFile(argv[1],
		       FILE_READ_ATTRIBUTES,
		       FILE_SHARE_READ |
		       FILE_SHARE_WRITE,
		       NULL,
		       OPEN_EXISTING, 
		       FILE_FLAG_BACKUP_SEMANTICS,
		       NULL);

      if (hIn == INVALID_HANDLE_VALUE)
	{
	  win_perror(argv[1]);
	  return 1;
	}
    }
  else
    hIn = GetStdHandle(STD_INPUT_HANDLE);

  if (!GetFileInformationByHandle(hIn, &by_handle_file_information))
    {
      win_perror(argc > 1 ? argv[1] : "stdin");
      return 1;
    }

  if (argc <= 2)
    if (!SetFileTime(GetStdHandle(STD_OUTPUT_HANDLE),
		     &by_handle_file_information.ftCreationTime,
		     &by_handle_file_information.ftLastAccessTime,
		     &by_handle_file_information.ftLastWriteTime))
      {
	win_perror("stdout");
	return 2;
      }
    else
      return 0;

  CloseHandle(hIn);
  while (argc-- > 2)
    {
      hOut = CreateFile((++argv)[1],
			FILE_WRITE_ATTRIBUTES,
			FILE_SHARE_READ |
			FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			NULL);

      if (hOut == INVALID_HANDLE_VALUE)
	{
	  win_perror(argv[1]);
	  continue;
	}

      if (!SetFileTime(hOut,
		       &by_handle_file_information.ftCreationTime,
		       &by_handle_file_information.ftLastAccessTime,
		       &by_handle_file_information.ftLastWriteTime))
	{
	  win_perror(argv[1]);
	  continue;
	}

      CloseHandle(hOut);
      if (!SetFileAttributes(argv[1],
			     by_handle_file_information.dwFileAttributes))
	{
	  win_perror(argv[1]);
	  continue;
	}
    }

  return 0;
}
