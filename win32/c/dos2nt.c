#define UNICODE
#define _UNICODE
#ifndef _DLL
#define _DLL
#endif
#define WIN32_LEAN_AND_MEAN
#include <minwcrtlib.h>
#include <winstrct.h>
#include <shellapi.h>
#include <ntdll.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "shell32.lib")
#ifndef _WIN64
#pragma comment(lib, "crtdll.lib")
#endif
#pragma comment(lib, "ntdll.lib")

int
wmain(int argc, LPWSTR *argv)
{
  int argcc = argc;

  SetOemPrintFLineLength(GetStdHandle(STD_ERROR_HANDLE));

  while (--argcc)
    {
      UNICODE_STRING NtName;
      ++argv;
      
      if (!RtlDosPathNameToNtPathName_U(argv[0], &NtName, NULL, NULL))
	oem_printf(stderr, "Invalid path: '%1!ws!'%%n", argv[0]);
      else
	{
	  if (argc > 2)
	    oem_printf(stdout,
		       "%1!ws! -> %2!.*ws!%%n",
		       argv[0],
		       NtName.Length / sizeof(*NtName.Buffer),
		       NtName.Buffer);
	  else
	    oem_printf(stdout,
		       "%1!.*ws!%%n",
		       NtName.Length / sizeof(*NtName.Buffer),
		       NtName.Buffer);

	  RtlFreeUnicodeString(&NtName);
	}
    }
}

int
wmainCRTStartup()
{
  int argc = 0;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
  if (argv == NULL)
    {
      MessageBoxA(NULL, "This program requires Windows NT.", "dos2nt",
		  MB_ICONERROR);
      ExitProcess((UINT) -1);
    }
  else
    exit(wmain(argc, argv));
}
