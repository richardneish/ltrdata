#define WIN32_LEAN_AND_MEAN

#include <wio.h>

#ifdef _MSC_VER
#pragma comment(lib, "kernel32")
#endif

//-- wio.h definitions
//--------------- SafeSend(), LineRecv(), BufRecv() and BufSend() functions are
//--------------- used for I/O on overlapped streams such as sockets, comm
//--------------- devices or named pipes.

EXTERN_C
VOID
CALLBACK
FileIoCompletionRoutine(DWORD dwErrorCode,
			DWORD dwNumberOfBytesTransfered,
			LPOVERLAPPED lpOverlapped)
{
  lpOverlapped->Internal = dwNumberOfBytesTransfered;
  lpOverlapped->InternalHigh = dwErrorCode;
  if (lpOverlapped->hEvent != NULL)
    SetEvent(lpOverlapped->hEvent);
}
