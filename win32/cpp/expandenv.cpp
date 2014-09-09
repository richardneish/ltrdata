#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <wfilemap.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "nullcrt")
#endif

int main()
{
  WMappedFile fmIn((HANDLE)hStdIn, FMAP_RO);

  if( !fmIn )
    {
      win_errno = fmIn.dwLastError;
      win_perror("stdin");
      return fmIn.dwLastError;
    }

  char cTooSmall;
  DWORD dwBytesNeeded = ExpandEnvironmentStrings((const char *)(void *)fmIn,
						 &cTooSmall, 0);

  char *pcBuffer = (char *)halloc_seh(dwBytesNeeded);

  DWORD dwBytesExpanded = (ExpandEnvironmentStrings((const char *)(void *)fmIn,
						    pcBuffer, dwBytesNeeded) >>
			   1) - 1;
  if( !dwBytesExpanded )
    {
      win_perror("ExpandEnvironmentStrings");
      return -1;
    }

  DWORD dwBytesWritten;
  if( !WriteFile(hStdOut, pcBuffer, dwBytesExpanded, &dwBytesWritten, NULL) )
    {
      win_perror("WriteFile");
      return -1;
    }

  return 0;
}
