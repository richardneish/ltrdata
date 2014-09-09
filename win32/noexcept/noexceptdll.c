#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#define DLLEXPORT __declspec(dllexport)
#define NORETURN __declspec(noreturn)

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ntdll.lib")

HMODULE hModDLL = NULL;

LONG
WINAPI
ExceptionFilter(LPEXCEPTION_POINTERS exception_pointers)
{
  char buf[256];
  char *msg;

  if (exception_pointers->ExceptionRecord->ExceptionCode ==
      EXCEPTION_ACCESS_VIOLATION)
    {
      _snprintf(buf, sizeof buf, "Access violation at %p %s %p.",
		exception_pointers->ExceptionRecord->ExceptionAddress,
		exception_pointers->ExceptionRecord->ExceptionInformation[0] ?
		"writing" : "reading",
		exception_pointers->ExceptionRecord->ExceptionInformation[1]);
      buf[sizeof(buf)-1] = 0;
      msg = "Access violation";
    }
  else
    {
      switch (exception_pointers->ExceptionRecord->ExceptionCode)
	{
	case EXCEPTION_BREAKPOINT:
	case EXCEPTION_SINGLE_STEP:
	  msg = "Breakpoint";
	  break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
	case EXCEPTION_PRIV_INSTRUCTION:
	  msg = "Illegal or privileged instruction";
	  break;
	case EXCEPTION_IN_PAGE_ERROR:
	  msg = "In-page error";
	  break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
	case EXCEPTION_FLT_OVERFLOW:
	case EXCEPTION_FLT_UNDERFLOW:
	  msg = "Division by zero or overflow";
	  break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
	  msg = "Cannot continue after exception";
	  break;
	case EXCEPTION_STACK_OVERFLOW:
	  msg = "Stack overflow";
	  break;
	default:
	  msg = "Unknown exception";
	}
      _snprintf(buf, sizeof buf, "%s at %p.", msg,
		exception_pointers->ExceptionRecord->ExceptionAddress);
      buf[sizeof(buf)-1] = 0;
    }

  if (exception_pointers->ExceptionRecord->ExceptionFlags &
      EXCEPTION_NONCONTINUABLE)
    {
      if (MessageBox(NULL, buf, "Non-continuable exception",
		     MB_ICONSTOP | MB_OKCANCEL | MB_SYSTEMMODAL) == MB_OK)
	ExitThread(-1);

      return EXCEPTION_CONTINUE_SEARCH;
    }

  switch (MessageBox(NULL, buf, "Exception",
		     MB_ICONSTOP | MB_ABORTRETRYIGNORE | MB_SYSTEMMODAL))
    {
    case IDRETRY:
      return EXCEPTION_CONTINUE_EXECUTION;
    case IDIGNORE:
      ExitThread(-1);
    }

  return EXCEPTION_CONTINUE_SEARCH;
}

BOOL WINAPI
DllMain(HINSTANCE hInstDLL, DWORD dwReason, LPVOID lpReserved)
{
  hModDLL = hInstDLL;
  return TRUE;
}

DWORD DLLEXPORT WINAPI
InstallHandler(DWORD dw)
{
  SetErrorMode(SEM_FAILCRITICALERRORS |
	       SEM_NOGPFAULTERRORBOX |
	       SEM_NOOPENFILEERRORBOX);
  SetUnhandledExceptionFilter(ExceptionFilter);

  if (dw > 0)
    MessageBox(NULL, "Successfully hooked application and installed exception "
	       "filter.", "noexcept.dll",
	       MB_ICONINFORMATION | MB_SETFOREGROUND);

  return dw;
}

VOID DLLEXPORT NORETURN WINAPI
UninstallHandler(DWORD dw)
{
  SetErrorMode(0);
  SetUnhandledExceptionFilter(NULL);

  if (dw > 0)
    MessageBox(NULL, "Exception handling restored to default mode.",
	       "noexcept.dll", MB_ICONINFORMATION | MB_SETFOREGROUND);

  FreeLibraryAndExitThread(hModDLL, dw);
}
