#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#pragma comment(lib, "user32.lib")

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

DWORD dwWndCount = 0;
DWORD dwChildrenCount = 0;
DWORD dwFindPid = 0;
BOOL bEnumChildren = FALSE;
BOOL bNoHidden = FALSE;
int iLevel = 0;

BOOL CALLBACK
EnumChildWindowProc(HWND hWnd, LPARAM lparam)
{
  char cBuf[MAX_PATH] = "";
  int i;

  if (bNoHidden)
    if (!IsWindowVisible(hWnd) | !IsWindowEnabled(hWnd))
      return TRUE;

  ++dwChildrenCount;

  ++iLevel;
  putch('\t');
  for (i = iLevel; i; i--)
    putch('\t');
  putch('\t');

  GetClassName(hWnd, cBuf, sizeof cBuf);
  CharToOem(cBuf, cBuf);
  printf("%s\\", cBuf);
  GetWindowText(hWnd, cBuf, sizeof cBuf);
  CharToOem(cBuf, cBuf);
  printf("%s\n", cBuf);

  EnumChildWindows(hWnd, (WNDENUMPROC)EnumChildWindowProc, 0);
  --iLevel;

  return TRUE;
}

BOOL CALLBACK EnumWindowProc(HWND hWnd, LPARAM lparam)
{
  char cBuf[MAX_PATH] = "";
  DWORD dwPid = 0;
  DWORD dwTid = GetWindowThreadProcessId(hWnd, &dwPid);

  if (bNoHidden)
    if (!IsWindowVisible(hWnd) | !IsWindowEnabled(hWnd))
      return TRUE;

  if (dwFindPid)
    if (dwFindPid != dwPid)
      return TRUE;

  ++dwWndCount;

  printf("%i\t%i\t", dwPid, dwTid);

  GetClassName(hWnd, cBuf, sizeof cBuf);
  CharToOem(cBuf, cBuf);
  printf("%s\\", cBuf);
  GetWindowText(hWnd, cBuf, sizeof cBuf);
  CharToOem(cBuf, cBuf);
  printf("%s\n", cBuf);

  if (bEnumChildren)
    EnumChildWindows(hWnd, (WNDENUMPROC)EnumChildWindowProc, 0);

  return TRUE;
}

int
main(int argc, char **argv)
{
  if (argv[1])
    if (stricmp(argv[1], "/CHILDREN") == 0)
      {
	bEnumChildren = TRUE;
	++argv;
      }

  if (argv[1])
    if (stricmp(argv[1], "/NOHIDDEN") == 0)
      {
	bNoHidden = TRUE;
	++argv;
      }

  if (argv[1])
    {
      dwFindPid = strtoul(argv[1], NULL, 0);
      if (dwFindPid != 0)
	++argv;
    }

  if (argv[1])
    {
      puts("Usage: ENUMWIN [/CHILDREN] [/NOHIDDEN] [pid]");
      return 0;
    }

  puts("Pid\tTid\tClass name\\Window title\r\n"
       "-------------------------------------------------------------");
  EnumWindows((WNDENUMPROC)EnumWindowProc, 0);

  if (bEnumChildren)
    printf("Found %u top level windows and %u child windows.\n", dwWndCount,
	   dwChildrenCount);
  else
    printf("Found %u windows.\n", dwWndCount);

  return dwWndCount+dwChildrenCount;
}

