#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

#define BLOCK_EXPONENT   12    // 2 ** 12 = 4096 bytes
#define BLOCK_SIZE       (1 << BLOCK_EXPONENT)

int
main(int argc, char **argv)
{
  if( argc <= 1 )
    {
      puts("cxr, cxrbackup and cxrrestore -- By Olof Lagerkvist 2001.\r\n"
	   "http://www.ltr-data.se    olof@ltr-data.se\r\n"
	   "\n"
	   "Usage: cxrbackup file1 [file2 ...]\r\n"
	   "\n"
	   "Produces a redundant xor checksum file to stdout for the\r\n"
	   "data in the input files given. Each input file can be\r\n"
	   "restored using the cxrrestore utility with the checksum\r\n"
	   "file as input. Note! cxrrestore will need all files used to\r\n"
	   "build the checksum unchanged. If any file has been edited,\r\n"
	   "none of the files can be restored.");
      return -1;
    }

  const DWORD dwBufferSize = argc << BLOCK_EXPONENT;
  char *cBuffer = (char*)LocalAlloc(LMEM_FIXED, dwBufferSize);
  if (cBuffer == NULL)
    {
      win_perror(argv[0]);
      fputs("Memory allocation error.\r\n", stderr);
      return -1;
    }

  HANDLE *phFiles = (HANDLE*)halloc_seh(sizeof(HANDLE) * argc);

  DWORD dwBytesWritten;

  phFiles[0] = hStdOut;

  for (int c=1; c < argc; c++)
    {
      Sleep(0);
      
      phFiles[c] = CreateFile(argv[c], GENERIC_READ, FILE_SHARE_READ, NULL,
			      OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

      if (phFiles[c] == INVALID_HANDLE_VALUE)
        {
          win_perror(argv[0]);
          fprintf(stderr, "Error opening file %s.\n", argv[c]);
          return -1;
        }

      WriteFile(phFiles[0], argv[c], (DWORD) (strlen(argv[c])+1),
		&dwBytesWritten, NULL);
      if (dwBytesWritten != strlen(argv[c])+1)
        {
          win_perror("cxrbackup: Error writing to stdout");
          return -1;
        }

      unsigned __int64 ui64FileSize = 0;
      *(DWORD*)&ui64FileSize = GetFileSize(phFiles[c],
					   ((DWORD*)&ui64FileSize)+1);
      if (win_errno != NO_ERROR)
        {
          win_perror(argv[0]);
          fprintf(stderr, "Can't get file size for %s.\n", argv[c]);
          return -1;
        }

      WriteFile(phFiles[0], &ui64FileSize, sizeof(ui64FileSize),
		&dwBytesWritten, NULL);
      if (dwBytesWritten != sizeof ui64FileSize)
        {
          win_perror("cxrbackup: Error writing to stdout");
          return -1;
        }
    }

  WriteFile(phFiles[0], "", 1, &dwBytesWritten, NULL);
  if (dwBytesWritten != 1)
    {
      win_perror("cxrbackup: Error writing to stdout");
      return -1;
    }

  for(;;)
    {
      Sleep(0);
      
      bool moredata = false;
      ZeroMemory(cBuffer, dwBufferSize);
      for (int c=1; c < argc; c++)
        {
          Sleep(0);

          DWORD dwReadLen;
          if (!ReadFile(phFiles[c], cBuffer+(c << BLOCK_EXPONENT), BLOCK_SIZE,
			&dwReadLen, NULL))
            {
              win_perror(argv[0]);
              fprintf(stderr, "Error reading file %s.\n", argv[c]);
              return -1;
            }

          moredata |= dwReadLen != 0;
        }

      if (!moredata)
        return 0;

      for (DWORD bytecount=0; bytecount < BLOCK_SIZE; bytecount++)
        for (int filecount=1; filecount < argc; filecount++)
          cBuffer[bytecount] ^= 
	    cBuffer[bytecount+(filecount << BLOCK_EXPONENT)];

      DWORD dwWritten;
      if (!WriteFile(phFiles[0], cBuffer, BLOCK_SIZE, &dwWritten, NULL))
	if (dwWritten != BLOCK_SIZE)
	  {
	    win_perror("cxrbackup: Error writing to stdout");
	    return -1;
	  }
    }
}

