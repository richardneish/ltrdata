#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

#define BUFFER_EXPONENT   12    // 2 ** 12 = 4096 bytes
#define BUFFER_SIZE       (1 << BUFFER_EXPONENT)

int
main(int argc, char **argv)
{
  if (argc <= 1)
    {
      puts("cxr, cxrbackup and cxrrestore -- By Olof Lagerkvist 2001.\r\n"
	   "http://www.ltr-data.se      olof@ltr-data.se\r\n"
	   "\n"
	   "Usage: cxr file1 [file2 ...]\r\n"
	   "\n"
	   "Calculates redundant xor checksums for each byte position\r\n"
	   "in input files and writes the checksum stream on stdout.\r\n"
	   "An empty string parameter, \"\", as input file means stdin.");
      return -1;
    }
  
  char *data = (char*)LocalAlloc(LMEM_FIXED, argc << BUFFER_EXPONENT);
  if (data == NULL)
    {
      win_perror(argv[0]);
      fputs("Memory allocation error.\r\n", stderr);
      return -1;
    }

  HANDLE *phFiles = (HANDLE*)halloc_seh(sizeof(HANDLE) * argc);

  phFiles[0] = hStdOut;

  for (int c=1; c < argc; c++)
    {
      Sleep(0);

      if( argv[c][0] == 0 )
        phFiles[c] = hStdIn;
      else
        phFiles[c] = CreateFile(argv[c], GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 
				NULL);

      if (phFiles[c] == INVALID_HANDLE_VALUE)
        {
          win_perror(argv[0]);
          fprintf(stderr, "Error opening file %s.\n", argv[c]);
          return -1;
        }
    }

  DWORD dwReadLen = BUFFER_SIZE;
  for(;;)
    {
      for (int c=1; c < argc; c++)
        {
          Sleep(0);

          if (!ReadFile(phFiles[c], data+(c << BUFFER_EXPONENT), dwReadLen,
			&dwReadLen, NULL))
            {
              if (win_errno == ERROR_BROKEN_PIPE)
                return 0;

              win_perror(argv[0]);

              if (argv[c][0])
                fprintf(stderr, "Error reading file %s.\n", argv[c]);
	      else
                fputs("Error reading from stdin.\r\n", stderr);

	      return -1;
	    }
        }

      if (dwReadLen == 0)
        return 0;

      ZeroMemory(data, dwReadLen);
      for (DWORD bytecount=0; bytecount < dwReadLen; bytecount++)
        for (int filecount=1; filecount < argc; filecount++)
          data[bytecount] ^= data[bytecount+(filecount << BUFFER_EXPONENT)];

      DWORD dwWritten;
      if (!WriteFile(phFiles[0], data, dwReadLen, &dwWritten, NULL))
	if (dwWritten != dwReadLen)
	  {
	    win_perror("cxr: Error writing to stdout");
	    return -1;
	  }
    }
}

