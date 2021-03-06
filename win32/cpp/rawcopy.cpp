#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN
#include <stdlib.h>
#include <winstrct.h>
#include <winioctl.h>

#include "rawcopy.rc.h"

#define STDBUFSIZ	512
#define BIGBUFSIZ	(512 << 10)

char buffer[STDBUFSIZ];
LARGE_INTEGER copylength = { 0 };
LARGE_INTEGER readbytes = { 0 };
LARGE_INTEGER writtenblocks = { 0 };
LARGE_INTEGER skippedwritebytes = { 0 };
SIZE_T bufsiz = STDBUFSIZ;

HANDLE hIn = INVALID_HANDLE_VALUE;
HANDLE hOut = INVALID_HANDLE_VALUE;

int
main(int argc, char **argv)
{
  bool bBigBufferMode = false;	// -m switch
  bool bDeviceLockRequired = true;	// not -l switch
  bool bVerboseMode = false;	// -v switch
  bool bIgnoreErrors = false;	// -i switch
  bool bAdjustSize = false;     // -a switch
  bool bCreateSparse = false;   // -s switch
  bool bDifferential = false;   // -D switch
  bool bNonBufferedIn = false;  // -r switch
  bool bNonBufferedOut = false; // -w switch
  bool bWriteThrough = false;   // -x switch
  bool bExtendedDASDIO = false; // -d switch
  bool bDisplayHelp = false;
  DWORD retrycount = 0;         // -f switch
  SIZE_T sizBigBufferSize = BIGBUFSIZ;	// -m:nnn parameter
  LARGE_INTEGER sizWriteOffset = { 0 };	// -o:nnn parameter

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
		sizBigBufferSize = strtoul(argv[1] + 2, &szSizeSuffix, 0);
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
		    sizBigBufferSize <<= 10;
		  case 'M':
		    sizBigBufferSize <<= 10;
		  case 'K':
		    sizBigBufferSize <<= 10;
		    break;
		  case 'g':
		    sizBigBufferSize *= 1000;
		  case 'm':
		    sizBigBufferSize *= 1000;
		  case 'k':
		    sizBigBufferSize *= 1000;
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
	  case 'o':
	    {
	      if (argv[1][1] != ':')
		{
		  bDisplayHelp = true;
		  break;
		}
	      
	      char *SizeSuffix = NULL;
	      switch (sscanf(argv[1] + 2, "%I64i%c",
			     &sizWriteOffset, &SizeSuffix))
		{
		case 2:
		  switch (*SizeSuffix)
		    {
		    case 'G':
		      sizWriteOffset.QuadPart <<= 10;
		    case 'M':
		      sizWriteOffset.QuadPart <<= 10;
		    case 'K':
		      sizWriteOffset.QuadPart <<= 10;
		      break;
		    case 'g':
		      sizWriteOffset.QuadPart *= 1000;
		    case 'm':
		      sizWriteOffset.QuadPart *= 1000;
		    case 'k':
		      sizWriteOffset.QuadPart *= 1000;
		    case 0:
		      break;
		    default:
		      fprintf(stderr,
			      "Invalid forward skip suffix: %c\r\n",
			      SizeSuffix);
		      return -1;
		    }

		case 1:
		  break;

		default:
		  fprintf(stderr, "Invalid skip size: %s\n", argv[1]);
		  return -1;
		}

	      if (bVerboseMode)
		printf("Write starts at %I64i bytes.\n", sizWriteOffset);
	    }

	    argv[1] += strlen(argv[1]) - 1;

	    break;
	  case 'f':
	    if (argv[1][1] != ':')
	      {
		bDisplayHelp = true;
		break;
	      }
	      
	    retrycount = strtoul(argv[1] + 2, NULL, 0);
	    if (retrycount == 0)
	      retrycount = 10;

	    argv[1] += strlen(argv[1]) - 1;

	    break;
	  case 'l':
	    bDeviceLockRequired = false;
	    break;
	  case 'i':
	    bIgnoreErrors = true;
	    break;
	  case 'v':
	    bVerboseMode = true;
	    break;
	  case 'a':
	    bAdjustSize = true;
	    break;
	  case 's':
	    bCreateSparse = true;
	    break;
	  case 'D':
	    bDifferential = true;
	    break;
	  case 'r':
	    bNonBufferedIn = true;
	    break;
	  case 'w':
	    bNonBufferedOut = true;
	    break;
	  case 'x':
	    bWriteThrough = true;
	    break;
	  case 'd':
	    bExtendedDASDIO = true;
	    break;
	  default:
	    bDisplayHelp = true;
	  }

      --argc;
      ++argv;
    }

  if (bIgnoreErrors)
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

  if (bDisplayHelp)
    {
      fputs("File and device read/write utility.\r\n"
	    "\n"
	    "Version " RAWCOPY_VERSION ". "
	    "Copyright (C) Olof Lagerkvist 1997-2014.\r\n"
	    "Differential operation based on modification by LZ.\r\n"
	    "This program is open source freeware.\r\n"
	    "http://www.ltr-data.se      olof@ltr-data.se\r\n"
	    "\n"
	    "Usage:\r\n"
	    "rawcopy [-lvirwxsd] [-f[:count]] [-m[:bufsize]] [-o:writeoffset]\r\n"
	    "       [[[[[skipforward] copylength] infile] outfile]\r\n"
	    "\n"
	    "Default infile/outfile if none or blank given is standard input/output device.\r\n"
	    "-l     Access devices without locking access to them. Use this switch when you\r\n"
	    "       are reading/writing physical drives and other processes are using the\r\n"
	    "       drives.\r\n"
	    "       Note! This may destroy your data!\r\n"
	    "\n"
	    "-v     Verbose mode. Writes to stderr what is being done.\r\n"
	    "\n"
	    "-f     Number of retries on failed I/O operations.\r\n"
	    "\n"
	    "-m     Try to buffer more of input file into memory before writing to output\r\n"
	    "       file.\r\n"
	    "\n"
	    "       Buffer size may be suffixed with K,M or G. If -m is given without a\r\n"
	    "       buffer size, 512 KB is assumed. If -m is not given a buffer size of 512\r\n"
	    "       bytes is used and read/write failures can be ignored.\r\n"
	    "\n"
	    "-i     Ignores and skip over read/write failures without displaying any dialog\r\n"
	    "       boxes.\r\n"
	    "\n"
	    "-r     Read input without intermediate buffering.\r\n"
	    "\n"
	    "-w     Write output without intermediate buffering.\r\n"
	    "\n"
	    "-x     Write through to output without going via system cache.\r\n"
	    "\n"
	    "-s     Creates output file as sparse file on NTFS volumes and skips explicitly\r\n"
	    "       writing all-zero blocks.\r\n"
	    "\n"
	    "-D     Differential operation. Skips rewriting blocks in output file that are\r\n"
	    "       already equal to corresponding blocks in input file.\r\n"
	    "\n"
	    "-d     Use extended DASD I/O when reading/writing disks. You also need to\n"
	    "       select a compatible buffer size with the -m switch for successful\r\n"
	    "       operation.\r\n"
	    "\n"
	    "-a     Adjusts size out output file to disk volume size of input. This switch\r\n"
	    "       is only valid if input is a disk volume.\r\n"
	    "\n"
	    "Examples for Windows NT:\r\n"
	    "rawcopy -m diskimage.img \\\\.\\A:\r\n"
	    "  Writes a diskette image file called \"diskimage.img\" to a physical diskette\r\n"
	    "  in drive A:. (File extension .img is just an example, sometimes such images\r\n"
	    "  are called for example .vfd.)\r\n"
	    "rawcopy \\\\.\\PIPE\\MYPIPE \"\"\r\n"
	    "  Reads data from named pipe \"MYPIPE\" on the local machine and write on\r\n"
	    "  standard output.\r\n"
	    "rawcopy 512 \\\\.\\PhysicalDrive0 parttable\r\n"
	    "  Copies 512 bytes (partition table) from first physical harddrive to file\r\n"
	    "  called \"parttable\".\r\n", stderr);

      return -1;
    }

  LARGE_INTEGER skipforward = { 0 };
  if (argc > 4)
    {
      char SizeSuffix;
      switch (sscanf(argv[1], "%I64i%c",
		     &skipforward, &SizeSuffix))
	{
	case 2:
	  switch (SizeSuffix)
	    {
	    case 'G':
	      skipforward.QuadPart <<= 10;
	    case 'M':
	      skipforward.QuadPart <<= 10;
	    case 'K':
	      skipforward.QuadPart <<= 10;
	      break;
	    case 'g':
	      skipforward.QuadPart *= 1000;
	    case 'm':
	      skipforward.QuadPart *= 1000;
	    case 'k':
	      skipforward.QuadPart *= 1000;
	    case 0:
	      break;
	    default:
	      fprintf(stderr,
		      "Invalid forward skip suffix: %c\r\n", SizeSuffix);
	      return -1;
	    }

	case 1:
	  break;

	default:
	  fprintf(stderr, "Invalid skip size: %s\n", argv[1]);
	  return -1;
	}

      if (bVerboseMode)
	printf("Skipping %I64i bytes.\n", skipforward);

      argv++;
      argc--;
    }

  if (argc > 3)
    {
      char SizeSuffix;
      switch (sscanf(argv[1], "%I64i%c",
		     &copylength, &SizeSuffix))
	{
	case 2:
	  switch (SizeSuffix)
	    {
	    case 'G':
	      copylength.QuadPart <<= 10;
	    case 'M':
	      copylength.QuadPart <<= 10;
	    case 'K':
	      copylength.QuadPart <<= 10;
	      break;
	    case 'g':
	      copylength.QuadPart *= 1000;
	    case 'm':
	      copylength.QuadPart *= 1000;
	    case 'k':
	      copylength.QuadPart *= 1000;
	    case 0:
	      break;
	    default:
	      fprintf(stderr, "Invalid copylength suffix: %c\r\n",
		      SizeSuffix);
	      return -1;
	    }

	case 1:
	  break;

	default:
	  fprintf(stderr, "Invalid copylength: %s\n", argv[1]);
	  return 1;
	}

      if (bVerboseMode)
	printf("Copying %I64i bytes.\n", copylength);

      argv++;
      argc--;
    }

  if (argc > 2 ? argv[1][0] : false)
    hIn = CreateFile(argv[1],
		     GENERIC_READ,
		     FILE_SHARE_READ | FILE_SHARE_WRITE,
		     NULL,
		     OPEN_EXISTING,
		     FILE_FLAG_SEQUENTIAL_SCAN |
		     (bNonBufferedIn ? FILE_FLAG_NO_BUFFERING : 0),
		     NULL);
  else
    hIn = hStdIn;

  if (hIn == INVALID_HANDLE_VALUE)
    {
      win_perror(argc > 1 ? argv[1] : "stdin");
      if (bVerboseMode)
	fputs("Error opening input file.\r\n", stderr);
      return -1;
    }

  if (skipforward.QuadPart)
    {
      if (SetFilePointer(hIn, skipforward.LowPart, &skipforward.HighPart,
			 FILE_CURRENT) == INVALID_SET_FILE_POINTER)
	if (win_errno != NO_ERROR)
	  {
	    win_perror("Fatal, cannot set input file pointer");
	    return -1;
	  }
    }

  if (bExtendedDASDIO)
    {
      // Turn on FSCTL_ALLOW_EXTENDED_DASD_IO so that we can make sure that we
      // read the entire drive
      DWORD dwReadSize;
      DeviceIoControl(hIn,
		      FSCTL_ALLOW_EXTENDED_DASD_IO,
		      NULL,
		      0,
		      NULL,
		      0,
		      &dwReadSize,
		      NULL);
    }

  LARGE_INTEGER disk_size = { 0 };
  if (bAdjustSize)
    {
      // Get volume size
      DWORD dwBytesReturned;
      DeviceIoControl(hIn,
		      IOCTL_DISK_UPDATE_PROPERTIES,
		      NULL,
		      0,
		      NULL,
		      0,
		      &dwBytesReturned,
		      NULL);

      PARTITION_INFORMATION partition_info = { 0 };
      DISK_GEOMETRY disk_geometry = { 0 };
      if (DeviceIoControl(hIn,
			  IOCTL_DISK_GET_PARTITION_INFO,
			  NULL,
			  0,
			  &partition_info,
			  sizeof(partition_info),
			  &dwBytesReturned,
			  NULL))
	disk_size.QuadPart = partition_info.PartitionLength.QuadPart;
      else if (DeviceIoControl(hIn,
			       IOCTL_DISK_GET_DRIVE_GEOMETRY,
			       NULL,
			       0,
			       &disk_geometry,
			       sizeof(disk_geometry),
			       &dwBytesReturned,
			       NULL))
	disk_size.QuadPart =
	  disk_geometry.Cylinders.QuadPart *
	  disk_geometry.TracksPerCylinder *
	  disk_geometry.SectorsPerTrack *
	  disk_geometry.BytesPerSector;
      else
	{
	  win_perror("Cannot get input disk volume size");
	  return 1;
	}
    }

  char *bufptr = NULL;
  char *diffbufptr = NULL;

  // If -m option is used we use big buffer
  if (bBigBufferMode)
    {
      if ((bufptr = (char *)LocalAlloc(LPTR, sizBigBufferSize)) != NULL)
	bufsiz = LocalSize(bufptr);
      else
	{
	  bufptr = buffer;
	  win_perror("Memory allocation error, "
		     "using 512 bytes buffer instead%%nError message");
	}

      if (bVerboseMode)
	fprintf(stderr, "Buffering %u bytes.\n", bufsiz);
    }
  // -m is not used
  else
    {
      bufptr = buffer;
      if (bVerboseMode)
	fprintf(stderr,
		"Sequential scan mode used, "
		"max read/write buffer is %u bytes.\n", bufsiz);
    }

  if (bDifferential)
    {
      diffbufptr = (char*)LocalAlloc(LPTR, bufsiz);
      if (diffbufptr == NULL)
	{
	  win_perror("Memory allocation error");
	  return -1;
	}
    }

  // Jump to next parameter if input file was given
  if (argc > 2)
    {
      argv++;
      argc--;
    }

  if (argc > 1 ? argv[1][0] : false)
    {
      hOut = CreateFile(argv[1],
			(bDifferential ? GENERIC_READ : 0) | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL |
			(bNonBufferedOut ? FILE_FLAG_NO_BUFFERING : 0) |
			(bWriteThrough ? FILE_FLAG_WRITE_THROUGH : 0),
			hIn);

      if (hOut == INVALID_HANDLE_VALUE)
	if (GetLastError() == ERROR_INVALID_PARAMETER)
	  hOut = CreateFile(argv[1],
			    (bDifferential ? GENERIC_READ : 0) | GENERIC_WRITE,
			    FILE_SHARE_READ | FILE_SHARE_WRITE,
			    NULL,
			    OPEN_EXISTING,
			    (bNonBufferedOut ? FILE_FLAG_NO_BUFFERING : 0) |
			    (bWriteThrough ? FILE_FLAG_WRITE_THROUGH : 0),
			    NULL);
    }
  else
    hOut = hStdOut;

  if (hOut == INVALID_HANDLE_VALUE)
    {
      win_perror(argc > 1 ? argv[1] : "stdout");
      if (bVerboseMode)
	fputs("Error opening output file.\r\n", stderr);
      return -1;
    }

  if (bExtendedDASDIO)
    {
      // Turn on FSCTL_ALLOW_EXTENDED_DASD_IO so that we can make sure that we
      // read the entire drive
      DWORD dwReadSize;
      DeviceIoControl(hOut,
		      FSCTL_ALLOW_EXTENDED_DASD_IO,
		      NULL,
		      0,
		      NULL,
		      0,
		      &dwReadSize,
		      NULL);
    }

  // If not regular disk file, try to lock volume using FSCTL operation
  BY_HANDLE_FILE_INFORMATION ByHandleFileInfo;
  if (!GetFileInformationByHandle(hIn, &ByHandleFileInfo))
    {
      DWORD z;
      FlushFileBuffers(hIn);
      if (DeviceIoControl(hIn, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &z, NULL))
	if (DeviceIoControl(hIn, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &z,
			    NULL))
	  {
	    if (bVerboseMode)
	      fputs("Source device locked and dismounted.\r\n", stderr);
	  }
	else
	  {
	    if (bVerboseMode)
	      fputs("Source device locked.\r\n", stderr);
	  }
      else
	switch (win_errno)
	  {
	  case ERROR_NOT_SUPPORTED:
	  case ERROR_INVALID_FUNCTION:
	  case ERROR_INVALID_HANDLE:
	  case ERROR_INVALID_PARAMETER:
	    break;

	  default:
	    if (bDeviceLockRequired)
	      {
		if (bVerboseMode)
		  {
		    DWORD dwSavedErrno = win_errno;
		    fprintf(stderr, "System Error %u.\n", dwSavedErrno);
		    win_errno = dwSavedErrno;
		  }

		win_perror("Cannot lock source device (use -l to ignore)");
		return -1;
	      }
	    else
	      win_perror("Warning! Source device not locked");
	  }
    }

  LARGE_INTEGER existing_out_file_size = { 0 };
  if (GetFileInformationByHandle(hOut, &ByHandleFileInfo))
    {
      existing_out_file_size.LowPart = ByHandleFileInfo.nFileSizeLow;
      existing_out_file_size.HighPart = ByHandleFileInfo.nFileSizeHigh;

      LARGE_INTEGER current_out_pointer = { 0 };
      current_out_pointer.LowPart =
	SetFilePointer(hOut, 0, &current_out_pointer.HighPart, FILE_CURRENT);
      if (current_out_pointer.LowPart == INVALID_SET_FILE_POINTER)
	if (win_errno != NO_ERROR)
	  {
	    win_perror("Fatal, cannot get output file pointer");
	    return -1;
	  }

      existing_out_file_size.QuadPart -= current_out_pointer.QuadPart;
    }
  else
    {
      DWORD z;
      FlushFileBuffers(hOut);

      if (DeviceIoControl(hOut, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &z, NULL))
	if (DeviceIoControl(hOut, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &z,
			    NULL))
	  {
	    if (bVerboseMode)
	      fputs("Target device locked and dismounted.\r\n", stderr);
	  }
	else
	  {
	    if (bVerboseMode)
	      fputs("Target device locked.\r\n", stderr);
	  }
      else
	switch (win_errno)
	  {
	  case ERROR_NOT_SUPPORTED:
	  case ERROR_INVALID_FUNCTION:
	  case ERROR_INVALID_HANDLE:
	  case ERROR_INVALID_PARAMETER:
	    break;

	  default:
	    if (bDeviceLockRequired)
	      {
		if (bVerboseMode)
		  {
		    DWORD dwSavedErrno = win_errno;
		    fprintf(stderr, "System Error %u.\n", dwSavedErrno);
		    win_errno = dwSavedErrno;
		  }

		win_perror("Cannot lock target device (use -l to ignore)");
		return -1;
	      }
	    else
	      win_perror("Warning! Target device not locked");
	  }
    }

  if (bCreateSparse)
    {
      DWORD dw;

      if (!DeviceIoControl(hOut, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &dw,
			   NULL))
	{
	  win_perror("Warning! Failed to set sparse flag for file");
	  bCreateSparse = false;
	}
    }

  if (sizWriteOffset.QuadPart != 0)
    {
      sizWriteOffset.LowPart =
	SetFilePointer(hOut,
		       sizWriteOffset.LowPart,
		       &sizWriteOffset.HighPart,
		       FILE_CURRENT);

      if (sizWriteOffset.LowPart == INVALID_SET_FILE_POINTER)
	if (GetLastError() != NO_ERROR)
	  {
	    win_perror("Error seeking on output device");
	    return 2;
	  }
    }

  for (;;)
    {
      Sleep(0);

      if (bVerboseMode)
	fprintf(stderr, "Reading block %I64u\r", writtenblocks);

      // Read next block
      DWORD dwBlockSize = (DWORD) bufsiz;
      DWORD retries = 0;

      if (copylength.QuadPart != 0)
	if (readbytes.QuadPart >= copylength.QuadPart)
	  break;
	else if ((dwBlockSize + readbytes.QuadPart) >= copylength.QuadPart)
	  {
	    dwBlockSize = (DWORD)(copylength.QuadPart - readbytes.QuadPart);
	    if (bVerboseMode)
	      fprintf(stderr, "Reading last %u bytes...\r", dwBlockSize);
	  }

      DWORD dwReadSize;
      while (!ReadFile(hIn, bufptr, dwBlockSize, &dwReadSize, NULL))
	{
	  // End of pipe?
	  DWORD dwErrNo = win_errno;
	  if ((dwErrNo == ERROR_BROKEN_PIPE) |
	      (dwErrNo == ERROR_INVALID_PARAMETER))
	    break;

	  WErrMsg errmsg;

	  if (retries < retrycount)
	    {
	      fprintf(stderr, "Rawcopy read failure: %s\nRetrying...\n",
		      (char*)errmsg);
	      retries++;
	      Sleep(500);
	      continue;
	    }

	  retries = 0;

	  switch (bIgnoreErrors ? IDIGNORE :
		  MessageBox(NULL, errmsg, "Rawcopy read failure",
			     MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION |
			     MB_DEFBUTTON2 | MB_TASKMODAL))
	    {
	    case IDABORT:
	      return -1;
	    case IDRETRY:
	      continue;
	    case IDIGNORE:
	      if (bVerboseMode)
		{
		  CharToOem(errmsg, errmsg);
		  fprintf(stderr, "Ignoring read error at block %I64u: %s\r\n",
			  writtenblocks, errmsg);
		}

	      dwReadSize = dwBlockSize;
	      ZeroMemory(bufptr, dwReadSize);
	      LARGE_INTEGER newpos = { 0 };
	      newpos.QuadPart = bufsiz;
	      if (SetFilePointer(hIn, newpos.LowPart, &newpos.HighPart,
				 FILE_CURRENT) == INVALID_SET_FILE_POINTER)
		if (win_errno != NO_ERROR)
		  {
		    win_perror("Fatal, cannot set input file pointer");
		    return -1;
		  }
	    }
	  break;
	}

      // Check for EOF condition
      if (dwReadSize == 0)
	{
	  if (bVerboseMode)
	    fputs("\r\nEnd of input.\r\n", stderr);

	  break;
	}

      // Check for all-zeroes block and skip explicitly writing it to output
      // file if "create sparse" flag is set and we are writing outside
      // output file data
      bool bSkipWriteBlock = false;
      if (bCreateSparse)
	if (readbytes.QuadPart >= existing_out_file_size.QuadPart)
	  {
	    bSkipWriteBlock = true;
	    for (PULONGLONG bufptr2 = (PULONGLONG) bufptr;
		 bufptr2 < (PULONGLONG) (bufptr + dwReadSize);
		 bufptr2++)
	      if (*bufptr2 != 0)
		{
		  bSkipWriteBlock = false;
		  break;
		}
	  }

      readbytes.QuadPart += dwReadSize;

      // Check existing block in output file and skip explicitly writing it
      // again if "differential" flag is set and output block is already equal
      // to corresponding block in input file.
      if (bDifferential)
	if (readbytes.QuadPart <= existing_out_file_size.QuadPart)
	  {
	    DWORD dwDiffReadSize;
	    if (!ReadFile(hOut, diffbufptr, dwReadSize, &dwDiffReadSize, NULL))
	      {
		win_perror("Error reading output file");
		return -1;
	      }

	    if (dwDiffReadSize != dwReadSize)
	      {
		fputs("Error reading output file.\r\n", stderr);
		return -1;
	      }

	    LARGE_INTEGER rewind_pos;
	    rewind_pos.QuadPart = -(LONGLONG)dwDiffReadSize;
	    if (SetFilePointer(hOut,
			       rewind_pos.LowPart,
			       &rewind_pos.HighPart,
			       FILE_CURRENT) == INVALID_SET_FILE_POINTER)
	      if (win_errno != NO_ERROR)
		{
		  win_perror("Fatal, cannot set output file pointer");
		  return -1;
		}

	    if (memcmp(bufptr, diffbufptr, dwReadSize) == 0)
	      bSkipWriteBlock = true;
	  }

      DWORD dwWriteSize = 0;
      if (bSkipWriteBlock)
	{
	  if (bVerboseMode)
	    fprintf(stderr, "Skipped block %I64u\r", writtenblocks);

	  LONG lZero = 0;
	  if (SetFilePointer(hOut, dwReadSize, &lZero, FILE_CURRENT) ==
	      INVALID_SET_FILE_POINTER)
	    if (win_errno != NO_ERROR)
	      {
		win_perror("Fatal, cannot set output file pointer");
		return -1;
	      }

	  if (readbytes.QuadPart > existing_out_file_size.QuadPart)
	    SetEndOfFile(hOut);

	  dwWriteSize = dwReadSize;

	  skippedwritebytes.QuadPart += dwWriteSize;
	}
      else
	{
	  if (bVerboseMode)
	    fprintf(stderr, "Writing block %I64u\r", writtenblocks);

	  DWORD retries = 0;

	  // Write next block
	  while (!WriteFile(hOut, bufptr, dwReadSize, &dwWriteSize, NULL))
	    {
	      DWORD dwErrNo = win_errno;
	      if ((dwErrNo == ERROR_BROKEN_PIPE) |
		  (dwErrNo == ERROR_INVALID_PARAMETER))
		break;

	      WErrMsg errmsg;

	      if (retries < retrycount)
		{
		  fprintf(stderr, "Rawcopy write failure: %s\nRetrying...\n",
			  (char*)errmsg);
		  retries++;
		  Sleep(500);
		  continue;
		}

	      retries = 0;

	      switch (bIgnoreErrors ? IDIGNORE :
		      MessageBox(NULL, errmsg, "Rawcopy write failure",
				 MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION |
				 MB_DEFBUTTON2 | MB_TASKMODAL))
		{
		case IDABORT:
		  return -1;
		case IDRETRY:
		  continue;
		case IDIGNORE:
		  if (bVerboseMode)
		    {
		      CharToOem(errmsg, errmsg);
		      fprintf(stderr,
			      "Ignoring write error at block %I64u: %s\r\n",
			      writtenblocks, errmsg);
		    }

		  LONG lZero = 0;
		  if (SetFilePointer(hOut, dwReadSize - dwWriteSize, &lZero,
				     FILE_CURRENT) == INVALID_SET_FILE_POINTER)
		    if (win_errno != NO_ERROR)
		      {
			win_perror("Fatal, cannot set output file pointer");
			return -1;
		      }
		}
	      break;
	    }

	  // Check for EOF condition
	  if (dwWriteSize == 0)
	    break;
	}

      if (dwWriteSize < dwReadSize)
	fprintf(stderr, "Warning: %u bytes lost.\n",
		dwReadSize - dwWriteSize);

      // Check for EOF condition
      if (dwWriteSize == 0)
	{
	  if (bVerboseMode)
	    fputs("\r\nEnd of output.\r\n", stderr);

	  break;
	}

      ++writtenblocks.QuadPart;
      if (copylength.QuadPart != 0)
	if (writtenblocks.QuadPart >= copylength.QuadPart)
	  break;
    }

  if (bAdjustSize)
    {
      if (disk_size.QuadPart > skipforward.QuadPart)
	disk_size.QuadPart -= skipforward.QuadPart;

      ULARGE_INTEGER existing_size = { 0 };
      DWORD ptr;

      // This piece of code compares size of created image file with that of
      // the original disk volume and possibly adjusts image file size if it
      // does not exactly match the size of the original disk/partition.
      existing_size.LowPart = GetFileSize(hOut, &existing_size.HighPart);
      if (existing_size.LowPart == INVALID_FILE_SIZE)
	if (GetLastError() != NO_ERROR)
	  {
	    win_perror("Error getting output file size");
	    return 1;
	  }

      if (existing_size.QuadPart < (ULONGLONG)disk_size.QuadPart)
	{
	  fprintf(stderr,
		  "Warning: Output file size is smaller "
		  "than input disk volume size.\r\n"
		  "Input disk volume size: %I64i bytes.\r\n"
		  "Output image file size: %I64i bytes.\r\n",
		  disk_size, existing_size);
	  return 1;
	}

      ptr = SetFilePointer(hOut,
			   disk_size.LowPart,
			   (LPLONG) &disk_size.HighPart,
			   FILE_BEGIN);
      if (ptr == INVALID_SET_FILE_POINTER)
	if (GetLastError() != NO_ERROR)
	  {
	    win_perror("Error setting output file size");
	    return 1;
	  }

      if (!SetEndOfFile(hOut))
	{
	  win_perror("Error setting output file size");
	  return 1;
	}
    }

  if (bDifferential)
    {
      LARGE_INTEGER updated_bytes;
      updated_bytes.QuadPart = readbytes.QuadPart - skippedwritebytes.QuadPart;
      fprintf(stderr, "\r\n%.4g %s of %.4g %s updated (%.3g %%).\r\n",
	      TO_h(updated_bytes.QuadPart), TO_p(updated_bytes.QuadPart),
	      TO_h(readbytes.QuadPart), TO_p(readbytes.QuadPart),
	      (double)(100 * (double)updated_bytes.QuadPart /
		       readbytes.QuadPart));
    }
  else if (bVerboseMode)
    fprintf(stderr, "\r\n%.4g %s copied.\r\n",
	    TO_h(readbytes.QuadPart), TO_p(readbytes.QuadPart));
}
