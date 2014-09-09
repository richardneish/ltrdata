#include "iohlp.h"

/*
 * iohlp.cpp
 *
 * I/O help functions, *nix edition.
 * Copyright (C) Olof Lagerkvist
 */

#include <unistd.h>
#include <string.h>
#include <errno.h>

//-- iohlp.h definitions
//--------------- Safe send/recv procedures.

/// Read until end-of-line. Returns number of characters written in buffer.
EXTERN_C
unsigned
LineRecv(int h, char *pBuf, unsigned dwBufSize, unsigned long)
{
  unsigned dwLen = 0;

  for (char *ptr = pBuf; dwLen < dwBufSize - 2;)
    {
      if (read(h, ptr, 1) != 1)
	break;

      if (ptr[0] == '\r')
	continue;

      if (ptr[0] == '\n')
	break;

      dwLen++;
      ptr++;
    }

  pBuf[dwLen] = '\x00';
  return dwLen;
}

/// Send characters until null-terminator is reached.
EXTERN_C
unsigned
BufRecv(int h, void *pBuf, unsigned dwBufSize, unsigned long)
{
  unsigned dwDone = 0;

  for (char *ptr = (char *)pBuf; dwDone < dwBufSize;)
    {
      int iReadLen = read(h, ptr, dwBufSize - dwDone);

      if (iReadLen < 0)
	break;

      if (iReadLen == 0)
	{
	  errno = 0;
	  break;
	}

      dwDone += iReadLen;
      ptr += iReadLen;
    }

  return dwDone;
}

/// Send characters until null-terminator is reached.
EXTERN_C
 bool
BufSend(int h, const void *pBuf, unsigned dwBufSize, unsigned long)
{
  unsigned dwDone = 0;

  for (const char *ptr = (const char *)pBuf; dwDone < dwBufSize;)
    {
      int iWriteLen = write(h, ptr, dwBufSize - dwDone);

      if (iWriteLen == -1)
	break;

      dwDone += iWriteLen;
      ptr += iWriteLen;
    }

  return dwDone == dwBufSize;
}

/* Read until end-of-line and optionally echo typed characters to hEchoDevice
 * (OVERLAPPED). Set hEchoDevice to INVALID_HANDLE_VALUE to avoid echoing.
 * This function supports backspace functionality. dwTimeOut is the timeout
 * in ms, set to INFINITE to disable timeouting.
 */
/*
   Not implemented in Unix yet!!
BOOL ReadCommandLine(HANDLE hConn, LPSTR lpBuffer, DWORD dwBufferSize,
   HANDLE hEcho, DWORD dwTimeout)
{
   WOverlapped ol;
   if (ol.hEvent == INVALID_HANDLE_VALUE)
      return false;

   try
   {
      char *pBufptr = lpBuffer;

      for(;;)
      {
         Sleep(0);

         DWORD dwReadLen;
         if (!ReadFile(hConn, pBufptr, 1, &dwReadLen, &ol))
            if (GetLastError() != ERROR_IO_PENDING)
               break;

         if (WaitForSingleObject(ol.hEvent, dwTimeout) != WAIT_OBJECT_0)
            throw 1;

         if (!ol.GetResult(hConn, &dwReadLen))
            throw 1;

         if (dwReadLen != 1)
            throw 1;

         if (*pBufptr == '\n')
   	      break;

         //cout << (int)*pBufptr << ' ';

         switch (*pBufptr)
         {
            case '\r':
               continue;
      	   case 8: case 127:
   	   	   if (pBufptr > lpBuffer)
               {
                  char cBackspaceRollback[] = { 8, ' ', 8, 0 };
                  if (hEcho != INVALID_HANDLE_VALUE)
                     if (!SafeSend(hEcho, cBackspaceRollback))
                        throw 1;

            		--pBufptr;
               }
	            break;
            case 0:
         	   break;
   	      default:
               if (pBufptr >= lpBuffer+dwBufferSize)
                  continue;

               if (hEcho != INVALID_HANDLE_VALUE)
               {
                  char cEchoStr[] = { *pBufptr, 0 };
                  if (!SafeSend(hEcho, cEchoStr))
                     throw 1;
               }

         		++pBufptr;
         }
      }

      *pBufptr = NULL;

      if (hEcho != INVALID_HANDLE_VALUE)
         if (!SafeSend(hEcho, "\r\n"))
            throw 1;
   }
   catch(int)
   {
      Sleep(100);
      return false;
   }

   Sleep(0);
   return true;
}
*/
