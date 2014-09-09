#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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
