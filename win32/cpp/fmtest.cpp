#define WIN32_LEAN_AND_MEAN

#include <stdio.h>

#include <winstrct.h>
#include <wfilemap.h>
#include <wconsole.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "nullcrt.lib")
#endif

int
main()
{
  WMappedFile fmIn((HANDLE)hStdIn, FMAP_RO);
  
  if (!fmIn)
    {
      WErrMsg errmsg(fmIn.dwLastError);
      CharToOem(errmsg, errmsg);
      fprintf(stderr, "stdin: %s\n", errmsg);
      return fmIn.dwLastError;
    }

  WMappedFile fmOut((HANDLE)hStdOut, FMAP_RW, fmIn.GetBytesMapped(), 0,
		    fmIn.GetBytesMapped());
  
  if (!fmOut)
    {
      WErrMsg errmsg(fmOut.dwLastError);
      CharToOem(errmsg, errmsg);
      fprintf(stderr, "stdout: %s\n", errmsg);
      return fmOut.dwLastError;
    }
  
  CopyMemory(fmOut, fmIn, fmIn.GetBytesMapped());
  
  return 0;
}

