#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <wconsole.h>
#include <wfind.h>

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "winstrct.lib")

char file[32769];
char oem_file[32769];
LARGE_INTEGER TotalFileSize;
LONGLONG BytesPerCluster;
DWORD dwBytesPerSector;
DWORD dwSectorsPerCluster;
DWORD dwFreeClusters;
DWORD dwTotalClusters;
bool follow_reparse_points = true;
bool count_files = false;

//
// These classes are used to track multiple hard links to files.
//

// This needs to be a power of 2
#define FILE_HASH_TABLE_SIZE 1024

class FileTableClass
{
  ULARGE_INTEGER FileNumber;
  DWORD VolumeSerialNumber;
  FileTableClass *Next;

  FileTableClass(ULARGE_INTEGER &NewFileNumber,
		 DWORD NewVolumeSerialNumber,
		 FileTableClass *NewNext) :
    FileNumber(NewFileNumber),
    VolumeSerialNumber(NewVolumeSerialNumber),
    Next(NewNext)
  {
  }

public:

  // This function adds a file to the list if it is not already in the list.
  // Returns true if the list already held the file, false otherwise.
  static bool FindOrAdd(DWORD dwLowPart, DWORD dwHighPart,
			DWORD dwVolumeSerialNumber)
  {
    ULARGE_INTEGER FileNumber;
    FileNumber.HighPart = dwHighPart;
    FileNumber.LowPart = dwLowPart;

    for (FileTableClass *Ptr =
	   FileHashTable[FileNumber.QuadPart & (FILE_HASH_TABLE_SIZE - 1)];
	 Ptr != NULL;
	 Ptr = Ptr->Next)
      if ((Ptr->FileNumber.QuadPart == FileNumber.QuadPart) &
	  (Ptr->VolumeSerialNumber == dwVolumeSerialNumber))
	return true;

    FileHashTable[FileNumber.QuadPart & (FILE_HASH_TABLE_SIZE - 1)] =
      new FileTableClass(FileNumber, dwVolumeSerialNumber,
			 FileHashTable[FileNumber.QuadPart &
				       (FILE_HASH_TABLE_SIZE - 1)]);

    return false;
  }
} *FileHashTable[FILE_HASH_TABLE_SIZE] = { NULL };

void
directory(char *fptr, DWORD dwLastClusterMask);

