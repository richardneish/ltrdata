#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <wtime.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

void printfattr(LPCSTR lpMsg, DWORD dwAttr)
{
  printf("%s", lpMsg);
  if (dwAttr & FILE_ATTRIBUTE_ARCHIVE)
    printf(" Archive");
  if (dwAttr & FILE_ATTRIBUTE_COMPRESSED)
    printf(" Compressed");
  if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
    printf(" Directory");
  if (dwAttr & FILE_ATTRIBUTE_ENCRYPTED)
    printf(" Encrypted");
  if (dwAttr & FILE_ATTRIBUTE_HIDDEN)
    printf(" Hidden");
  if (dwAttr & FILE_ATTRIBUTE_OFFLINE)
    printf(" Off-line");
  if (dwAttr & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
    printf(" Not-content-indexed");
  if (dwAttr & FILE_ATTRIBUTE_READONLY)
    printf(" Read-only");
  if (dwAttr & FILE_ATTRIBUTE_REPARSE_POINT)
    printf(" Reparse-point");
  if (dwAttr & FILE_ATTRIBUTE_SPARSE_FILE)
    printf(" Sparse");
  if (dwAttr & FILE_ATTRIBUTE_SYSTEM)
    printf(" System");
  if (dwAttr & FILE_ATTRIBUTE_TEMPORARY)
    printf(" Temporary");
  if (dwAttr & FILE_ATTRIBUTE_NORMAL)
    printf(" Normal");

  puts("");
}

void printdt(const char *msg, FILETIME ft)
{
  WSystemTime st = ft;

  fputs(msg, stdout);

  char cBuf[80] = "";
  if (GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, cBuf,
		    sizeof cBuf))
    fputs(cBuf, stdout);

  fputc(' ', stdout);

  if (GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, cBuf, sizeof cBuf))
    fputs(cBuf, stdout);

  fputs("\r\n", stdout);
}      

void print_info(HANDLE h)
{
  printf("File type: ");

  switch (GetFileType(h))
    {
    case FILE_TYPE_CHAR:
      puts("Character device.");
      break;
    case FILE_TYPE_DISK:
      puts("Disk file.");
      break;
    case FILE_TYPE_PIPE:
      puts("Socket or pipe.");
      break;
    case FILE_TYPE_REMOTE:
      puts("Remote file.");
      break;
    default:
      puts("Unknown.");
    }

  BY_HANDLE_FILE_INFORMATION info;
  if (GetFileInformationByHandle(h, &info))
    {
      printfattr("File attributes:", info.dwFileAttributes);
      if (*(QWORD*)&info.ftCreationTime)
	printdt("Creation time: ", info.ftCreationTime);
      if (*(QWORD*)&info.ftLastAccessTime)
	printdt("Last access time: ", info.ftLastAccessTime);
      if (*(QWORD*)&info.ftLastWriteTime)
	printdt("Last write time: ", info.ftLastWriteTime);
      if (info.dwVolumeSerialNumber)
	printf("Volume serial number: %.4X-%.4X\n",
	       (DWORD)HIWORD(info.dwVolumeSerialNumber),
	       (DWORD)LOWORD(info.dwVolumeSerialNumber));

      ULARGE_INTEGER FileSize;
      FileSize.HighPart = info.nFileSizeHigh;
      FileSize.LowPart = info.nFileSizeLow;
      printf("File size: %.4g %s\n",
	     TO_h(FileSize.QuadPart), TO_p(FileSize.QuadPart));

      printf("Number of file system links: %i\n", (int)info.nNumberOfLinks);

      ULARGE_INTEGER FileIndex;
      FileIndex.HighPart = info.nFileIndexHigh;
      FileIndex.LowPart = info.nFileIndexLow;
      printf("Sequence: %u\n", HIWORD(info.nFileIndexHigh));
      printf("Number: %.f\n",
	     (double)(FileIndex.QuadPart & 0x0000FFFFFFFFFFFF));
    }
  else
    {
      ULARGE_INTEGER FileSize;
      FileSize.LowPart = GetFileSize(h, &FileSize.HighPart);
      if (FileSize.LowPart == INVALID_FILE_SIZE)
	if (win_errno != ERROR_SUCCESS)
	  {
	    win_perror("No information available");
	    return;
	  }

      printf("File size: %.4g %s\n",
	     TO_h(FileSize.QuadPart), TO_p(FileSize.QuadPart));
    }
}

int main(int argc, char **argv)
{
  if (argc == 1)
    {
      puts("stdin:");
      print_info(hStdIn);
      return 0;
    }

  while (argc > 1)
    {
      DWORD dwFileAttr = GetFileAttributes(argv[1]);
      if (dwFileAttr == INVALID_FILE_ATTRIBUTES)
	dwFileAttr = 0;

      HANDLE h = CreateFile(argv[1], 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
			    OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS |
			    (dwFileAttr & FILE_ATTRIBUTE_REPARSE_POINT ?
			     FILE_FLAG_OPEN_REPARSE_POINT : 0), NULL);

      if (h == INVALID_HANDLE_VALUE)
	win_perror(argv[1]);
      else
	{
	  CharToOem(argv[1], argv[1]);
	  printf("\nFilename: %s\n", argv[1]);
	  print_info(h);
	  CloseHandle(h);
	}
      
      argc--;
      argv++;
    }
}      
