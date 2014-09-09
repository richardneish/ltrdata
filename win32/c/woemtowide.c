#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

LPWSTR
OemToWideAlloc(LPCSTR lpSrc)
{
  LPWSTR lpDst;
  int iReqSize = MultiByteToWideChar(CP_OEMCP, 0, lpSrc, -1, NULL, 0);
  if (iReqSize == 0)
    return NULL;

  lpDst = (LPWSTR) halloc(iReqSize << 1);
  if (lpDst == NULL)
    return NULL;

  if (MultiByteToWideChar(CP_OEMCP, 0, lpSrc, -1, lpDst, iReqSize) != iReqSize)
    {
      hfree(lpDst);
      return NULL;
    }

  return lpDst;
}
