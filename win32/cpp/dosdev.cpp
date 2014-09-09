#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

#include "dosdev.rh"

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

char buffer[65535] = "";

int
main(int argc, char **argv)
{
  DWORD dwFlags = 0;
  bool displayhelp = false;
  bool querymode = false;

  // Nice argument parse loop :)
  while (argv[1] ? argv[1][0] ? (argv[1][0] | 0x02) == '/' : false : false)
    {
      while ((++argv[1])[0])
	switch (argv[1][0] | 0x20)
	  {
	  case 'r':
	    dwFlags |= DDD_RAW_TARGET_PATH;
	    break;
	  case 'd':
	    dwFlags |= DDD_REMOVE_DEFINITION;
	    break;
	  case 'l':
	    dwFlags |= DDD_EXACT_MATCH_ON_REMOVE;
	    break;
	  case 'q':
	    querymode = true;
	    break;
	  default:
	    displayhelp = true;
	  }

      --argc;
      ++argv;
    }

  if (argc < 2)
    querymode = true;

  if (displayhelp)
    {
      puts
	("DOSDEV is a user interface for the Win32 API functions DefineDosDevice() and\r\n"
	 "QueryDosDevice(), used to define, redefine, or delete MS-DOS device names in\r\n"
	 "Windows NT/2000/XP/2003 or MS-DOS drive aliases (SUBST) in Windows 95/98/ME.\r\n"
	 "\n" "Freeware by Olof Lagerkvist, (c) 1997-2011.\r\n"
	 "http://www.ltr-data.se     olof@ltr-data.se\r\n" "\n"
	 "Define devices syntax:\r\n"
	 "DOSDEV [/R] [/D] [/L] device targetpath ...\r\n" "\n"
	 "Query devices syntax (Windows NT only):\r\n"
	 "DOSDEV /Q [devices ...]\r\n" "\n"
	 "/R    Windows NT only: Don't convert the target path from an Win32 path (DOS-\r\n"
	 "      style path) to a native Windows NT path, but takes it as is.\r\n"
	 "\n" "/D    Windows 9x: Removes the specified drive alias.\r\n"
	 "      \r\n"
	 "      Windows NT: Removes the specified definition for the specified device. To\r\n"
	 "      determine which definition to remove, Windows NT walks the list of\r\n"
	 "      mappings for the device, looking for a match of specified target path\r\n"
	 "      against a prefix of each mapping associated with this device. The first\r\n"
	 "      mapping that matches is the one removed. If target path is not specified,\r\n"
	 "      Windows NT will remove the first mapping associated with the device and\r\n"
	 "      pop the most recent one pushed. If there is nothing left to pop, the\r\n"
	 "      device name will be removed. If /D is NOT specified, the target path\r\n"
	 "      parameter will become the new mapping for this device.\r\n"
	 "\n"
	 "/L    If this switch is given along with /D, Windows NT will use an exact match\r\n"
	 "      to determine which mapping to remove. Use this value to insure that you\r\n"
	 "      do not delete something that you did not define. In Windows 9x this\r\n"
	 "      switch is ignored.\r\n" "\n"
	 "/Q    Windows NT only: Displays target path(s) for the given devices. If no\r\n"
	 "      devices given, all currently defined MS-DOS devices are listed.");
      return 0;
    }

  if (querymode)
    do
      {
	buffer[0] = NULL;
	char *bufptr = buffer;
	if (QueryDosDevice(argv[1], buffer, sizeof(buffer)))
	  while (strlen(bufptr))
	    {
	      if (argc > 1)
		printf("%s => %s\n", argv[1], bufptr);
	      else
		puts(bufptr);

	      bufptr += strlen(bufptr) + 1;
	    }
	else
	  win_perror(argv[1]);

	if (!argv[1])
	  break;

	--argc;
	++argv;
      }
    while (argv[1]);
  else
    while (argv[1])
      {
	win_errno = NO_ERROR;
	DefineDosDevice(dwFlags, argv[1], argc > 2 ? argv[2] : NULL);
	win_perror(argv[1]);

	if (argv[2])
	  {
	    argc -= 2;
	    argv += 2;
	  }
	else
	  {
	    --argc;
	    ++argv;
	  }
      }

  return 0;
}
