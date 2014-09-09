#if defined(UNICODE) || defined(_UNICODE)
#  ifndef UNICODE
#    define UNICODE
#  endif
#  ifndef _UNICODE
#    define _UNICODE
#  endif
#define APP_TITLE "CALLDLLW"
#else
#define APP_TITLE "CALLDLL"
#endif

#define WIN32_LEAN_AND_MEAN

#include <winstrct.h>

#if defined(_DLL) && !defined(_WIN64)
#include <minwcrtlib.h>
#pragma comment(lib, "minwcrt")
#else
#include <stdlib.h>
#endif

typedef DWORD WINAPI CallDLLFunc(LPCTSTR szParam);

int PASCAL _tWinMain(HINSTANCE hCurInstance, HINSTANCE hPrevInstance,
		     LPTSTR lpCmdLine, int nCmdShow)
{
  HINSTANCE hLib;
  CallDLLFunc *Func;
#ifdef UNICODE
  size_t SizeNeeded;
  LPSTR lpszProcedureName;
#endif

  if ((__argc < 3) | (__argc > 4))
    {
      MessageBox(NULL,
		 _T("Syntax:\r\nCALLDLL <dllfile> <function> [\"string\"]"),
		 _T(APP_TITLE), MB_ICONINFORMATION);
      return -1;
    }

  hLib = LoadLibrary(__targv[1]);
  if (hLib == NULL)
    {
      MessageBox(NULL, win_error, __targv[1], MB_ICONEXCLAMATION);
      return -1;
    }

#ifdef UNICODE
  SizeNeeded = wcstombs(NULL, __wargv[2], 0);
  if (SizeNeeded == (size_t)-1)
    {
      MessageBoxA(NULL, "Unicode conversion error.", APP_TITLE, MB_ICONSTOP);
      return -1;
    }

  lpszProcedureName = malloc(SizeNeeded);
  if (lpszProcedureName == NULL)
    {
      MessageBoxA(NULL, _strerror(NULL), APP_TITLE, MB_ICONSTOP);
      return -1;
    }

  wcstombs(lpszProcedureName, __wargv[2], SizeNeeded);
  lpszProcedureName[SizeNeeded] = 0;
#else
#  define lpszProcedureName __argv[2]
#endif

  Func = (CallDLLFunc*)GetProcAddress(hLib, lpszProcedureName);
  if (Func == NULL)
    {
      MessageBoxA(NULL, win_errorA, lpszProcedureName, MB_ICONEXCLAMATION);
      return -1;
    }

  return Func(__targv[3]);
}
