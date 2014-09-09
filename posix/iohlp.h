#ifndef _WIO_H
#define _WIO_H

/*
 * iohlp.h
 *
 * I/O help functions, *nix edition.
 * Copyright (C) Olof Lagerkvist
 */

#include <string.h>

#ifdef __cplusplus
#define ZERODEFAULT = 0
#ifndef BOOL
typedef bool BOOL;
#define TRUE true
#define FALSE false
#endif
#ifndef EXTERN_C
#define EXTERN_C extern "C"
#endif
#else
#define ZERODEFAULT
#ifndef BOOL
typedef int BOOL;
#define TRUE 1
#define FALSE 0
#endif
#ifndef EXTERN_C
#define EXTERN_C
#endif
#endif

EXTERN_C unsigned LineRecv(int h, char *pBuf, unsigned dwBufSiz,
   unsigned long dwTimeout ZERODEFAULT);
EXTERN_C unsigned BufRecv(int h, void *pBuf, unsigned dwBufSize,
   unsigned long dwTimeout ZERODEFAULT);
EXTERN_C BOOL BufSend(int h, const void *pBuf, unsigned dwBufSize,
   unsigned long dwTimeout ZERODEFAULT);

/// Send characters until null-terminator is reached.
#ifdef __cplusplus
inline bool StrSend(int h, const char *pBuf, unsigned long dwTimeout = 0)
{
   return BufSend(h, pBuf, strlen(pBuf), dwTimeout);
}
#else
#define StrSend(h,p,t) BufSend(h,p,strlen(p),t)
#endif

/*
BOOL ReadCommandLine(HANDLE hConn, LPSTR lpBuffer, DWORD dwBufferSize,
   HANDLE hEchoDevice = INVALID_HANDLE_VALUE, DWORD dwTimeOut = INFINITE);
*/

#endif

