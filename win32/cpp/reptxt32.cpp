#define WIN32_LEAN_AND_MEAN

#include <string.h>

#include <winstrct.h>
#include <wconsole.h>
#include <wstring.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

#define MAX_COMPARE_LENGTH 260

DWORD process(const char[], const char[], const char[], DWORD, DWORD);

int
main(int argc, char **argv)
{
  bool hidden = false;
  DWORD dwCmpFlags = SORT_STRINGSORT;
  char file[MAX_PATH] = "";

  while (file[0]++ < 2)
    if (argc - 1)
      if ((argv[1][0] | 0x02) == '/')
	if ((argv[1][1] | 0x20) == 'a')
	  {
	    hidden = true;
	    if ((argv[1][2] | 0x20) == 'i')
	      dwCmpFlags |= NORM_IGNORECASE;
	    ++argv;
	    --argc;
	  }
	else if ((argv[1][1] | 0x20) == 'i')
	  {
	    dwCmpFlags |= NORM_IGNORECASE;
	    if ((argv[1][2] | 0x20) == 'a')
	      hidden = true;
	    ++argv;
	    --argc;
	  }
	else
	  {
	    puts("REPTXT32, freeware by Olof Lagerkvist.\r\n"
		 "http://www.ltr-data.se      olof@ltr-data.se\r\n"
		 "More info, including distribution permissions and\r\n"
		 "source code are available on the website.\r\n"
		 "\n"
		 "Command line syntax:\r\n"
		 "REPTXT32 [/A][/I] filename \"text1\" \"text2\"\r\n"
		 "\n"
		 "The program replaces text1 with text2 in the files. The /A switch includes\r\n"
		 "hidden files and the /I switch ignores lower/uppercase when searching for\r\n"
		 "text1 in the files.\r\n"
		 "Please note that the length of text1 and text2 must be exactly the same.");
	    return 0;
	  }

  if (argc - 1)
    {
      if (argv[1][0] == '\"')
	strncpy(file, argv[1] + 1, sizeof(file));
      else
	strncpy(file, argv[1], sizeof(file));

      if (file[strlen(file) - 1] == '\"')
	file[strlen(file) - 1] = 0;
    }
  else
    {
      printf("Filename: ");
      file[0] = 0;
      fgets(file, sizeof file, stdin);
      file[strlen(file) - 1] = 0;
    }

  strncpy(file, strtrm(file), sizeof file);
  file[sizeof(file) - 1] = 0;

  if (file[0] == 0)
    return 0;

  char cmpstr[MAX_COMPARE_LENGTH];
  if (argc > 2)
    {
      if (argv[2][0] == '\"')
	strncpy(cmpstr, argv[2] + 1, sizeof cmpstr);
      else
	strncpy(cmpstr, argv[2], sizeof cmpstr);

      if (cmpstr[strlen(cmpstr) - 1] == '\"')
	cmpstr[strlen(cmpstr) - 1] = 0;
    }
  else
    {
      printf("Type the text string to be replaced: ");
      cmpstr[0] = 0;
      fgets(cmpstr, sizeof cmpstr, stdin);
      cmpstr[strlen(cmpstr) - 1] = 0;
    }

  if (cmpstr[0] == 0)
    return 0;

  UINT txtlen = (UINT) strlen(cmpstr);
  char newstr[MAX_COMPARE_LENGTH];
  FillMemory(newstr, txtlen, ' ');

  if (argc > 3)
    {
      if (argv[3][0] == '\"')
	strncpy(newstr, argv[3] + 1, txtlen + 1);
      else
	strncpy(newstr, argv[3], txtlen + 1);

      if (newstr[strlen(newstr) - 1] == '\"')
	newstr[strlen(newstr) - 1] = 0;
    }
  else
    {
      printf("Type the replacement text string   : ");
      fgets(newstr, txtlen + 1, stdin);
    }

  newstr[strlen(newstr)] = ' ';
  newstr[txtlen] = 0;

  char *fptr = strrchr(file, '\\');
  if (fptr == NULL)
    fptr = file;
  else
    fptr++;

  WIN32_FIND_DATA ft;
  HANDLE hFFF;
  if ((hFFF = FindFirstFile(file, &ft)) == INVALID_HANDLE_VALUE)
    {
      win_perror(file);
      return EOF;
    }

  do
    {
      if (ft.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	continue;

      if ((ft.nFileSizeHigh == 0) && (ft.nFileSizeLow < txtlen))
	{
	  printf("File \"%s\" ignored: Too short to contain \"%s\".",
		 ft.cFileName, cmpstr);
	  clreol();
	  puts("");

	  continue;
	}

      strncpy(fptr, ft.cFileName, sizeof(file) - (fptr - file));

      if (hidden)
	SetFileAttributes(ft.cFileName, FILE_ATTRIBUTE_NORMAL);
      else if (ft.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
	continue;

      DWORD dwReplaces =
	process(file, cmpstr, newstr, txtlen, dwCmpFlags);

      if (dwReplaces)
	{
	  printf("\r\"%s\" replaced with \"%s\" in %u place%s "
		 "in \"%s\".",
		 cmpstr, newstr, dwReplaces, (dwReplaces > 1 ? "s" : ""),
		 file);
	  clreol();
	  puts("");
	}

      if (hidden)
	SetFileAttributes(ft.cFileName, ft.dwFileAttributes);
    }
  while (FindNextFile(hFFF, &ft));
  FindClose(hFFF);

  fputc('\r', stdout);
  clreol();

  return 0;
}

DWORD
process(const char cFilename[], const char cmpstr[],
	const char newstr[], DWORD txtlen, DWORD dwCmpFlags)
{
  HANDLE hFile = CreateFile(cFilename, GENERIC_READ | GENERIC_WRITE,
			    FILE_SHARE_DELETE, NULL, OPEN_ALWAYS,
			    FILE_FLAG_SEQUENTIAL_SCAN, NULL);

  if (hFile == INVALID_HANDLE_VALUE)
    {
      win_perror(cFilename);
      return 0;
    }

  printf("\rReading %s...", cFilename);
  clreol();

  HANDLE hMap = CreateFileMapping(hFile, NULL,
				  PAGE_READWRITE | SEC_COMMIT | SEC_NOCACHE,
				  0, 0,
				  NULL);
  if (hMap == NULL)
    {
      win_perror(cFilename);
      CloseHandle(hFile);
      return 0;
    }

  char *buffer = (char *)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
  if (!buffer)
    {
      win_perror();
      CloseHandle(hMap);
      CloseHandle(hFile);
      return 0;
    }

  printf("\rSearching \"%s\"...", cFilename);
  clreol();

  DWORD stoppos = GetFileSize(hFile, NULL) - txtlen;
  DWORD occur = 0;
  LCID lcidLocale = MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
			     SORT_DEFAULT);
  for (DWORD now = 0; now <= stoppos; now++)
    {
      if (CompareString(lcidLocale, dwCmpFlags, buffer + now, txtlen, cmpstr,
			txtlen) == 2)
	{
	  CopyMemory(buffer + now, newstr, txtlen);
	  ++occur;

	  printf("\r%u replacement%s in \"%s\"...",
		 occur, occur > 1 ? "s" : "", cFilename);
	  clreol();
	}

      Sleep(0);
    }

  UnmapViewOfFile(buffer);
  CloseHandle(hMap);
  CloseHandle(hFile);

  return occur;
}
