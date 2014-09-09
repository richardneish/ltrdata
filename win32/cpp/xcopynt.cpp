// Compile for Windows NT ver 4.00 so that we can use CopyFileEx()
#define UNICODE
#define _UNICODE
#define _WIN32_WINNT 0x0400
#define WIN32_LEAN_AND_MEAN

#include <winstrct.h>
#include <shellapi.h>
#include <wfind.h>
#include <wntsecur.h>
#include <wconsole.h>

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")

#define asi (OWNER_SECURITY_INFORMATION|GROUP_SECURITY_INFORMATION|\
   DACL_SECURITY_INFORMATION|SACL_SECURITY_INFORMATION)

void *pSecurityBuffer = NULL;
DWORD dwSecurityBufferSize = 0;
bool bVerbose = true;

WCHAR source[32768] = L"";
WCHAR target[32768] = L"";

void xcopynt(LPWSTR, LPWSTR);
bool ReadACLSecurity(LPCWSTR);
bool WriteACLSecurity(LPCWSTR);

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
  XE_TOOLONGPATH,
  XE_NOT_ENOUGH_MEMORY
};

void
status_exit(XError XE)
{
  const char *errmsg = "";

  switch (XE)
    {
    case XE_CANCELLED:
      errmsg = "\r\nXCopyNT cancelled.\r\n";
      break;
    case XE_COPYFILE:
      errmsg = "\r\nFile copy error. XCopyNT aborted.\r\n";
      break;
    case XE_CREATEDIR:
      errmsg = "\r\nCannot create directory. XCopyNT aborted.\r\n";
      break;
    case XE_SECURITYREAD:
      errmsg =
	"\r\nRead of security descriptor failed. XCopyNT aborted.\r\n";
      break;
    case XE_SECURITYWRITE:
      errmsg =
	"\r\nFile security descriptor change failed. XCopyNT aborted.\r\n";
      break;
    case XE_CHDIR:
      errmsg = "\r\nCannot change to subdirectory. XCopyNT aborted.\r\n";
      break;
    case XE_CHDIRBACK:
      errmsg = "\r\nFatal file system error. XCopyNT aborted.\r\n";
      break;
    case XE_NOT_WINDOWSNT:
      MessageBoxA(NULL, "This program requires Windows NT.", "XCopyNT",
		  MB_ICONERROR);
      exit(XE);
    case XE_TOOLONGPATH:
      errmsg = "\r\nTarget path is too long.\r\n";
      break;
    }
  
  if (errmsg)
    fputs(errmsg, stderr);
  
  printf
    ("\nXCopyNT aborted. "
     "Largest security descriptor transferred was %u bytes.\n",
     dwSecurityBufferSize);

  exit(XE);
}

