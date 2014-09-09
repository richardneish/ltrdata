#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

LPSTR
WideToByteAlloc(LPCWSTR lpSrc)
{
  LPSTR lpDst;
  int iReqSize =
    WideCharToMultiByte(CP_ACP, 0, lpSrc, -1, NULL, 0, NULL, NULL);
  if (iReqSize == 0)
    return NULL;

  lpDst = (LPSTR) halloc(iReqSize);
  if (lpDst == NULL)
    return NULL;

  if (WideCharToMultiByte(CP_ACP, 0, lpSrc, -1, lpDst, iReqSize, NULL, NULL)
      != iReqSize)
    {
      hfree(lpDst);
      return NULL;
    }

  return lpDst;
}
