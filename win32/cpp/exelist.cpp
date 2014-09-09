#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <wfilever.h>
#include <wimghlp.h>
#include <wfind.h>

#include <stdlib.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

#pragma comment(lib, "version")
#pragma comment(lib, "imagehlp")

char cStartPath[MAX_PATH << 2] = "";
char cCurPath[MAX_PATH << 2] = "";

char cFileName[MAX_PATH+1] = "";
bool bSubDirs = false;        // -r (/S) switch
bool bHidden = false;         // -h (/A) switch

void DoSubdir(char *pcCurPathPtr);
void DisplayFileInfo();

DWORD dwFoundFiles = 0;

int
main(int argc, char **argv)
{
  SetOemPrintFLineLength(GetStdHandle(STD_ERROR_HANDLE));

  bool bDisplayHelp = false;

  // Nice argument parse loop :)
  while (argv[1] ? argv[1][0] ? (argv[1][0]|0x02) == '/' : false : false)
    {
      while ((++argv[1])[0])
      	switch (argv[1][0]|0x20)
	  {
	  case 'a': case 'h':
	    bHidden = true;
	    break;
	  case 'r': case 's':
	    bSubDirs = true;
	    break;
	  default:
	    bDisplayHelp = true;
	  }
      
      --argc;
      ++argv;
    }
  
  if (argv[1] == NULL)
    bDisplayHelp = true;
  else if (argv[1][0] == 0)
    bDisplayHelp = true;
  
  if (bDisplayHelp)
   {
     puts("Utility to view information in Win32 EXE file header.\r\n"
	  "\n"
	  "Written by Olof Lagerkvist 2001\r\n"
	  "http://www.ltr-data.se    olof@ltr-data.se\r\n"
	  "\n"
	  "Usage:\r\n"
	  "EXELIST [-[a][r][s]] exefile [ ... ]\r\n"
	  "\n"
	  "-a            Find hidden files/directories.\r\n"
	  "-r or -s      Find in subdirectories.");

     return 0;
   }

  GetCurrentDirectory(sizeof cStartPath, cStartPath);
  cStartPath[sizeof(cStartPath) - 1] = 0;

  while ((++argv)[0] != NULL)
    {
      SetCurrentDirectory(cStartPath);

      char *pcCurPathPtr = cCurPath;
      
      if (!GetFullPathName(*argv, sizeof cCurPath, cCurPath, &pcCurPathPtr))
	{
	  win_perror(*argv);
	  continue;
	}

      if ((pcCurPathPtr == cCurPath) | (pcCurPathPtr == NULL))
	continue;

      strncpy(cFileName, pcCurPathPtr, sizeof cFileName);
      cFileName[sizeof(cFileName)-1] = 0;

      pcCurPathPtr[0] = NULL;

      if (cCurPath[0])
	if (!SetCurrentDirectory(cCurPath))
	  {
	    WErrMsg errmsg;
	    oem_printf(stderr,
		       "Error changing current directory to '%1': %2%%n",
		       cCurPath, errmsg);
            continue;
         }

      DoSubdir(pcCurPathPtr);
   }

  if (dwFoundFiles)
    return (int)dwFoundFiles;

  LPSTR lpFileName;
  if (SearchPath(NULL, argv[-1], ".exe", sizeof cCurPath, cCurPath,
		 &lpFileName) == 0)
    {
      win_perror(argv[-1]);
      return 0;
    }

  DisplayFileInfo();

  return (int)dwFoundFiles;
}