int
wmain(int argc, LPWSTR *argv)
{
  SetOemPrintFLineLength(GetStdHandle(STD_ERROR_HANDLE));

  if (argc == 3)
    if (wcsicmp(argv[1], L"/Q") == 0)
      {
	bVerbose = false;
	argc--;
	argv++;
      }

  if (argc != 2)
    {
      puts("Usage: XCOPYNT [/Q] targetdir");
      return 0;
    }

  HANDLE hToken;
  if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
      TOKEN_PRIVILEGES TP;
      TP.PrivilegeCount = 1;
      LookupPrivilegeValue(L"", SE_SECURITY_NAME, &(TP.Privileges[0].Luid));
      TP.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

      AdjustTokenPrivileges(hToken, FALSE, &TP, NULL, NULL, NULL);
    }

  source[0] = NULL;
  if (wcslen(argv[1]) >= sizeof(target)/sizeof(*target))
    status_exit(XE_TOOLONGPATH);

  wcscpy(target, argv[1]);

  LPWSTR trgptr = target + wcslen(target);
  if (trgptr[-1] != L'\\')
    if (trgptr[-1] == L':')
      {
	trgptr[0] = L'.';
	trgptr[1] = L'\\';
	trgptr[2] = NULL;
	trgptr += 2;
      }
    else
      {
	trgptr[0] = L'\\';
	trgptr[1] = NULL;
	trgptr++;
      }

  // Get existing directory's security descriptor
  WSecurityDescriptor *pwsdBuffer;
  if (!ReadACLSecurity(L"."))
    pwsdBuffer = NULL;
  else
    pwsdBuffer = (WSecurityDescriptor*) pSecurityBuffer;

  WSecurityAttributes wsa(false, pwsdBuffer);

  for (;;)
    {
      if (CreateDirectoryEx(L".", target, &wsa))
	{
	  WriteACLSecurity(target);

	  WIN32_FILE_ATTRIBUTE_DATA w32fad;

	  if (GetFileAttributesEx(L".", GetFileExInfoStandard, &w32fad))
	    {
	      HANDLE hDir = CreateFile(target, GENERIC_WRITE,
				       FILE_SHARE_READ | FILE_SHARE_WRITE,
				       NULL, OPEN_EXISTING,
				       FILE_FLAG_BACKUP_SEMANTICS, NULL);

	      if (hDir != INVALID_HANDLE_VALUE)
		SetFileTime(hDir, &w32fad.ftCreationTime,
			    &w32fad.ftLastAccessTime,
			    &w32fad.ftLastWriteTime);

	      CloseHandle(hDir);
	    }

	  break;
	}
      
      if (win_errno == ERROR_ALREADY_EXISTS)
	{
	  if (pwsdBuffer)
	    WriteACLSecurity(target);

	  WIN32_FILE_ATTRIBUTE_DATA w32fad;

	  if (GetFileAttributesEx(L".", GetFileExInfoStandard, &w32fad))
	    {
	      HANDLE hDir = CreateFile(target, GENERIC_WRITE,
				       FILE_SHARE_READ | FILE_SHARE_WRITE,
				       NULL, OPEN_EXISTING,
				       FILE_FLAG_BACKUP_SEMANTICS, NULL);
	      
	      if (hDir != INVALID_HANDLE_VALUE)
		SetFileTime(hDir, &w32fad.ftCreationTime,
			    &w32fad.ftLastAccessTime,
			    &w32fad.ftLastWriteTime);
	      
	      CloseHandle(hDir);
	    }
	  
	  break;
	}
      else
	{
	  WErrMsgA errmsg;
	  oem_printf(stderr, "Error creating directory '%1!ws!': %2%%n",
		     target, (LPCSTR) errmsg);
	  if (bVerbose)
	    {
	      printf("abort, retry, ignore, Ignore always? (a,r,i,I): ");
	      switch (getche())
		{
		case 'a':
		  status_exit(XE_CREATEDIR);
		case 'r':
		  continue;
		case 'I':
		  bVerbose = false;
		}
	    }

	  if (pwsdBuffer)
	    WriteACLSecurity(target);

	  break;
	}
    }

  xcopynt(source, trgptr);

  printf
    ("\nXCopyNT done. Largest security descriptor transferred was %u bytes.\n",
     dwSecurityBufferSize);

  return XE_NOERROR;
}

bool
ReadACLSecurity(LPCWSTR source)
{
  DWORD dwSDSizeNeeded = 0;
  while (!((WSecurityDescriptor*) pSecurityBuffer)->
	 FromFile(source, asi,
		  dwSecurityBufferSize,
		  &dwSDSizeNeeded))
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
	oem_printf
	  (stderr,
	   "Error reading security descriptor (ACL) from '%1!ws!': %2%%n",
	   source, (LPCSTR) errmsg);

	if (bVerbose)
	  {
	    printf("Continue anyway (yes/no/always)? (y/n/a): ");
	    switch (getche())
	      {
	      case 'a':
		bVerbose = false;
	      case 'y':
		break;
	      default:
		status_exit(XE_SECURITYREAD);
	      }
	  }
	return false;
      }

  return true;
}

bool
WriteACLSecurity(LPCWSTR target)
{
  if (((WSecurityDescriptor*) pSecurityBuffer)->ToFile(target, asi))
    return true;
  else
    {
      WErrMsgA errmsg;
      oem_printf(stderr,
		 "Error writing security descriptor (ACL) to '%1!ws!': %2%%n",
		 target, (LPCSTR) errmsg);

      if (bVerbose)
	{
	  printf("Continue anyway (yes/no/always)? (y/n/a): ");
	  switch (getche())
	    {
	    case 'a':
	      bVerbose = false;
	    case 'y':
	      break;
	    default:
	      status_exit(XE_SECURITYWRITE);
	    }
	}
      return false;
    }
}

