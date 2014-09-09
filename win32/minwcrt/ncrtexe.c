#pragma comment(linker, "/nodefaultlib:msvcrt.lib")

__declspec(dllimport) void exit(int);

#define NULL ((void*)0)
#define SW_SHOWNORMAL 1

#ifdef WPRFLAG
typedef unsigned short tchar;
#define tmain wmain
#define tmainCRTStartup wmainCRTStartup
#define tWinMain wWinMain
#define tWinMainCRTStartup wWinMainCRTStartup
#else
typedef char tchar;
#define tmain main
#define tWinMain WinMain
#define tmainCRTStartup mainCRTStartup
#define tWinMainCRTStartup WinMainCRTStartup
#endif

#ifdef _WINMAIN_
int __stdcall tWinMain(void *, void *, tchar *, int);

int tWinMainCRTStartup()
{
  int mainret = tWinMain(NULL, NULL, NULL, SW_SHOWNORMAL);
  exit(mainret);
  return mainret;
}
#else
int tmain(int, tchar**, tchar**);

int tmainCRTStartup()
{
  int mainret = tmain(0, NULL, NULL);
  exit(mainret);
  return mainret;
}
#endif
