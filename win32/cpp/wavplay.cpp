#define WINVER                0x0400
#define _WIN32_WINDOWS        0x0400
#define _WIN32_WINNT          0x0400
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winuser.h>
#include <mmsystem.h>
#include <commdlg.h>

#pragma comment(lib, "kernel32")
#pragma comment(lib, "winmm")
#pragma comment(lib, "comdlg32")

#pragma comment(linker, "/subsystem:windows")
#pragma comment(linker, "/entry:startup")

int startup()
{
  OPENFILENAME of;
  char File[513] = "";

  of.lStructSize = sizeof(of);
  of.hwndOwner = NULL;
  of.hInstance = NULL;
  of.lpstrFilter = "Wave-filer\0*.wav\0Alla\0*.*\0";
  of.lpstrCustomFilter = NULL;
  of.nMaxCustFilter = 0;
  of.nFilterIndex = 1;
  of.lpstrFile = File;
  of.nMaxFile = sizeof(File) - 1;
  of.lpstrFileTitle = NULL;
  of.nMaxFileTitle = 0;
  of.lpstrInitialDir = NULL;
  of.lpstrTitle = "Välj fil som ska spelas upp";
  of.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
  while (GetOpenFileName(&of))
    PlaySound(File, NULL, SND_ASYNC | SND_FILENAME | SND_NOWAIT);

  PlaySound(NULL, NULL, NULL);

  ExitProcess(0);
}
