#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <stdlib.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

#ifndef BUFSIZE
#define BUFSIZE 2048
#endif

__declspec(noreturn)
void
usage()
{
  fputs("Text codepage conversion tool, freeware by Olof Lagerkvist.\r\n"
	"http://www.ltr-data.se             olof@ltr-data.se\r\n"
	"\n"
	"To convert files in-place from OEM to ANSI:\r\n"
	"txtcnv32 -o [filename] ...\r\n"
	"To convert files in-place from ANSI to OEM:\r\n"
	"txtcnv32 -a [filename] ...\r\n"
	"Without filenames, a text stream read from stdin is converted and written to\r\n"
	"stdout.\r\n"
	"\n"
	"To convert stdin to stdout using specific codepage conversions:\r\n"
	"txtcnv32 [-s] [-d:fromcodepage] [-e:tocodepage]\r\n"
	"If you do not specify 'fromcodepage', or if you specify codepage zero, the text\r\n"
	"to be converted read from stdin is assumed to be 16-bit Unicode and if you do\r\n"
	"not specify 'tocodepage', or specify codepage zero, 16-bit Unicode is written\r\n"
	"to stdout.\r\n"
	"\n"
	"The -s switch is used to 'byte-swap' unicode input/output, that is convert\r\n"
	"between little endian and big endian Unicode.\r\n",
	stderr);

  exit(1);
}

int
main(int argc, char **argv)
{
  enum
    {
      NoOperation,
      OEMToChar,
      CharToOEM,
      Codec
    } conv = NoOperation;

  UINT DecodePage = 0;
  UINT EncodePage = 0;

  bool byteswap = false;

  if (argc < 2)
    usage();

  while (argc > 1 ? argv[1][0] ? ((argv[1][0] | 0x02) == '/') &
	 ~((strcmp(argv[1], "-") == 0) | (strcmp(argv[1], "--") == 0)) :
	 false : false)
    {
      while ((++argv[1])[0])
	switch (argv[1][0] | 0x20)
	  {
	  case 'a':
	    if (conv != NoOperation)
	      usage();
	    conv = CharToOEM;
	    continue;

	  case 'o':
	    if (conv != NoOperation)
	      usage();
	    conv = OEMToChar;
	    continue;

	  case 'd':
	    if ((conv != NoOperation) & (conv != Codec))
	      usage();
	    conv = Codec;
	    if (argv[1][1] != ':')
	      usage();
	    DecodePage = strtoul(argv[1] + 2, NULL, 0);
	    argv[1] += strlen(argv[1]) - 1;
	    continue;

	  case 's':
	    byteswap = true;
	    break;

	  case 'e':
	    if ((conv != NoOperation) & (conv != Codec))
	      usage();
	    conv = Codec;
	    if (argv[1][1] != ':')
	      usage();
	    EncodePage = strtoul(argv[1] + 2, NULL, 0);
	    argv[1] += strlen(argv[1]) - 1;
	    continue;
	  }
      ++argv;
      --argc;

      if (argc == 1)
	break;
    }

  if (conv == NoOperation)
    usage();

  if (argc > 1)
    {
      if ((conv != OEMToChar) & (conv != CharToOEM))
	{
	  fputs
	    ("You can only specify filenames with the -a or -o switches.\r\n",
	     stderr);
	  return 1;
	}

      while (argc-- > 1)
	{
	  argv++;

	  HANDLE hFile =
	    CreateFile(argv[0], GENERIC_READ | GENERIC_WRITE, 0, NULL,
		       OPEN_EXISTING, 0, NULL);
	  if (hFile == INVALID_HANDLE_VALUE)
	    {
	      win_perror(argv[0]);
	      continue;
	    }

	  ULARGE_INTEGER FileSize;
	  FileSize.LowPart = GetFileSize(hFile, &FileSize.HighPart);

	  if (FileSize.LowPart == INVALID_FILE_SIZE)
	    if (win_errno != NO_ERROR)
	      {
		win_perror(argv[0]);
		CloseHandle(hFile);
		continue;
	      }

	  HANDLE hMap =
	    CreateFileMapping(hFile, NULL,
			      PAGE_READWRITE | SEC_COMMIT, 0, 0,
			      NULL);
	  if (hMap == NULL)
	    {
	      win_perror(argv[0]);
	      CloseHandle(hFile);
	      continue;
	    }

	  CloseHandle(hFile);

	  char *buf = (char *) MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, 0);
	  if (buf == NULL)
	    {
	      win_perror(argv[0]);
	      CloseHandle(hMap);
	      return -1;
	    }

	  CloseHandle(hMap);

	  PCHAR Ptr = buf;
	  ULONGLONG LeftToWrite = FileSize.QuadPart;
	  for ( ;
	       LeftToWrite & 0xFFFFFFFFC0000000;
	       LeftToWrite -= 0x40000000,
	       Ptr += 0x40000000)
	    if (conv == OEMToChar)
	      OemToCharBuff(Ptr, Ptr, 0x40000000);
	    else
	      CharToOemBuff(Ptr, Ptr, 0x40000000);

	  if (conv == OEMToChar)
	    OemToCharBuff(Ptr, Ptr, (DWORD) LeftToWrite);
	  else
	    CharToOemBuff(Ptr, Ptr, (DWORD) LeftToWrite);

	  UnmapViewOfFile(buf);
	}

      return 0;
    }

  char buf[BUFSIZE << 1];
  WCHAR tmpbuf[BUFSIZE];
  HANDLE hIn = hStdIn;
  HANDLE hOut = hStdOut;

  for (;;)
    {
      DWORD sizeread;
      if (!ReadFile(hIn, buf, sizeof(buf) >> 1, &sizeread, NULL))
	{
	  if (win_errno == ERROR_BROKEN_PIPE)
	    break;

	  win_perror("stdin");
	  return -1;
	}

      if (sizeread == 0)
	break;

      if (conv == OEMToChar)
	OemToCharBuff(buf, buf, sizeread);
      else if (conv == CharToOEM)
	CharToOemBuff(buf, buf, sizeread);
      else
	{
	  if (DecodePage != 0)
	    {
	      sizeread = MultiByteToWideChar(DecodePage, 0,
					     buf, sizeread,
					     tmpbuf,
					     sizeof(tmpbuf) / sizeof(*tmpbuf));
	      if (sizeread == 0)
		{
		  win_perror("Conversion error");
		  return -1;
		}
	    }
	  else
	    {
	      CopyMemory(tmpbuf, buf, sizeread);
	      sizeread >>= 1;
	    }

	  if (byteswap)
	    _swab((char*)tmpbuf, (char*)tmpbuf, sizeread << 1);

	  if (EncodePage != 0)
	    {
	      sizeread = WideCharToMultiByte(EncodePage, 0,
					     tmpbuf, sizeread,
					     buf, sizeof buf,
					     NULL, NULL);
	      if (sizeread == 0)
		{
		  win_perror("Conversion error");
		  return -1;
		}
	    }
	  else
	    CopyMemory(buf, tmpbuf, sizeread <<= 1);
	}

      DWORD sizewritten;
      if (!WriteFile(hOut, buf, sizeread, &sizewritten, NULL))
	{
	  win_perror("stdout");
	  return -1;
	}

      if (sizewritten != sizeread)
	fprintf(stderr, "Warning: Lost %u bytes.\n",
		(DWORD) (sizeread - sizewritten));

      Sleep(0);		// Let other threads run
    }

  return 0;
}
