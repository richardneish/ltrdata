#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

int oem_printf_line_length = 78;

BOOL
oem_printf(FILE *stream, LPCSTR lpMessage, ...)
{
  va_list param_list;
  LPSTR lpBuf = NULL;
  LPSTR lpBuf2 = NULL;

  va_start(param_list, lpMessage);

  if (!FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		      FORMAT_MESSAGE_FROM_STRING, lpMessage, 0, 0,
		      (LPSTR) &lpBuf, 0, &param_list))
    return FALSE;

  if (!FormatMessageA((oem_printf_line_length &
		       FORMAT_MESSAGE_MAX_WIDTH_MASK) |
		      FORMAT_MESSAGE_IGNORE_INSERTS |
		      FORMAT_MESSAGE_ALLOCATE_BUFFER |
		      FORMAT_MESSAGE_FROM_STRING, lpBuf, 0, 0,
		      (LPSTR) &lpBuf2, 0, NULL))
    {
      LocalFree(lpBuf);
      return FALSE;
    }

  LocalFree(lpBuf);

  CharToOemA(lpBuf2, lpBuf2);
  fprintf(stream, "%s", lpBuf2);
  LocalFree(lpBuf2);
  fflush(stream);
  return TRUE;
}
