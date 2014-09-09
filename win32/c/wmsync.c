#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <io.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int msync(void *addr, size_t len, int flags)
{
  if ((flags & MS_ASYNC) | (flags & MS_INVALIDATE))
    {
      errno = ENOSYS;
      return -1;
    }

  if (FlushViewOfFile(addr, len))
    return 0;

  switch (GetLastError())
    {
    case ERROR_CALL_NOT_IMPLEMENTED:
      errno = ENOSYS;
      return -1;
    case ERROR_INVALID_HANDLE:
      errno = EBADF;
      return -1;
    case ERROR_ACCESS_DENIED:
      errno = EACCES;
      return -1;
    default:
      errno = EINVAL;
      return -1;
    }
}
