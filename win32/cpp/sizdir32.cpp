#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <wconsole.h>
#include <wfind.h>

#pragma comment(lib, "winstrct.lib")
#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

char file[MAX_PATH], oem_file[MAX_PATH], *pattern;
int clussiz = 0;
QWORD minsiz = 0,
  maxsiz = (QWORD)-1i64,
  nrfiles = 0, totsiz = 0, allocsiz = 0, halfsiz = 0, dblsiz = 0, filesiz;
bool verbose = true, subdirs = true, compr = false;
volatile bool breaking = false;

void directory(char *);

BOOL WINAPI
CtrlHandler(DWORD dwCtrlType)
{
  switch (dwCtrlType)
    {
    case CTRL_C_EVENT: case CTRL_BREAK_EVENT: case CTRL_CLOSE_EVENT:
      breaking = true;
      return TRUE;
    default:
      return FALSE;
    }
}

int
main(int argc, char **argv)
{
  if (argc - 1)
    while ((argv[1][0] | 0x02) == '/')
      {
	while (*(++argv[1]))
	  switch (*argv[1] | 0x20)
	    {
	    case 'q':
	      verbose = false;
	      continue;
	    case 'c':
	      compr = true;
	      continue;
	    case 'l':
	      subdirs = false;
	      continue;
	    default:
	      puts
		("SIZDIR32, freeware by Olof Lagerkvist.\r\n"
		 "http://www.ltr-data.se   e-mail: olof@ltr-data.se\r\n"
		 "More info, including distribution permissions and\r\n"
		 "source code is available on the website.\r\n"
		 "\n"
		 "This program searches for files in a specified size interval in the current\r\n"
		 "directory and its subdirectories and displays disk usage information.\r\n"
		 "\n"
		 "Command line syntax:\r\n"
		 "SIZDIR32 [/C][/L][/Q] [minsize [maxsize [pattern]]]\r\n\n"
		 "/C       Finds correct allocation size for compressed files on NTFS volumes.\r\n"
		 "         This option may cause the search to run significantly slower.\r\n"
		 "/L       Search only current directory, no subdirectories.\r\n"
		 "/Q       No filename display. Displays only information about disk usage\r\n"
		 "         of the found files.\r\n"
		 "minsize  Lowest file size in the search interval. Without this parameter,\r\n"
		 "         you cannot specify highest file size or name pattern either and\r\n"
		 "         then the program only displays information about disk size,\r\n"
		 "         cluster size and free space.\r\n"
		 "maxsize  Highest file size in the search interval. Without this parameter\r\n"
		 "         (or if it is -1) files with sizes up to 2^64 are found.\r\n"
		 "pattern  Names of files to be found. This may include wildcards but no path.\r\n"
		 "\n"
		 "SIZDIR32 without parameters only displays information about disk allocation\r\n"
		 "size and free space.\r\n"
		 "\n"
		 "The minsize and maxsize parameters are numbers optionally appended by K, M, G\r\n"
		 "or T to specify kilobytes, megabytes, gigabytes or terabytes.");

	      return 0;
	    }
	++argv;
	--argc;

	if (argc == 1)
	  break;
      }

  if (argc > 1)
    {
      char *cMultiplicator = NULL;
      minsiz = (QWORD) strtol(argv[1], &cMultiplicator, 0);
      switch (cMultiplicator[0])
	{
	case 'K':
	  minsiz <<= 10;
	  break;
	case 'M':
	  minsiz <<= 20;
	  break;
	case 'G':
	  minsiz <<= 30;
	  break;
	case 'T':
	  minsiz <<= 40;
	  break;
	}
    }

  if (argc > 2)
    {
      char *cMultiplicator = NULL;
      maxsiz = (QWORD) strtol(argv[2], &cMultiplicator, 0);
      switch (cMultiplicator[0])
	{
	case 'K':
	  maxsiz <<= 10;
	  break;
	case 'M':
	  maxsiz <<= 20;
	  break;
	case 'G':
	  maxsiz <<= 30;
	  break;
	case 'T':
	  maxsiz <<= 40;
	  break;
	}
    }

  if (argc > 3)
    {
      if ((int)strchr(argv[3], '\\') | (int)strchr(argv[3], '/'))
	{
	  fputs("The name pattern cannot include a path.\r\n", stderr);
	  return 0;
	}

      pattern = argv[3];
    }
  else
    pattern = "*";

  SetConsoleCtrlHandler(CtrlHandler, TRUE);

  DWORD dwBytesPerSector;
  DWORD dwSectorsPerCluster;
  DWORD dwFreeClusters;
  DWORD dwTotalClusters;
  if (!GetDiskFreeSpace(".", &dwSectorsPerCluster, &dwBytesPerSector,
			&dwFreeClusters, &dwTotalClusters))
    if (!GetDiskFreeSpace(NULL, &dwSectorsPerCluster, &dwBytesPerSector,
			  &dwFreeClusters, &dwTotalClusters))
      {
	win_perror();
	return -1;
      }

  QWORD qwBytesPerCluster = (QWORD) dwSectorsPerCluster * dwBytesPerSector;

  if (argc > 1)
    {
      printf("Finding files '%s' in size interval from "
	     "%.4g %s to %.4g %s...\n",
	     pattern, TO_h(minsiz), TO_p(minsiz), TO_h(maxsiz), TO_p(maxsiz));
      fflush(stdout);

      QWORD bpc = qwBytesPerCluster;
      while (bpc >>= 1)
	++clussiz;

      directory(file);
      if (breaking)
	fputs("\r* break *", stdout);
      else
	fputc('\r', stdout);
      clreol();

      if (nrfiles)
	{
	  printf("\nFound %.f file%s of %.f byte%s (%.4g %s).\n",
		 (double)nrfiles, nrfiles > 1 ? "s" : "",
		 TO_B(totsiz), totsiz > 1 ? "s" : "",
		 TO_h(totsiz), TO_p(totsiz));

	  if (totsiz)
	    {
	      if (allocsiz > totsiz)
		printf("\nFiles allocating %.f byte%s (%.4g %s), "
		       "%.3g%% for padding.\n",
		       TO_B(allocsiz), allocsiz > 1 ? "s" : "",
		       TO_h(allocsiz), TO_p(allocsiz),
		       (double)(100 - 100 * (double)totsiz / allocsiz));
	      else
		printf("\nFiles allocating %.f byte%s (%.4g %s), "
		       "%.3g%% saved.\n",
		       TO_B(allocsiz), allocsiz > 1 ? "s" : "",
		       TO_h(allocsiz), TO_p(allocsiz),
		       (double) (100 - 100 * (double)allocsiz / totsiz));

	      if (halfsiz > totsiz)
		printf("\nUsing half block size, files would allocate\n"
		       "%.f byte%s (%.4g %s), "
		       "%.3g%% would be used for padding.\n",
		       TO_B(halfsiz), halfsiz > 1 ? "s" : "",
		       TO_h(halfsiz), TO_p(halfsiz),
		       (double) (100 - 100 * (double)totsiz / halfsiz));
	      else
		printf("\nUsing half block size, files would allocate\n"
		       "%.f byte%s (%.4g %s), "
		       "%.3g%% would be saved.\n",
		       TO_B(halfsiz), halfsiz > 1 ? "s" : "",
		       TO_h(halfsiz), TO_p(halfsiz),
		       (double) (100 - 100 * (double)halfsiz / totsiz));

	      if (dblsiz > totsiz)
		printf("Using double block size, files would allocate\n"
		       "%.f byte%s (%.4g %s), "
		       "%.3g%% would be used for padding.\n",
		       TO_B(dblsiz), dblsiz > 1 ? "s" : "",
		       TO_h(dblsiz), TO_p(dblsiz),
		       (double) (100 - 100 * (double)totsiz / dblsiz));
	      else
		printf("Using double block size, files would allocate\n"
		       "%.f byte%s (%.4g %s), "
		       "%.3g%% would be saved.\n",
		       TO_B(dblsiz), dblsiz > 1 ? "s" : "",
		       TO_h(dblsiz), TO_p(dblsiz),
		       (double) (100 - 100 * (double)dblsiz / totsiz));
	    }
	  puts("");
	}
      else
	puts("\r\nNo files found.\r\n");
    }

  QWORD qwBytesUsed = qwBytesPerCluster * (dwTotalClusters - dwFreeClusters);
  QWORD qwBytesFree = qwBytesPerCluster * dwFreeClusters;
  QWORD qwBytesTotal = qwBytesPerCluster * dwTotalClusters;
  printf("Allocation size on disk is %.f %s.\r\n"
	 "%.4g %s used and %.4g %s free of %.4g %s, "
	 "%.3g%% used and %.3g%% free.\r\n",
	 TO_h(qwBytesPerCluster), TO_p(qwBytesPerCluster),
	 TO_h(qwBytesUsed), TO_p(qwBytesUsed),
	 TO_h(qwBytesFree), TO_p(qwBytesFree),
	 TO_h(qwBytesTotal), TO_p(qwBytesTotal),
	 (double) 100 * qwBytesUsed / qwBytesTotal,
	 (double) 100 * qwBytesFree / qwBytesTotal);
}

