#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

#include <stdio.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

#define BLOCK_EXPONENT   12    // 2 ** 12 = 4096 bytes
#define BLOCK_SIZE       (1 << BLOCK_EXPONENT)

struct FileRec
{
  char cName[MAX_PATH];
  QWORD ui64Size;
  HANDLE handle;
  FileRec *next;
  static FileRec* first;
  FileRec(): ui64Size(0), handle(INVALID_HANDLE_VALUE), next(NULL) {}
  int countrec() const
    {
      if( next )
        return next->countrec() + 1;
      else
        return 1;
    }
};

FileRec *FileRec::first = NULL;

int
main(int argc, char **argv)
{
  if( argc != 2 )
    {
      puts("cxr, cxrbackup and cxrrestore -- By Olof Lagerkvist 2001.\r\n"
	   "http://www.ltr-data.se      olof@ltr-data.se\r\n"
	   "\n"
	   "Usage: cxrrestore file\r\n"
	   "\n"
	   "Restores a destroyed file using a redundant xor checksum\r\n"
	   "file read from stdin. The checksum file must have been\r\n"
	   "previously created using cxrbackup utility.\r\n"
	   "Note! cxrrestore will need all files used to build the\r\n"
	   "checksum unchanged. If any file has been edited, none of\r\n"
	   "the files can be restored.\r\n"
	   "\n"
	   "Usage: cxrrestore \"\"\r\n"
	   "\n"
	   "Lists files used to build a redundant xor checksum file\r\n"
	   "read from stdin.");
      return -1;
    }

  HANDLE hOutFile = INVALID_HANDLE_VALUE;
  HANDLE hIn = hStdIn;
  FileRec::first = new FileRec;
  unsigned __int64 ui64OutFileSize = 0;
  for (FileRec *frCount = FileRec::first; ;
       frCount = frCount->next = new FileRec)
    {
      Sleep(0);

      DWORD dwBytesRead;
      int c=0;
      while( c < sizeof(frCount->cName) )
        {
          Sleep(0);

          ReadFile(hIn, &(frCount->cName[c]), 1, &dwBytesRead, NULL);
          if( dwBytesRead != 1 )
            {
	      fprintf(stderr, "%s: Checksum file error.\n", argv[0]);
              return -1;
            }

          if( frCount->cName[c] == 0 )
            break;

          ++c;
        }

      if( c >= sizeof(frCount->cName) )
        {
	  fprintf(stderr, "%s: Checksum file format error.\n", argv[0]);
          return -1;
        }

      if( frCount->cName[0] == 0 )
        break;

      ReadFile(hIn, &(frCount->ui64Size), sizeof(frCount->ui64Size),
        &dwBytesRead, NULL);
      if( dwBytesRead != sizeof(frCount->ui64Size) )
        {
	  fprintf(stderr, "%s: Checksum file error.\n", argv[0]);
          return -1;
        }

      // No files should be opened or created in list mode
      if( argv[1][0] == 0 )
        continue;

      if( strcmpi(frCount->cName, argv[1]) == 0 )
        {
          hOutFile = CreateFile(argv[1], GENERIC_WRITE, 0, NULL,
            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,
            NULL);

          if( hOutFile == INVALID_HANDLE_VALUE )
            {
              win_perror(argv[0]);
	      fprintf(stderr, "Error creating %s.\n", argv[1]);
              return -1;
            }

          ui64OutFileSize = frCount->ui64Size;
          frCount->handle = hStdIn;
        }
      else
        {
          frCount->handle = CreateFile(frCount->cName, GENERIC_READ,
            FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN,
            NULL);

          if( frCount->handle == INVALID_HANDLE_VALUE )
            {
              win_perror(argv[0]);
	      fprintf(stderr, "Error opening file %s.\n", frCount->cName);
              return -1;
            }

          unsigned __int64 ui64NewSize;
          *(DWORD*)&ui64NewSize = GetFileSize(frCount->handle,
            ((DWORD*)&ui64NewSize)+1);

          if( ui64NewSize != frCount->ui64Size )
            {
	      fprintf(stderr, "%s: Fatal error: File %s size is different.\n",
		      argv[0], frCount->cName);
              return -1;
            }
        }
    }

  // Ta bort sista FileRec-objektet, då det pekar på nulltermineringen i filen
  for( FileRec *fr=FileRec::first; fr; fr=fr->next )
    if( fr->next )
      if( fr->next->next == NULL )
        {
          delete fr->next;
          fr->next = NULL;
        }
  const int filerecn = FileRec::first->countrec();

  if( argv[1][0] == 0 )
    {
      unsigned __int64 ui64TotalSize = 0; 
      for( FileRec *frCount = FileRec::first; frCount; frCount = frCount->next )
        {
	  printf("%s, %.4g %s\n",
		 frCount->cName,
		 TO_h(frCount->ui64Size),
		 TO_p(frCount->ui64Size));

          ui64TotalSize += frCount->ui64Size;
        }

      printf("\nTotal %.f file(s), %.4g %s\n",
	     (double)filerecn,
	     TO_h(ui64TotalSize),
	     TO_p(ui64TotalSize));

      return 0;
    }

  if( hOutFile == INVALID_HANDLE_VALUE )
    {
      fprintf(stderr, "%s: No information about %s in this checksum.\n",
	      argv[0], argv[1]);
      return -1;
    }

  const DWORD dwBufferSize = filerecn << BLOCK_EXPONENT;
  char *cBuffer = (char*)LocalAlloc(LMEM_FIXED, dwBufferSize);
  if( cBuffer == NULL )
    {
      win_perror(argv[0]);
      fputs("Memory allocation error.\r\n", stderr);
      return -1;
    }

  for( unsigned __int64 ui64BytesWritten = 0; ui64BytesWritten < ui64OutFileSize; )
    {
      Sleep(0);

      ZeroMemory(cBuffer, dwBufferSize);
      int c=1;
      for( FileRec *frCount = FileRec::first; frCount; frCount = frCount->next )
        {
          Sleep(0);

          DWORD dwReadLen;
          if( !ReadFile(frCount->handle, cBuffer+((c++) << BLOCK_EXPONENT),
            BLOCK_SIZE, &dwReadLen, NULL) )
            {
              win_perror(argv[0]);
	      fprintf(stderr, "Error reading file %s.\n", frCount->cName);
              return -1;
            }
        }

      for( DWORD bytecount=0; bytecount < BLOCK_SIZE; bytecount++ )
        for( int filecount=1; filecount < filerecn; filecount++ )
          cBuffer[bytecount] ^= cBuffer[bytecount+(filecount << BLOCK_EXPONENT)];

      DWORD dwWritten;
      if( !WriteFile(hOutFile, cBuffer, BLOCK_SIZE, &dwWritten, NULL) )
      if( dwWritten != BLOCK_SIZE )
        {
          win_perror(argv[0]);
	  fprintf(stderr, "Error writing to %s.\n", argv[1]);
          return -1;
        }

      ui64BytesWritten += dwWritten;
    }

    unsigned __int64 ui64NewSize = ui64OutFileSize;
    *(DWORD*)&ui64NewSize = SetFilePointer(hOutFile, (DWORD)ui64NewSize,
      ((LONG*)&ui64NewSize)+1, FILE_BEGIN);

    if( ui64NewSize != ui64OutFileSize )
      {
        win_perror(argv[0]);
        fprintf(stderr, "Seek error in file %s.\n", argv[1]);
        return -1;
      }

    if( !SetEndOfFile(hOutFile) )
      {
        win_perror(argv[0]);
	fprintf(stderr, "Size adjust error in file %s.\n", argv[1]);
        return -1;
      }
}