int
main(int argc, char **argv)
{
  enum PRESENTATION_FORMAT
    {
      PRESENTATION_FORMAT_BYTES,
      PRESENTATION_FORMAT_BLOCKS,
      PRESENTATION_FORMAT_HUMAN_READABLE,
      PRESENTATION_FORMAT_KB,
      PRESENTATION_FORMAT_MB,
      PRESENTATION_FORMAT_GB,
      PRESENTATION_FORMAT_TB
    } presentation_format = PRESENTATION_FORMAT_BYTES;

  bool displayhelp = false;

  SetOemPrintFLineLength(GetStdHandle(STD_ERROR_HANDLE));

  if (argc - 1)
    while ((argv[1][0] | 0x02) == '/')
      {
	while (*(++argv[1]))
	  switch (*argv[1] | 0x20)
	    {
	    case 'x':
	      follow_reparse_points = false;
	      continue;
	    case 'b':
	      presentation_format = PRESENTATION_FORMAT_BLOCKS;
	      continue;
	    case 'h':
	      presentation_format = PRESENTATION_FORMAT_HUMAN_READABLE;
	      continue;
	    case 'k':
	      presentation_format = PRESENTATION_FORMAT_KB;
	      continue;
	    case 'm':
	      presentation_format = PRESENTATION_FORMAT_MB;
	      continue;
	    case 'g':
	      presentation_format = PRESENTATION_FORMAT_GB;
	      continue;
	    case 't':
	      presentation_format = PRESENTATION_FORMAT_TB;
	      continue;
	    case 'c':
	      count_files = true;
	      continue;
	    default:
	      displayhelp = true;
	      continue;
	    }
	++argv;
	--argc;
	
	if (argc == 1)
	  break;
      }

  if (argc == 1)
    displayhelp = true;

  if (displayhelp)
    {
      puts("sizeof, freeware by Olof Lagerkvist.\r\n"
	   "http://www.ltr-data.se   e-mail: olof@ltr-data.se\r\n"
	   "More info, including distribution permissions and\r\n"
	   "source code is available on the website.\r\n"
	   "\n"
	   "This tool displays the total allocation size on disk for files and directories.\r\n"
	   "\n"
	   "Command line syntax:\r\n"
	   "sizeof [-x] [-presentation] file1 [file2 ...]\r\n"
	   "\n"
	   "-x       Do not follow junctions or other reparse points. This makes sure that\r\n"
	   "         sizeof stays within the filesystem where it starts searching.\r\n"
	   "\n"
	   "-c       Count and display number of files in each found directory, instead of\r\n"
	   "         count and display sizes of them.\r\n"
	   "\n"
	   "The presentation determines in which format to display the sizes of files and\r\n"
	   "directories. The argument can be any of the following:\r\n\n"
	   "-b       Number of 512-byte blocks.\r\n"
	   "-h       Human readable format.\r\n"
	   "-k       Kilobytes.\r\n"
	   "-m       Megabytes.\r\n"
	   "-g       Gigabytes.\r\n"
	   "-t       Terabytes.");
      return 0;
    }

  EnableBackupPrivileges();
  
  while (argc-- > 1)
    {
      ++argv;

      if (strlen(argv[0]) > sizeof(file))
	{
	  oem_printf(stderr, "Too long path: %1%%n", argv[0]);
	  continue;
	}

      strcpy(file, argv[0]);

      char *filepart = strrchr(file, '/');
      if (filepart != NULL)
	*filepart = '\\';

      filepart = strrchr(file, '\\');
      if (filepart != NULL)
	++filepart;
      else
	filepart = file;

      WFileFinder finder(file);
      if (!finder)
	{
	  win_perror(file);
	  continue;
	}

      const char *specialdirnode = NULL;
      if (strcmp(filepart, ".") == 0)
	specialdirnode = ".";
      else if (strcmp(filepart, "..") == 0)
	specialdirnode = "..";

      *filepart = 0;

      DWORD dwBytesPerSector;
      DWORD dwSectorsPerCluster;
      DWORD dwFreeClusters;
      DWORD dwTotalClusters;
      if (!GetDiskFreeSpace(file, &dwSectorsPerCluster, &dwBytesPerSector,
			    &dwFreeClusters, &dwTotalClusters))
	if (!GetDiskFreeSpace(NULL, &dwSectorsPerCluster, &dwBytesPerSector,
			      &dwFreeClusters, &dwTotalClusters))
	  {
	    win_perror();
	    return -1;
	  }

      BytesPerCluster = (LONGLONG) dwSectorsPerCluster * dwBytesPerSector;
      DWORD dwClusterMask = 0;
      while (BytesPerCluster >>= 1)
	++dwClusterMask;

      do
	{
	  if ((strcmp(finder.cFileName, ".") == 0) ||
	      strcmp(finder.cFileName, "..") == 0)
	    continue;

	  if (specialdirnode != NULL)
	    strcpy(finder.cFileName, specialdirnode);

	  *filepart = 0;
	  if (strlen(file) + strlen(finder.cFileName) >= sizeof(file))
	    {
	      oem_printf(stderr,
			 "Too long path: %1%2%%n", file, finder.cFileName);

	      if (finder.Next())
		continue;
	      else
		break;
	    }

	  strcpy(filepart, finder.cFileName);

	  CharToOem(file, oem_file);

	  if (finder.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	    {
	      if (strlen(file) + 13 >= sizeof(file))
		{
		  oem_printf(stderr,
			     "Too long path: %1%2%%n", file, finder.cFileName);
		  
		  if (finder.Next())
		    continue;
		  else
		    break;
		}

	      strcat(filepart, "\\");

	      TotalFileSize.QuadPart = 0;
	      directory(filepart + strlen(filepart), dwClusterMask);
	    }
	  // If just counting number of files
	  else if (count_files)
	    TotalFileSize.QuadPart = 1;
	  // If file is compressed, get compressed size before calculations
	  else if (finder.dwFileAttributes &
	      (FILE_ATTRIBUTE_COMPRESSED | FILE_ATTRIBUTE_SPARSE_FILE))
	    {
	      TotalFileSize.LowPart =
		GetCompressedFileSize(file, (LPDWORD) &TotalFileSize.HighPart);

	      if (TotalFileSize.LowPart == 0xFFFFFFFF)
		if (win_errno != NO_ERROR)
		  {
		    win_perror(file);
		    continue;
		  }
	    }
	  else
	    {
	      TotalFileSize.HighPart = finder.nFileSizeHigh;
	      TotalFileSize.LowPart = finder.nFileSizeLow;

	      TotalFileSize.QuadPart =
		(((TotalFileSize.QuadPart - 1) >> dwClusterMask) + 1) <<
		dwClusterMask;
	    }

	  switch (presentation_format)
	    {
	    case PRESENTATION_FORMAT_BLOCKS:
	      printf("%I64i\t%s\n", TotalFileSize.QuadPart >> 9, oem_file);
	      break;
	    case PRESENTATION_FORMAT_HUMAN_READABLE:
	      printf("%.4g %s\t%s\n", TO_h(TotalFileSize.QuadPart),
		     TO_p(TotalFileSize.QuadPart), oem_file);
	      break;
	    case PRESENTATION_FORMAT_KB:
	      printf("%I64i\t%s\n", TotalFileSize.QuadPart >> 10, oem_file);
	      break;
	    case PRESENTATION_FORMAT_MB:
	      printf("%I64i\t%s\n", TotalFileSize.QuadPart >> 20, oem_file);
	      break;
	    case PRESENTATION_FORMAT_GB:
	      printf("%I64i\t%s\n", TotalFileSize.QuadPart >> 30, oem_file);
	      break;
	    case PRESENTATION_FORMAT_TB:
	      printf("%I64i\t%s\n", TotalFileSize.QuadPart >> 40, oem_file);
	      break;
	    default:
	      printf("%I64i\t%s\n", TotalFileSize.QuadPart, oem_file);
	      break;
	    }
	}
      while (finder.Next());
    }
}

void
directory(char *fptr, DWORD dwLastClusterMask)
{
  DWORD dwClusterMask = 0;
  if (GetDiskFreeSpace(file, &dwSectorsPerCluster, &dwBytesPerSector,
		       &dwFreeClusters, &dwTotalClusters))
    {
      BytesPerCluster = (LONGLONG) dwSectorsPerCluster * dwBytesPerSector;
      dwClusterMask = 0;
      while (BytesPerCluster >>= 1)
	++dwClusterMask;
    }
  else
    dwClusterMask = dwLastClusterMask;

  fptr[0] = '*';
  fptr[1] = 0;

  Sleep(0);			// Let others run

  WFileFinder finddata(file);
  if (finddata)
    do
      if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
	  if (finddata.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
	    if (!follow_reparse_points)
	      continue;

	  if (!(strcmp(finddata.cFileName, ".") == 0) &
	      !(strcmp(finddata.cFileName, "..") == 0))
	    {
	      if (strlen(finddata.cFileName) + 13 + fptr - file >=
		  sizeof(file))
		{
		  *fptr = 0;
		  oem_printf(stderr,
			     "Too long path: %1%2%%n",
			     file, finddata.cFileName);
		  continue;
		}

	      strcpy(fptr, finddata.cFileName);
	      strcat(fptr, "\\");

	      directory(fptr + strlen(fptr), dwClusterMask);
	    }
	}
      else if (count_files)
	++TotalFileSize.QuadPart;
      else
	{
	  if (strlen(finddata.cFileName) + fptr - file >= sizeof file)
	    {
	      *fptr = 0;
	      oem_printf(stderr,
			 "Too long path: %1%2%%n", file, finddata.cFileName);

	      continue;
	    }
	  strcpy(fptr, finddata.cFileName);

	  bool skip_this = false;
	  HANDLE hFile =
	    CreateFile(file, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		       OPEN_EXISTING, 0, NULL);
	  if (hFile != INVALID_HANDLE_VALUE)
	    {
	      BY_HANDLE_FILE_INFORMATION file_info;
	      if (GetFileInformationByHandle(hFile, &file_info))
		skip_this =
		  FileTableClass::
		  FindOrAdd(file_info.nFileIndexLow,
			    file_info.nFileIndexHigh & 0xFFFF,
			    file_info.dwVolumeSerialNumber);

	      CloseHandle(hFile);
	    }

	  LARGE_INTEGER filesize;

	  // If file is compressed, get compressed size before calculations
	  if (skip_this)
	    ;
	  else if (finddata.dwFileAttributes &
		   (FILE_ATTRIBUTE_COMPRESSED | FILE_ATTRIBUTE_SPARSE_FILE))
	    {
	      filesize.LowPart =
		GetCompressedFileSize(file, (LPDWORD) &filesize.HighPart);

	      if ((filesize.LowPart == 0xFFFFFFFF) ?
		  (win_errno != NO_ERROR) : false)
		win_perror(file);
	      else
		TotalFileSize.QuadPart += filesize.QuadPart;
	    }
	  else
	    {
	      filesize.LowPart = finddata.nFileSizeLow;
	      filesize.HighPart = finddata.nFileSizeHigh;

	      TotalFileSize.QuadPart +=
		(((filesize.QuadPart - 1) >> dwClusterMask) + 1) <<
		dwClusterMask;
	    }

	}
	  
    while (finddata.Next());
}
