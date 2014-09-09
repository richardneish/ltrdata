#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <shellapi.h>
#include <wfind.h>
#include <wntsecur.h>
#include <stdlib.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")
#ifndef _WIN64
#pragma comment(lib, "crtdll.lib")
#endif

#define si (OWNER_SECURITY_INFORMATION|GROUP_SECURITY_INFORMATION|\
   DACL_SECURITY_INFORMATION|SACL_SECURITY_INFORMATION)

void *pSecurityBuffer = NULL;
DWORD dwSecurityBufferSize = 0;

bool ReadSecurity(LPCWSTR);
bool WriteSecurity(LPCWSTR);

enum XError
{
  XE_NOERROR,
  XE_CANCELLED,
  XE_COPYFILE,
  XE_CREATEDIR,
  XE_SECURITYREAD,
  XE_SECURITYWRITE,
  XE_CHDIR,
  XE_CHDIRBACK,
  XE_NOT_WINDOWSNT,
  XE_NOT_ENOUGH_MEMORY
};

void
status_exit(XError XE)
{
  const char *errmsg = NULL;
  switch(XE)
    {
    case XE_CANCELLED:
      errmsg = "\r\nCopyACLs cancelled.\r\n";
      break;
    case XE_SECURITYREAD:
      errmsg = "\r\nRead of security descriptor failed. CopyACLs aborted.\r\n";
      break;
    case XE_SECURITYWRITE:
      errmsg = "\r\nFile security descriptor change failed. "
	"CopyACLs aborted.\r\n";
      break;
    case XE_NOT_WINDOWSNT:
      MessageBoxA(NULL, "This program requires Windows NT.", "CopyACLs",
		  MB_ICONERROR);
      exit(XE);
    }
      
  if (errmsg)
    fputs(errmsg, stderr);
      
  exit(XE);
}

int
wmain(int argc, LPWSTR *argv)
{
  if (argc < 3)
    {
      puts("Usage: CopyACLs source target1 [target2 ...]");
      return 0;
    }

  SetOemPrintFLineLength(GetStdHandle(STD_ERROR_HANDLE));

  HANDLE hToken;
  if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
      TOKEN_PRIVILEGES TP;
      TP.PrivilegeCount = 1;
      LookupPrivilegeValue(L"", SE_SECURITY_NAME, &(TP.Privileges[0].Luid));
      TP.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

      AdjustTokenPrivileges(hToken, FALSE, &TP, NULL, NULL, NULL);
    }

  if (ReadSecurity(argv[1]))
    while (--argc > 1)
      WriteSecurity((++argv)[1]);
      
  printf("\nCopyACLs done. Security descriptor size: %u bytes.\n",
	 dwSecurityBufferSize);

  return XE_NOERROR;
}

bool
ReadSecurity(LPCWSTR source)
{
  DWORD dwSDSizeNeeded = 0;
  while (!((WSecurityDescriptor*)pSecurityBuffer)->
	 FromFile(source, si, dwSecurityBufferSize, &dwSDSizeNeeded))
    switch (win_errno)
      {
      case ERROR_INSUFFICIENT_BUFFER:
	if (pSecurityBuffer)
	  hfree(pSecurityBuffer);

	pSecurityBuffer = halloc(dwSDSizeNeeded);
	if (pSecurityBuffer == NULL)
	  status_exit(XE_NOT_ENOUGH_MEMORY);

	dwSecurityBufferSize = dwSDSizeNeeded;
	
	continue;

      case ERROR_CALL_NOT_IMPLEMENTED:
	status_exit(XE_NOT_WINDOWSNT);

      default:
	WErrMsgA errmsg;
	oem_printf(stderr, 
		   "Error reading security descriptor from '%1!ws!': %2%%n",
		   source, (LPCSTR) errmsg);
	status_exit(XE_SECURITYREAD);
      }

  return true;
}

bool
WriteSecurity(LPCWSTR target)
{
  if( ((WSecurityDescriptor*) pSecurityBuffer)->ToFile(target, si) )
    return true;

  WErrMsgA errmsg;
  oem_printf(stderr,
	     "Error writing security descriptor to '%1!ws!': %2%%n",
	     target, (LPCSTR) errmsg);
  return false;
}

extern "C" int
wmainCRTStartup()
{
  int argc = 0;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
  if (argv == NULL)
    status_exit(XE_NOT_WINDOWSNT);
  else
    exit(wmain(argc, argv));
}
