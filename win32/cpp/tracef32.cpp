#define _WIN32_WINNT 0x0400
#define _WIN32_WINDOWS 0x0400
#define WINVER 0x0400
#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <wconsole.h>
#include <wfind.h>

#include <stdlib.h>
#include <conio.h>
#include <process.h>
#include <io.h>

#pragma comment(lib, "winstrct.lib")

#define MAX_FILES 75

UINT
CALLBACK
watchthread(HANDLE h)
{
  getch();
  return 0;
}

int
main(int argc, char **argv)
{
  if (argc != 2)
    return 0;

  UINT tid;
  HANDLE hWatchThread = (HANDLE)
    _beginthreadex(NULL, 0, watchthread, NULL, 0, &tid);
  if (hWatchThread == NULL)
    {
      win_perror("_beginthread");
      return -1;
    }

  clrscr();

  printf("%12s - %10s - %10s - %s\n",
	 "size", "speed KBps", "avg KBps", "name");

  char *fullpath = (char*) malloc(strlen(argv[1]) + MAX_PATH);
  if (fullpath == NULL)
    {
      perror(NULL);
      return -1;
    }

  strcpy(fullpath, argv[1]);
  for (char *ptr = strchr(fullpath, '/');
       ptr != NULL;
       ptr = strchr(ptr + 1, '/'))
    *ptr = '\\';

  char *nameptr = strrchr(fullpath, '\\');
  if (nameptr != NULL)
    ++nameptr;
  else
    nameptr = fullpath;

  int files = 0;
  WFileFinder finddata(argv[1]);
  if (!finddata)
    {
      win_perror(argv[1]);
      return 1;
    }

  HANDLE handles[MAX_FILES] = { 0 };

  do
    {
      Sleep(0);

      if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
	  --files;
	  continue;
	}
      
      strcpy(nameptr, finddata.cFileName);

      handles[files] = CreateFile(fullpath, FILE_READ_ATTRIBUTES,
				  FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
				  OPEN_EXISTING, 0, NULL);

      CharToOem(finddata.cFileName, finddata.cFileName);

      if (handles[files] == INVALID_HANDLE_VALUE)
	{
	  handles[files] = NULL;
	  win_perror(finddata.cFileName);
	}
      else
	printf("%12s - %10s - %10s - %.38s\n",
	       "",
	       "0",
	       "0",
	       finddata.cFileName);
    }
  while (finddata.Next() & ((++files) < MAX_FILES));

  if (files < 1)
    return 0;

  SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

  LARGE_INTEGER beginsize[MAX_FILES] = { 0 };
  LARGE_INTEGER lastsize[MAX_FILES] = { 0 };
  LARGE_INTEGER iterations[MAX_FILES] = { 0 };

  for(;;)
    {
      for (int i = 0; i < files; i++)
	if (handles[i] != NULL)
	  {
	    gotoxy(1, (SHORT) i + 2);

	    BY_HANDLE_FILE_INFORMATION fileinfo;
	    if (GetFileInformationByHandle(handles[i], &fileinfo))
	      {
		LARGE_INTEGER filesize;
		filesize.LowPart = fileinfo.nFileSizeLow;
		filesize.HighPart = fileinfo.nFileSizeHigh;

		if (beginsize[i].QuadPart == 0)
		  {
		    beginsize[i] = filesize;
		    iterations[i].QuadPart = 0;
		  }
		else
		  iterations[i].QuadPart++;

		if ((filesize.QuadPart == lastsize[i].QuadPart) |
		    (iterations[i].QuadPart == 0))
		  printf("%#6.4g %5s - %10I64i",
			 TO_h(filesize.QuadPart),
			 TO_p(filesize.QuadPart),
			 (DWORD) 0);
		else
		  printf("%#6.4g %5s - %10I64i - %10I64i",
			 TO_h(filesize.QuadPart),
			 TO_p(filesize.QuadPart),
			 (filesize.QuadPart -
			  lastsize[i].QuadPart) >> 10,
			 ((filesize.QuadPart -
			   beginsize[i].QuadPart) /
			  iterations[i].QuadPart) >> 10);

		lastsize[i] = filesize;
	      }
	  }

      if (WaitForSingleObject(hWatchThread, 1000) != WAIT_TIMEOUT)
	break;
    }

  puts("");
  return 0;
}
