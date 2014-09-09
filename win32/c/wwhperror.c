#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <winsock.h>
#include <winstrct.h>

#pragma comment(lib, "user32")

typedef VOID (WINAPI * s_perror_t)(LPCSTR,int);

/* h_perrorW()
 *
 */
void h_perrorW(LPCWSTR __errmsg)
{
  DWORD __h_errno = h_errno;
  LPSTR errmsg = NULL;
  HMODULE hMSWSOCKLib = NULL;
  s_perror_t s_perror;

  if (!FormatMessageA(FORMAT_MESSAGE_MAX_WIDTH_MASK |
		      FORMAT_MESSAGE_FROM_SYSTEM |
		      FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, __h_errno,
		      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		      (LPSTR)&errmsg, 0, NULL))
    {
      errmsg = NULL;
      hMSWSOCKLib = LoadLibrary("MSWSOCK.DLL");
      if (hMSWSOCKLib != NULL)
	{
	  s_perror = (s_perror_t)GetProcAddress(hMSWSOCKLib, "s_perror");
	  if (s_perror != NULL)
	    {
	      s_perror("", __h_errno);
	      FreeLibrary(hMSWSOCKLib);
	      return;
	    }
	  FreeLibrary(hMSWSOCKLib);
	}
    }

  if (__errmsg ? !!*__errmsg : FALSE)
    if (errmsg != NULL)
      oem_printf(stderr, "%1!ws!: %2%%n", __errmsg, errmsg);
    else
      oem_printf(stderr, "%1!ws!: Winsock error %2!u!%%n",
		 __errmsg, (DWORD) __h_errno);
  else
    if (errmsg != NULL)
      oem_printf(stderr, "%1%%n", errmsg);
    else
      fprintf(stderr, "Winsock error %u\n", (DWORD) __h_errno);

  if (errmsg != NULL)
    LocalFree(errmsg);
}