void
xcopynt(LPWSTR src, LPWSTR dst)
{
  WFileFinder finddata(L"*");

  if (!finddata)
    return;

  do
    {
      if ((wcscmp(finddata.cFileName, L"..") == 0) |
	  (wcscmp(finddata.cFileName, L".") == 0))
	continue;

      if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
	  if (wcslen(finddata.cFileName) >=
	      (sizeof(source)/sizeof(*source)) - (src - source) - 2)
	    {
	      oem_printf(stderr,
			 "'%1!ws!\\%2!ws!': Path is too long.%%n",
			 source, finddata.cFileName);

	      status_exit(XE_TOOLONGPATH);
	    }

	  if (wcslen(finddata.cFileName) >=
	      (sizeof(target)/sizeof(*target)) - (dst - target) - 2)
	    {
	      oem_printf(stderr,
			 "'%1!ws!\\%2!ws!': Path is too long.%%n",
			 target, finddata.cFileName);

	      status_exit(XE_TOOLONGPATH);
	    }

	  wcscpy(src, finddata.cFileName);
	  wcscat(src, L"\\");
	  wcscpy(dst, finddata.cFileName);
	  wcscat(dst, L"\\");

	  oem_printf(stderr, "Creating directory '%1!ws!'%%n", target);

	  // Get existing directory's security descriptor
	  WSecurityDescriptor *pwsdBuffer;
	  if (!ReadACLSecurity(finddata.cFileName))
	    pwsdBuffer = NULL;
	  else
	    pwsdBuffer = (WSecurityDescriptor*) pSecurityBuffer;

	  WSecurityAttributes wsa(false, pwsdBuffer);

	  for (;;)
	    {
	      if (CreateDirectoryEx(finddata.cFileName, target, &wsa))
		{
		  WriteACLSecurity(target);

		  WIN32_FILE_ATTRIBUTE_DATA w32fad;

		  if (GetFileAttributesEx(finddata.cFileName,
					  GetFileExInfoStandard, &w32fad))
		    {
		      HANDLE hDir = CreateFile(target, GENERIC_WRITE,
					       FILE_SHARE_READ |
					       FILE_SHARE_WRITE, NULL,
					       OPEN_EXISTING,
					       FILE_FLAG_BACKUP_SEMANTICS,
					       NULL);

		      if (hDir != INVALID_HANDLE_VALUE)
			SetFileTime(hDir, &w32fad.ftCreationTime,
				    &w32fad.ftLastAccessTime,
				    &w32fad.ftLastWriteTime);

		      CloseHandle(hDir);
		    }

		  break;
		}

	      if (win_errno == ERROR_ALREADY_EXISTS)
		{
		  if (pwsdBuffer)
		    WriteACLSecurity(target);

		  WIN32_FILE_ATTRIBUTE_DATA w32fad;
		  
		  if (GetFileAttributesEx(L".", GetFileExInfoStandard,
					  &w32fad))
		    {
		      HANDLE hDir = CreateFile(target, GENERIC_WRITE,
					       FILE_SHARE_READ|
					       FILE_SHARE_WRITE,
					       NULL, OPEN_EXISTING,
					       FILE_FLAG_BACKUP_SEMANTICS,
					       NULL);
		      
		      if (hDir != INVALID_HANDLE_VALUE)
			SetFileTime(hDir, &w32fad.ftCreationTime,
				    &w32fad.ftLastAccessTime,
				    &w32fad.ftLastWriteTime);
		      
		      CloseHandle(hDir);
		    }
		  
		  break;
		}
	      else
		{
		  WErrMsgA errmsg;
		  oem_printf(stderr,
			     "Cannot create directory '%1!ws!': %2%%n",
			     target, (LPCSTR) errmsg);

		  if (bVerbose)
		    {
		      printf("abort, retry, ignore, Ignore always? "
			     "(a,r,i,I): ");
		      switch (getche())
			{
			case 'a':
			  status_exit(XE_CREATEDIR);
			case 'r':
			  continue;
			case 'I':
			  bVerbose = false;
			}
		    }

		  if (pwsdBuffer)
		    WriteACLSecurity(target);

		  break;
		}
	    }

	  if (!SetCurrentDirectory(finddata.cFileName))
	    {
	      if (!bVerbose)
		continue;

	      WErrMsgA errmsg;
	      oem_printf(stderr,
			 "Cannot change directory to '%1!ws!': %2%%n",
			 source, (LPCSTR) errmsg);

	      if (bVerbose)
		{
		  printf("Do you want to continue copying without this\n"
			 "directory tree (yes/no/always)? (y/n/a): ");
		  switch (getche())
		    {
		    case 'a':
		      bVerbose = false;
		    case 'y':
		      continue;
		    default:
		      status_exit(XE_CHDIR);
		    }
		}
	      else
		continue;
	    }

	  xcopynt(src + wcslen(src), dst + wcslen(dst));

	  if (!SetCurrentDirectory(L".."))
	    {
	      WErrMsgA errmsg;
	      oem_printf
		(stderr,
		 "Fatal error: Can't change back to directory '%1!ws!': %2%%n",
		 source, errmsg);

	      status_exit(XE_CHDIRBACK);
	    }
	}
      else
	{
	  if (wcslen(finddata.cFileName) >=
	      (sizeof(source)/sizeof(*source)) - (src - source) - 2)
	    {
	      oem_printf(stderr,
			 "'%1!ws!\\%2!ws!': Path is too long.%%n",
			 source, finddata.cFileName);

	      status_exit(XE_TOOLONGPATH);
	    }

	  if (wcslen(finddata.cFileName) >=
	      (sizeof(target)/sizeof(*target)) - (dst - target) - 2)
	    {
	      oem_printf(stderr,
			 "'%1!ws!\\%2!ws!': Path is too long.%%n",
			 target, finddata.cFileName);

	      status_exit(XE_TOOLONGPATH);
	    }

	  wcscpy(src, finddata.cFileName);
	  wcscpy(dst, finddata.cFileName);

	  oem_printf(stdout, "%1!ws!%%n", source);

	  int cancel = false;
	  DWORD dwCopyFlags = COPY_FILE_FAIL_IF_EXISTS;

	  while (!CopyFileEx(finddata.cFileName, target, NULL, NULL, &cancel,
			     dwCopyFlags))
	    if (win_errno == ERROR_FILE_EXISTS)
	      {
		oem_printf(stdout,
			   "The file '%1!ws!' already exists.%%n", target);

		if (bVerbose)
		  {
		    printf("replace, skip, abort, Skip always? (r,s,a,S): ");
		    switch (getche())
		      {
		      case 'r':
			dwCopyFlags = 0;
			continue;
		      case 'a':
			status_exit(XE_CANCELLED);
		      case 'S':
			bVerbose = false;
		      }
		  }

		break;
	      }
	    else
	      {
		WErrMsgA errmsg;
		oem_printf(stdout,
			   "Cannot copy '%1!ws!' to '%2!ws!': %3%%n",
			   source, target, (LPCSTR) errmsg);

		if (bVerbose)
		  {
		    printf("retry, skip, abort, Skip always? (r,s,a,S): ");
		    switch (getche())
		      {
		      case 'r':
			continue;
		      case 'a':
			status_exit(XE_COPYFILE);
		      case 'S':
			bVerbose = false;
		      }
		  }
		break;
	      }

	  if (ReadACLSecurity(finddata.cFileName))
	    WriteACLSecurity(target);
	}

      Sleep(0);
    }
  while (finddata.Next());
}

extern "C"
__declspec(noreturn)
void
wmainCRTStartup()
{
  int argc;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
  if (argv == NULL)
    {  
      MessageBoxA(NULL, "This program requires Windows NT.", "ntname",
		  MB_ICONSTOP);
      ExitProcess((UINT) -1);
    }
  else
    exit(wmain(argc, argv));
}
