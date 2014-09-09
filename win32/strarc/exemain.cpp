/* Stream Archive I/O utility, Copyright (C) Olof Lagerkvist 2004-2014
 *
 * exemain.cpp
 * Main source file for strarc.exe. This contains the startup wmain() function.
 */

#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef _DLL
#define _DLL
#endif
#ifndef UNICODE
#define UNICODE
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Use the WinStructured library classes and functions.
#include <windows.h>
#include <intsafe.h>
#include <shellapi.h>
#include <ntdll.h>
#include <winstrct.h>

#include <wio.h>
#include <wntsecur.h>
#include <wprocess.h>
#include <wsync.h>

#include <stdlib.h>

#include "strarc.hpp"

StrArc *Session = NULL;

WCriticalSection *SessionProtect = NULL;

int
strarc_exception_handler(StrArc *Session, NTSTATUS ExceptionCode)
{
  const StrArc::StrArcExceptionData *ExceptionData =
    Session->GetExceptionData();

  if (ExceptionData->ErrorMessage != NULL)
    fputws(ExceptionData->ErrorMessage, stderr);

  if (ExceptionData->SysErrorMessage != NULL)
    if (ExceptionData->ObjectName != NULL)
      oem_printf(stderr, "%%n'%1!ws!': %2!ws!%%n",
		 ExceptionData->ObjectName,
		 ExceptionData->SysErrorMessage);
    else
      oem_printf(stderr, "%%n%1!ws!%%n",
		 ExceptionData->SysErrorMessage);
  else
    {
      WErrMsg errmsg(RtlNtStatusToDosError(ExceptionCode));
      oem_printf(stderr,
		 "%%nException 0x%1!X!: %2!ws!%%n",
		 ExceptionCode,
		 errmsg);
    }

  fprintf(stderr,
	  "\nstrarc aborted, %I64u file%s processed.\n",
	  Session->GetFileCount(),
	  Session->GetFileCount() != 1 ? "s" : "");

  return ExceptionData->ErrorCode != 0 ?
    (int) ExceptionData->ErrorCode : (int) ExceptionCode;
}

BOOL
WINAPI
ConsoleCtrlHandler(DWORD dwCtrlType)
{
  switch (dwCtrlType)
    {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_SHUTDOWN_EVENT:
      SessionProtect->Enter();

      if (Session != NULL)
	Session->bCancel = true;

      SessionProtect->Leave();

      return TRUE;

    default:
      return FALSE;
    }
}

int
__cdecl
wmain(int argc, LPWSTR *argv)
{
  SetOemPrintFLineLength(GetStdHandle(STD_ERROR_HANDLE));

  // This call enables backup and restore priviliges for this process. If
  // the priviliges are not granted that is silently ignored. Later file I/O
  // will give appropriate error returns anyway, if privileges were actually
  // needed.
  EnableBackupPrivileges();

  SessionProtect = new WCriticalSection;

  Session = new StrArc;

  SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);

  int rc;

  __try
  {
    rc = Session->Main(argc, argv);
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
    {
      rc = strarc_exception_handler(Session, GetExceptionCode());
    }

  SetConsoleCtrlHandler(ConsoleCtrlHandler, FALSE);

  SessionProtect->Enter();

  delete Session;

  Session = NULL;

  SessionProtect->Leave();

  return rc;
}

// This is enough startup code for this program if compiled to use the DLL CRT.
extern "C" int
wmainCRTStartup()
{
  int argc = 0;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
  if (argv == NULL)
    {
      MessageBoxA(NULL,
		  "This program requires Windows NT.",
		  "strarc",
		  MB_ICONERROR);
      ExitProcess((UINT) -1);
    }

  exit(wmain(argc, argv));
}
