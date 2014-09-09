#define _WIN32_WINNT 0x0500

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>

#ifdef _DLL
#pragma comment(lib, "ntdllp.lib")
#endif

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "kernel32.lib")

int
WINAPI
WinMain(HINSTANCE, HINSTANCE, LPSTR, int nCmdShow)
{
  OPENFILENAME_NT4 openFileDialog = { 0 };

  openFileDialog.lStructSize = sizeof(openFileDialog);

  openFileDialog.Flags =
    OFN_EXPLORER |
    OFN_FORCESHOWHIDDEN |
    OFN_ALLOWMULTISELECT |
    OFN_DONTADDTORECENT |
    OFN_FILEMUSTEXIST |
    OFN_HIDEREADONLY |
    OFN_LONGNAMES |
    OFN_PATHMUSTEXIST;

  WCHAR current_path[MAX_PATH] = L"";
  if (GetCurrentDirectory(sizeof(current_path)/sizeof(*current_path)-1,
			  current_path))
    openFileDialog.lpstrInitialDir = current_path;

  WCHAR file_names[32768] = L"";
  openFileDialog.lpstrFile = file_names;
  openFileDialog.nMaxFile = sizeof(file_names)/sizeof(*file_names)-1;

  while (GetOpenFileName((LPOPENFILENAME)&openFileDialog))
    {
      LPWSTR fname = file_names + wcslen(file_names) + 1;

      if (*fname == 0)
	fname = file_names;

      for (;
	   *fname != 0;
	   fname += wcslen(fname)+1)
	ShellExecute(NULL,
		     L"open",
		     fname,
		     NULL,
		     NULL,
		     nCmdShow);
    }
}

extern "C" int
WinMainCRTStartup()
{
  ExitProcess(WinMain(NULL, NULL, NULL, SW_SHOWDEFAULT));
}
