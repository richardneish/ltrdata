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

void *
mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off)
{
  HANDLE hMap;
  void *lpView;
  DWORD flProtect = 0;
  ULARGE_INTEGER Offset;
  DWORD dwDesiredAccess = 0;

  Offset.QuadPart = off;

  if (prot & PROT_EXEC)
    {
      flProtect = PAGE_EXECUTE;
      if (prot & PROT_READ)
	flProtect = PAGE_EXECUTE_READ;
      if (prot & PROT_WRITE)
	flProtect = PAGE_EXECUTE_READWRITE;
      if ((flags & MAP_PRIVATE) | (flags & MAP_COPY))
	flProtect = PAGE_EXECUTE_WRITECOPY;
    }
  else
    {
      if (prot & PROT_READ)
	flProtect = PAGE_READONLY;
      if (prot & PROT_WRITE)
	flProtect = PAGE_READWRITE;
      if ((flags & MAP_PRIVATE) | (flags & MAP_COPY))
	flProtect = PAGE_WRITECOPY;
    }

  if (flags & MAP_NORESERVE)
    flProtect |= SEC_RESERVE;
  if ((flags & MAP_RENAME) | (flags & MAP_INHERIT) | (flags & MAP_NOEXTEND) |
      (flags & MAP_HASSEMAPHORE))
    {
      errno = ENOSYS;
      return MAP_FAILED;
    }

  hMap = CreateFileMapping((HANDLE)_get_osfhandle(fd), NULL, flProtect, 0, 0,
			   NULL);
  if (hMap == NULL)
    switch (GetLastError())
      {
      case ERROR_CALL_NOT_IMPLEMENTED:
	errno = ENOSYS;
	return MAP_FAILED;
      case ERROR_INVALID_HANDLE:
	errno = EBADF;
	return MAP_FAILED;
      case ERROR_ACCESS_DENIED:
	errno = EACCES;
	return MAP_FAILED;
      case ERROR_NOT_ENOUGH_MEMORY:
	errno = ENOMEM;
	return MAP_FAILED;
      default:
	errno = EINVAL;
	return MAP_FAILED;
      }

  dwDesiredAccess = SECTION_ALL_ACCESS;
  if (~prot & PROT_READ)
    dwDesiredAccess &= ~SECTION_MAP_READ;
  if (~prot & PROT_WRITE)
      dwDesiredAccess &= ~SECTION_MAP_WRITE;
  if (~prot & PROT_EXEC)
      dwDesiredAccess &= ~SECTION_MAP_EXECUTE;
  if ((flags & MAP_PRIVATE) | (flags & MAP_COPY))
    dwDesiredAccess |= FILE_MAP_COPY;

  lpView = MapViewOfFileEx(hMap, dwDesiredAccess, Offset.HighPart,
			   Offset.LowPart, len, addr);
  if (lpView == NULL)
    {
      switch (GetLastError())
	{
	case ERROR_CALL_NOT_IMPLEMENTED:
	  errno = ENOSYS;
	  break;
	case ERROR_INVALID_HANDLE:
	  errno = EBADF;
	  break;
	case ERROR_ACCESS_DENIED:
	  errno = EACCES;
	  break;
	case ERROR_NOT_ENOUGH_MEMORY:
	  errno = ENOMEM;
	  break;
	default:
	  errno = EINVAL;
	  break;
	}

      CloseHandle(hMap);
      return MAP_FAILED;
    }

  CloseHandle(hMap);

  if ((flags & MAP_FIXED) && (lpView != addr))
    {
      UnmapViewOfFile(lpView);
      errno = ENOMEM;
      return MAP_FAILED;
    }

  return lpView;
}

#pragma warning(push)
#pragma warning(disable : 4100)
int munmap(void *addr, size_t len)
{
  if (UnmapViewOfFile(addr))
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
    case ERROR_NOT_ENOUGH_MEMORY:
      errno = ENOMEM;
      return -1;
    default:
      errno = EINVAL;
      return -1;
    }
}
#pragma warning(pop)