void
directory(char *fptr)
{
  fputc('\r', stdout);

  static SIZE_T dwFileNameLength;
  dwFileNameLength = strlen(file);
  if (dwFileNameLength > 79)
    dwFileNameLength = 79;

  CharToOem(file, oem_file);
  fwrite(oem_file, dwFileNameLength, 1, stdout);
  clreol();
  Sleep(0);			// Let others run

  if (subdirs)
    {
      // Det här objektet måste läggas på stacken och kan inte vara statisk
      // eftersom directory() anropar sig själv.
      WFilteredFileFinder finddata("*", 0, FILE_ATTRIBUTE_DIRECTORY);
      if (finddata)
	do
	  if (!(strcmp(finddata.cFileName, ".") == 0) &
	      !(strcmp(finddata.cFileName, "..") == 0))
	    {
	      if (strlen(finddata.cFileName) + fptr - file >= sizeof(file))
		{
		  printf("Too long path to %s.\n", finddata.cFileName);
		  continue;
		}

	      strcpy(fptr, finddata.cFileName);
	      strcat(fptr, "\\");

	      if (SetCurrentDirectory(finddata.cFileName))
		{
		  directory(fptr + strlen(fptr));
		  SetCurrentDirectory("..");
		}
	    }
	while (finddata.Next() && !breaking);
    }

  WFilteredFileFinder finddata(pattern, FILE_ATTRIBUTE_DIRECTORY, 0, minsiz,
			       maxsiz);

  if (finddata)
    do
      {
	if (strlen(finddata.cFileName) + fptr - file >= sizeof file)
	  {
	    printf("Too long path to %s.\n", finddata.cFileName);

	    continue;
	  }
	strcpy(fptr, finddata.cFileName);

	++nrfiles;

	filesiz = finddata.GetFile64BitSize();

	if (verbose)
	  if (filesiz == -1i64)
	    {
	      win_perror(finddata.cFileName);
	      filesiz = 0;
	    }
	  else
	    {
	      CharToOem(file, oem_file);
	      printf("\r%s, %.4g %s", oem_file, TO_h(filesiz), TO_p(filesiz));
	      clreol();
	      puts("");
	    }

	totsiz += filesiz;

	// If file is compressed, get compressed size before calculations
	if (compr &&
	    (finddata.dwFileAttributes &
	     (FILE_ATTRIBUTE_COMPRESSED | FILE_ATTRIBUTE_SPARSE_FILE)))
	  filesiz = finddata.GetCompressedFile64BitSize();

	allocsiz += (((filesiz - 1) >> clussiz) + 1) << clussiz;
	halfsiz += (((filesiz - 1) >> (clussiz - 1)) + 1) << (clussiz - 1);
	dblsiz += (((filesiz - 1) >> (clussiz + 1)) + 1) << (clussiz + 1);
      }
    while (finddata.Next() && !breaking);
}
