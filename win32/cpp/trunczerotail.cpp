#define WIN32_LEAN_AND_MEAN
#include <stdlib.h>
#include <winstrct.h>
#include <winioctl.h>

#define STDBUFSIZ	512
#define BIGBUFSIZ	(512 << 10)

char buffer[STDBUFSIZ];
DWORD bufsiz = STDBUFSIZ;

HANDLE hOut = INVALID_HANDLE_VALUE;

int
main(int argc, char **argv)
{
  bool bBigBufferMode = false;	// -m switch
  bool bDisplayHelp = false;
  bool bIgnoreErrors = true;

  // Nice argument parse loop :)
  while (argv[1] ? argv[1][0] ? (argv[1][0] | 0x02) == '/' : false : false)
    {
      while ((++argv[1])[0])
	switch (argv[1][0])
	  {
	  case 'm':
	    bBigBufferMode = true;
	    if (argv[1][1] == ':')
	      {
		char *szSizeSuffix = NULL;
		bufsiz = strtoul(argv[1] + 2, &szSizeSuffix, 0);
		if (szSizeSuffix == argv[1] + 2)
		  {
		    fprintf(stderr,
			    "Invalid buffer size: %s\r\n", szSizeSuffix);
		    return -1;
		  }

		if (szSizeSuffix[0] ? szSizeSuffix[1] != 0 : false)
		  {
		    fprintf(stderr,
			    "Invalid buffer size: %s\r\n", argv[1] + 2);
		    return -1;
		  }

		switch (szSizeSuffix[0])
		  {
		  case 'G':
		    bufsiz <<= 10;
		  case 'M':
		    bufsiz <<= 10;
		  case 'K':
		    bufsiz <<= 10;
		    break;
		  case 'g':
		    bufsiz *= 1000;
		  case 'm':
		    bufsiz *= 1000;
		  case 'k':
		    bufsiz *= 1000;
		  case 0:
		    break;
		  default:
		    fprintf(stderr,
			    "Invalid size suffix: %c\r\n", szSizeSuffix[0]);
		    return -1;
		  }

		argv[1] += strlen(argv[1]) - 1;
	      }
	    break;
	  default:
	    bDisplayHelp = true;
	  }

      --argc;
      ++argv;
    }

  DWORD bufsizbits = 0;
  for (DWORD rembufsiz = (bufsiz & ~0x1FF) | 0x200;
       rembufsiz > 1;
       rembufsiz >>= 1)
    bufsizbits++;

  bufsiz = 1 << bufsizbits;
  DWORD bufsizmask = bufsiz - 1;

  if (bIgnoreErrors)
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

  if (bDisplayHelp)
    {
      fputs("File truncation program. Truncates file where last non-zero data is located.\r\n"
	    "\n"
	    "Version 1.0.0. Copyright (C) Olof Lagerkvist 2011.\r\n"
	    "This program is open source freeware.\r\n"
	    "http://www.ltr-data.se      olof@ltr-data.se\r\n"
	    "\n"
	    "Usage:\r\n"
	    "trunczerotail [-m:bufsize] [file]\r\n"
	    "\n"
	    "Default file if none or blank given is standard output.\r\n"
	    "\n"
	    "-m     Buffer size to use while scanning.\r\n", stderr);

      return -1;
    }

  char *bufptr = NULL;

  // If -m option is used we use big buffer
  if (bBigBufferMode)
    {
      bufptr = (char *)LocalAlloc(LPTR, bufsiz);
      if (bufptr == NULL)
	{
	  bufptr = buffer;
	  win_perror("Memory allocation error, "
		     "using 512 bytes buffer instead%%nError message");
	}

      fprintf(stderr, "Buffering %u bytes.\n", bufsiz);
    }
  // -m is not used
  else
    {
      bufptr = buffer;
      fprintf(stderr,
	      "Buffer size is %u bytes.\n", bufsiz);
    }

  if (argc > 1 ? argv[1][0] : false)
    hOut = CreateFile(argv[1],
		      GENERIC_READ | GENERIC_WRITE,
		      FILE_SHARE_READ | FILE_SHARE_WRITE,
		      NULL,
		      OPEN_EXISTING,
		      FILE_ATTRIBUTE_NORMAL,
		      NULL);
  else
    hOut = hStdOut;

  if (hOut == INVALID_HANDLE_VALUE)
    {
      win_perror(argc > 1 ? argv[1] : "stdout");
      return -1;
    }

  LARGE_INTEGER existing_out_file_size = { 0 };
  BY_HANDLE_FILE_INFORMATION ByHandleFileInfo;
  if (!GetFileInformationByHandle(hOut, &ByHandleFileInfo))
    {
      win_perror("Unsupported type of file");
      return -1;
    }

  existing_out_file_size.LowPart = ByHandleFileInfo.nFileSizeLow;
  existing_out_file_size.HighPart = ByHandleFileInfo.nFileSizeHigh;

  LONGLONG last_block_bytes = existing_out_file_size.QuadPart & bufsizmask;

  LARGE_INTEGER current_out_pointer = { 0 };
  if (last_block_bytes != 0)
    current_out_pointer.QuadPart = bufsiz - last_block_bytes;
  current_out_pointer.LowPart =
    SetFilePointer(hOut, current_out_pointer.LowPart,
		   &current_out_pointer.HighPart, FILE_END);
  if (current_out_pointer.LowPart == INVALID_SET_FILE_POINTER)
    if (win_errno != NO_ERROR)
      {
	win_perror("Fatal, cannot set output file pointer");
	return -1;
      }

  for (;;)
    {
      Sleep(0);

      if (current_out_pointer.QuadPart < bufsiz)
	break;

      current_out_pointer.QuadPart = -(LONGLONG)bufsiz;

      current_out_pointer.LowPart =
	SetFilePointer(hOut, current_out_pointer.LowPart,
		       &current_out_pointer.HighPart, FILE_CURRENT);
      if (current_out_pointer.LowPart == INVALID_SET_FILE_POINTER)
	if (win_errno != NO_ERROR)
	  {
	    win_perror("Fatal, cannot set output file pointer");
	    return -1;
	  }

      fprintf(stderr, "Reading at %I64u bytes...\r", current_out_pointer);

      DWORD dwReadSize;
      if (!ReadFile(hOut, bufptr, bufsiz, &dwReadSize, NULL))
	{
	  win_perror("Fatal, cannot read from file");
	  return -1;
	}

      if (dwReadSize == 0)
	{
	  fputs("Fatal, read failed.\r\n", stderr);
	  return -1;
	}

      bool bEndOfDataFound = false;
      for (PULONGLONG bufptr2 = (PULONGLONG) bufptr;
	   bufptr2 < (PULONGLONG) (bufptr + dwReadSize);
	   bufptr2++)
	if (*bufptr2 != 0)
	  {
	    bEndOfDataFound = true;
	    break;
	  }

      if (bEndOfDataFound)
	break;

      current_out_pointer.QuadPart = -(LONGLONG)dwReadSize;

      current_out_pointer.LowPart =
	SetFilePointer(hOut, current_out_pointer.LowPart,
		       &current_out_pointer.HighPart, FILE_CURRENT);
      if (current_out_pointer.LowPart == INVALID_SET_FILE_POINTER)
	if (win_errno != NO_ERROR)
	  {
	    win_perror("Fatal, cannot set output file pointer");
	    return -1;
	  }
    }

  if (current_out_pointer.QuadPart + bufsiz >= existing_out_file_size.QuadPart)
    {
      fputs("\r\nFile not truncated.\r\n", stderr);
      return 0;
    }

  if (!SetEndOfFile(hOut))
    {
      win_perror("Fatal, cannot set end of file");
      return -1;
    }

  ULARGE_INTEGER current_size;
  current_size.LowPart = GetFileSize(hOut, &current_size.HighPart);
  if (current_size.LowPart == INVALID_FILE_SIZE)
    if (win_errno == NO_ERROR)
      {
	win_perror("Fatal, cannot get size of file");
	return -1;
      }

  fprintf(stderr, "\r\nFile truncated to %I64u bytes.\r\n", current_size);

  return 0;
}
