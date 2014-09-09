#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <shellapi.h>

#pragma comment(lib, "shell32.lib")
#pragma comment(linker, "/subsystem:windows")

typedef DWORD WINAPI CallDLLWFunc(LPCWSTR szParam);

int
wmain(int iArgc, LPWSTR *wszArgv)
{
  HINSTANCE hLib;
  CallDLLWFunc *Func;
  char *funcname = NULL;
  int funcnamelen = 0;

  if ((iArgc < 3) | (iArgc > 4))
    {
      MessageBox(NULL,
		 "Syntax:\r\nCALLDLLW <dllfile> <function> [\"string\"]",
		 "CALLDLLW", MB_ICONINFORMATION);
      return -1;
    }

  funcnamelen = WideCharToMultiByte(CP_ACP, 0, wszArgv[2], -1, NULL, 0, NULL,
				    NULL);
  if (funcnamelen == 0)
    {
      MessageBox(NULL, win_errorA,
		 "Wide to multibyte character conversion error", MB_ICONSTOP);
      return -1;
    }

  funcname = halloc(funcnamelen);
  if (funcname == NULL)
    {
      MessageBox(NULL, win_error, "Memory allocation error", MB_ICONSTOP);
      return -1;
    }

  if (!WideCharToMultiByte(CP_ACP, 0, wszArgv[2], -1, funcname, funcnamelen,
			   NULL, NULL))
    {
      MessageBox(NULL, win_error,
		 "Wide to multibyte character conversion error", MB_ICONSTOP);
      return -1;
    }

  hLib = LoadLibraryW(wszArgv[1]);
  if (hLib == NULL)
    {
      MessageBox(NULL, win_error, "Error loading DLL", MB_ICONEXCLAMATION);
      return -1;
    }

  Func = (CallDLLWFunc*) GetProcAddress(hLib, funcname);
  if (Func == NULL)
    {
      MessageBox(NULL, win_error, "Function not found in DLL",
		 MB_ICONEXCLAMATION);
      return -1;
    }

  return Func(wszArgv[3]);
}

int
WinMainCRTStartup()
{
  int iArgc;
  LPWSTR *wszArgv = CommandLineToArgvW(GetCommandLineW(), &iArgc);
  if (wszArgv == NULL)
    {
      MessageBox(NULL, "This program requires Windows NT.", "CALLDLLW",
		 MB_ICONSTOP);
      ExitProcess((DWORD) -1);
    }

  ExitProcess(wmain(iArgc, wszArgv));
}