void DoSubdir(char *pcCurPathPtr)
{
  if (sizeof(cCurPath) - (pcCurPathPtr-cCurPath) < MAX_PATH - 3)
    {
      oem_printf(stderr, "Too long path: '%1'%%n", cCurPath);
      return;
    }

  if (bSubDirs)
    {
      WFilteredFileFinder filefind("*", bHidden ? 0 : FILE_ATTRIBUTE_HIDDEN,
				   FILE_ATTRIBUTE_DIRECTORY);
      if (!filefind)
	{
	  switch (win_errno)
	    {
	    case ERROR_FILE_NOT_FOUND: case ERROR_NO_MORE_FILES:
	      break;
	    default:
	      win_perror();
	    }

	  return;
	}

      do
	{
	  Sleep(0);

	  if (filefind.cFileName[0] == '.')
            if (filefind.cFileName[1] == NULL)
	      continue;
            else if(filefind.cFileName[1] == '.')
	      if (filefind.cFileName[2] == NULL)
		continue;

	  if (strlen(filefind.cFileName) >=
	      sizeof(cCurPath)-(pcCurPathPtr-cCurPath)-2)
	    {
	      oem_printf(stderr, "%1\\%2: Too long path.\n",
			 cCurPath,
			 filefind.cFileName);
	      continue;
	    }

	  strcpy(pcCurPathPtr, filefind.cFileName);
	  strcat(pcCurPathPtr, "\\");

	  if (!SetCurrentDirectory(cCurPath))
	    {
	      WErrMsg errmsg;

	      oem_printf(stderr,
			 "Cannot change current directory to '%1': %2%%n",
			 cCurPath, errmsg);
	      continue;
	    }

	  DoSubdir(cCurPath+strlen(cCurPath));
	  pcCurPathPtr[0] = NULL;

	  if (!SetCurrentDirectory(".."))
	    {
	      WErrMsg errmsg;

	      oem_printf(stderr,
			 "Cannot change back to '%1': %2%%n",
			 cCurPath, errmsg);
	      return;
	    }
	  
	} while (filefind.Next());

      switch (win_errno)
	{
	case ERROR_NO_MORE_FILES: case ERROR_FILE_NOT_FOUND:
	  break;
	default:
	  win_perror();
	}
   }

  WFilteredFileFinder filefind(cFileName, FILE_ATTRIBUTE_DIRECTORY|
			       (bHidden?0:FILE_ATTRIBUTE_HIDDEN), 0);
  if (!filefind)
    {
      switch (win_errno)
	{
	case ERROR_FILE_NOT_FOUND: case ERROR_NO_MORE_FILES:
	  break;
	default:
	  win_perror();
	}
      return;
    }
  
  do
    {
      Sleep(0);

      if (strlen(filefind.cFileName) >=
		 sizeof(cCurPath)-(pcCurPathPtr-cCurPath)-2)
	{
	  oem_printf(stderr,
		     "%1\\%2: Too long path.\n",
		     cCurPath, filefind.cFileName);
	  continue;
	}

      strcpy(pcCurPathPtr, filefind.cFileName);
      DisplayFileInfo();

    } while (filefind.Next());

  switch (win_errno) 
    {
    case ERROR_NO_MORE_FILES: case ERROR_FILE_NOT_FOUND:
      break;
    default:
      win_perror();
    }
}

void DisplayFileInfo()
{
  ++dwFoundFiles;
      
  WMappedImage image(cCurPath, ".", false, true);
  switch (win_errno)
    {
    case NO_ERROR:
      if (image.ModuleName)
	printf("Module name: %s\n", image.ModuleName);
      else
	printf("File name: %s\n", cCurPath);
      
      printf("Size of image: %.4g %s\n",
	     TO_h(image.SizeOfImage), TO_p(image.SizeOfImage));
      
      if (image.fSystemImage)
	puts("Windows NT kernel module image.");
      
      if (image.fDOSImage)
	puts("MZ format (MS-DOS executable) image.");
      else if (image.ModuleName)
	puts("PE format (Windows NT executable) image.");
      else
	puts("Unknown image type.");
      
      if (image.Characteristics)
	printf("Characteristics flags: %p\n",(void*)image.Characteristics);
      
      if (image.NumberOfSections)
	printf("Number of sections: %u\n", (DWORD)image.NumberOfSections);
      
      break;
      
    case ERROR_BAD_FORMAT:
      printf("File name: %s\n", cCurPath);
      break;
      
    default:
      win_perror(cCurPath);
    }
  
  DWORD dwTest = (DWORD)-1;
  if (GetBinaryType(cCurPath, &dwTest))
    switch (dwTest)
      {
      case SCS_32BIT_BINARY:
	puts("Windows 32-bit executable.");
	break;
      case SCS_DOS_BINARY:
	puts("MS-DOS executable.");
	break;
      case SCS_OS216_BINARY:
	puts("OS/2 subsystem executable.");
	break;
      case SCS_PIF_BINARY:
	puts("MS-DOS program information file (PIF).");
	break;
      case SCS_POSIX_BINARY:
	puts("POSIX subsystem executable.");
	break;
      case SCS_WOW_BINARY:
	puts("Windows 16-bit executable.");
	break;
#ifdef SCS_64BIT_BINARY
      case SCS_64BIT_BINARY:
	puts("Windows 64-bit executable.");
	break;
#endif
      default:
	puts("File is executable (unknown system ID).");
      }
  else
    puts("File is not executable.");    
  
  WFileVerInfo fvi(cCurPath);
  fvi.PrintCommonFileVerRecords();
  puts("");
}
