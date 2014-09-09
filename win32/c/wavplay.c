#define WINVER                0x0400
#define _WIN32_WINDOWS        0x0400
#define _WIN32_WINNT          0x0400
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <commdlg.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "comdlg32.lib")

#pragma comment(linker, "/subsystem:windows")
#pragma comment(linker, "/entry:startup")

int
startup()
{
  OPENFILENAME of = { sizeof(of) };
  char File[MAX_PATH] = "";
  of.lpstrFilter = "Wave files\0*.wav\0All files\0*.*\0";
  of.lpstrFile = File;
  of.nMaxFile = sizeof File;
  of.lpstrTitle = "Select a file to play";
  of.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

  while (GetOpenFileName(&of))
    PlaySound(File, NULL, SND_ASYNC | SND_FILENAME | SND_NOWAIT);

  PlaySound(NULL, NULL, 0);

  ExitProcess(0);
}
