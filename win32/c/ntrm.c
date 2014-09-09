#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE
#ifndef _DLL
#define _DLL
#endif
#include <windows.h>
#include <shellapi.h>
#include <ntdll.h>

#ifdef _WIN64
#include <stdlib.h>
#else
#include <minwcrtlib.h>
#endif

#include <winstrct.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "shell32.lib")
#ifndef _WIN64
#pragma comment(lib, "crtdll.lib")
#endif
#pragma comment(lib, "ntdll.lib")

int
wmain(int argc, LPWSTR *argv)
{
  UNICODE_STRING NtPath;
  OBJECT_ATTRIBUTES objattr;
  NTSTATUS stat;

  SetOemPrintFLineLength(GetStdHandle(STD_ERROR_HANDLE));

  if (argc < 2)
    {
      fprintf(stderr, "Usage:\r\n%ws file [...]\r\n", argv[0]);
      return 1;
    }

  while (--argc)
    {
      if ((++argv)[0][0] == '\0')
	continue;

      if (argv[0][0] == L'\\')
	stat = RtlCreateUnicodeString(&NtPath, argv[0]);
      else
	stat = RtlDosPathNameToNtPathName_U(argv[0], &NtPath, NULL, NULL);

      if (!NT_SUCCESS(stat))
	{
	  LPSTR szErrMsg = nt_errmsgA(stat);
	  oem_printf(stderr, "Error: '%1!ws!': %2%%n",
		     argv[0],
		     szErrMsg);
	  LocalFree(szErrMsg);
	  return 1;
	}

      InitializeObjectAttributes(&objattr, &NtPath, OBJ_CASE_INSENSITIVE, NULL,
				 NULL);

      stat = NtDeleteFile(&objattr);

      if (NT_SUCCESS(stat))
	oem_printf(stderr, "Success: '%1!ws!'%%n", argv[0]);
      else
	{
	  LPSTR szErrMsg = nt_errmsgA(stat);
	  oem_printf(stderr, "Error: '%1!ws!': %2%%n",
		     argv[0],
		     szErrMsg);
	  LocalFree(szErrMsg);
	}

      RtlFreeUnicodeString(&NtPath);
    }

  return 0;
}

__declspec(noreturn)
void
wmainCRTStartup()
{
  int argc = 0;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
  if (argv == NULL)
    {
      MessageBoxA(NULL, "This program requires Windows NT.",
		  "Incompatible Windows version", MB_ICONEXCLAMATION);
      ExitProcess((UINT) -1);
    }
  else
    exit(wmain(argc, argv));
}
