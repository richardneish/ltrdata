#define _WIN32_WINNT 0x0400
#define _WIN32_WINDOWS 0x0400
#define WINVER 0x0400
#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <wconsole.h>

#include <conio.h>
#include <process.h>
#include <io.h>

#pragma comment(lib, "winstrct")

#ifdef _DLL
#pragma comment(lib, "minwcrt")
#endif

#define MAX_FILES 75

DWORD watchthread(HANDLE h)
{
  getch();
  return 0;
}

int
main(int argc, char **argv)
{
  DWORD beginsize[MAX_FILES] = { 0 };
  DWORD lastsize[MAX_FILES] = { 0 };
  DWORD iterations[MAX_FILES] = { 0 };

  int files;
  intptr_t i;
  struct _finddata_t ft;
  HANDLE hWatchThread;

  if (argc != 2)
    return 0;

  hWatchThread = (HANDLE)_beginthread(watchthread, 0, NULL);
  if (hWatchThread == INVALID_HANDLE_VALUE)
    {
      win_perror("_beginthread");
      return -1;
    }

  clrscr();

  printf("%16s - %10s - %10s - %s\n",
	 "size", "speed KBps", "avg KBps", "name");

  SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

  for (;;)
    {
      gotoxy(1, 2);
      files = 0;

      i = _findfirst(argv[1], &ft);
      if (i != -1)
	{
	  do
	    {
	      if (ft.attrib & _A_SUBDIR)
		continue;

	      if (beginsize[files] == 0)
		{
		  beginsize[files] = ft.size;
		  iterations[files] = 0;
		}
	      else
		++iterations[files];

	      if (((ft.size == lastsize[files]) & (ft.size != 0)) |
		  (iterations[files] == 0))
		printf("%11u - %10u\n",
		       (DWORD)ft.size,
		       (DWORD)0);
	      else
		{
		  CharToOem(ft.name, ft.name);

		  printf("%11u - %10u - %10u - %.44s\n",
			 (DWORD)ft.size,
			 (DWORD)(ft.size - lastsize[files]) >> 10,
			 (DWORD)((ft.size - beginsize[files]) /
				 iterations[files]) >> 10,
			 (char*)ft.name);
		}

	      lastsize[files ++] = ft.size;
	    }
	  while ((_findnext(i, &ft) == 0) & (files < MAX_FILES));
	  _findclose(i);
	}
      else
	perror(NULL);

      clreol();

      if (WaitForSingleObject(hWatchThread, 1000) != WAIT_TIMEOUT)
	break;
    }

  return 0;
}
