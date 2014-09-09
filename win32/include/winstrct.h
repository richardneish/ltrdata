/* winstrct.h
 *
 * Defines Windows "Unix-like" error number and error message variables and
 * functions.
 * C++: Encapsulates in C++ objects some Windows API get/set function pairs.
 * Written by Olof Lagerkvist 2000-2005.
 */

#ifndef _INC_WINSTRCT_
#define _INC_WINSTRCT_

#ifndef _INC_WINDOWS_
# include <windows.h>
#endif

#ifndef _USERENTRY
#  define _USERENTRY
#endif

// The inlines here uses the stream I/O library functions.
#ifndef _INC_STDIO
#  include <stdio.h>
#endif

#include <tchar.h>

#ifndef EXTERN_C
#  ifdef __cplusplus
#     define EXTERN_C extern "C"
#  else
#     define EXTERN_C
#  endif
#endif

#define WINSOCK_MODULE TEXT("WSOCK32")
#define PDH_MODULE TEXT("PDH")
#define NTDLL_MODULE TEXT("NTDLL")

#ifdef _MSC_VER
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")

#pragma comment(lib, "winstrcp.lib")
#pragma comment(lib, "winstrct.lib")
#endif

// import/export macros
#define IMPORT __declspec(dllimport)
#define EXPORT __declspec(dllexport)

#ifdef __DLL__
# define DLL_EXTERN EXPORT
# define EXE_EXTERN IMPORT
#else
# define DLL_EXTERN IMPORT
# define EXE_EXTERN EXPORT
#endif

#if __BCPLUSPLUS__ > 0x0520
#pragma option push -vi
#endif

#ifndef INVALID_HKEY
#define INVALID_HKEY ((HKEY)INVALID_HANDLE_VALUE)
#endif

#ifndef WORD_MIN
#define WORD_MIN ((WORD)0x80000000)
#endif

#ifndef WORD_MAX
#define WORD_MAX ((WORD)0x7FFFFFFF)
#endif

#ifndef DWORD_MAX
#define DWORD_MAX ((DWORD)0xFFFFFFFF)
#endif

#ifndef LONGLONG_MIN
#define LONGLONG_MIN ((DWORDLONG)0x8000000000000000)
#endif

#ifndef LONGLONG_MAX
#define LONGLONG_MAX ((DWORDLONG)0x7FFFFFFFFFFFFFFF)
#endif

#ifndef DWORDLONG_MAX
#define DWORDLONG_MAX ((DWORDLONG)0xFFFFFFFFFFFFFFFF)
#endif

// Windows version macros
#define WinVer_Major       (GetVersion()&0x000000FF)
#define WinVer_Minor       ((GetVersion()&0x0000FF00)>>8)
#define WinVer_Build       ((GetVersion()&0x7FFF0000)>>16)

// Windows platform macros (boolean return values)
#define WinVer_WindowsNT   (!(GetVersion()&0x80000000))
#define WinVer_Windows95   (!!((GetVersion()&0x800000FF) >= 0x80000004))
#define WinVer_Win32s	   (!!(((GetVersion()^0x80000000)&0x800000FF) < 4))
#define WinVer_Win95orNT4  (WinVer_Major >= 4)
#define WinVer_Win32sor95  (!!(GetVersion()&0x80000000))
#define WinVer_Win32sorNT3 (WinVer_Major < 4)
#define WinVer_Win2korXP   (WinVer_Major >= 5)

#ifndef _QWORD_DEFINED
typedef DWORDLONG QWORD;
#define _QWORD_DEFINED
#endif

typedef struct _REPARSE_DATA_MOUNT_POINT
{
  DWORD ReparseTag;
  WORD ReparseDataLength;
  WORD Reserved;
  WORD NameOffset;
  WORD NameLength;
  WORD DisplayNameOffset;
  WORD DisplayNameLength;
  BYTE Data[65536];
} REPARSE_DATA_MOUNT_POINT, *PREPARSE_DATA_MOUNT_POINT;

/// Macros for "human readable" sizes in bytes
#define _1KB  (1ui64<<10)
#define _1MB  (1ui64<<20)
#define _1GB  (1ui64<<30)
#define _1TB  (1ui64<<40)

#define TO_B(n)  ((double)(n))
#define TO_KB(n) ((double)(n)/_1KB)
#define TO_MB(n) ((double)(n)/_1MB)
#define TO_GB(n) ((double)(n)/_1GB)
#define TO_TB(n) ((double)(n)/_1TB)

#define TO_h(n) ((n)>=_1TB ? TO_TB(n) : (n)>=_1GB ? TO_GB(n) :	\
	        (n)>=_1MB ? TO_MB(n) : (n)>=_1KB ? TO_KB(n) : (n))
#define TO_p(n) ((n)>=_1TB ? "TB" : (n)>=_1GB ? "GB" :			\
	        (n)>=_1MB ? "MB" : (n)>=_1KB ? "KB": (n)==1 ? "byte" : "bytes")

/// Prototype for rundll32.exe callable funtions.
typedef void WINAPI
RunDLLFunc(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow);

#define halloc_seh(n) HeapAlloc(GetProcessHeap(),HEAP_GENERATE_EXCEPTIONS,(n))
#define halloc(n) HeapAlloc(GetProcessHeap(), 0, (n))
#define hrealloc_seh(p, n) HeapReAlloc(GetProcessHeap(),		\
				       HEAP_GENERATE_EXCEPTIONS, (p), (n))
#define hrealloc(p, n) HeapReAlloc(GetProcessHeap(), 0, (p), (n))
#define hsize(p) HeapSize(GetProcessHeap(), 0, (p))
#define hfree(p) HeapFree(GetProcessHeap(), 0, (p))

#define SetLastErrorToNtStatus(s) SetLastError(RtlNtStatusToDosError(s))

#ifndef _WIN64
__inline long long __declspec(naked)
htonll()
{
  __asm
    {
      mov eax,[esp+4] ;
      mov edx,[esp+8] ;
      bswap eax ;
      bswap edx ;
      xchg eax,edx ;
      ret ;
    }
}
#endif

// Complement to the lstr*** API functions
__inline int
lstrncmpi(LPCTSTR lpString1, LPCTSTR lpString2, int iMaxLength)
{
  int iLength1 = lstrlen(lpString1);
  int iLength2 = lstrlen(lpString2);
  int iCompare = CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE|
			       SORT_STRINGSORT, lpString1,
			       iLength1<iMaxLength ? iLength1 : iMaxLength,
			       lpString2,
			       iLength2<iMaxLength ? iLength2 : iMaxLength);

  return iCompare - 2;
}

#ifdef  __cplusplus
extern "C" {
#endif
  extern
  int
  oem_printf_line_length;

__forceinline
void
SetOemPrintFLineLength(HANDLE hConsole)
{
  CONSOLE_SCREEN_BUFFER_INFO con_info;
  if (GetConsoleScreenBufferInfo(hConsole, &con_info))
    oem_printf_line_length =
      con_info.srWindow.Right - con_info.srWindow.Left - 2;
  else
    oem_printf_line_length = 0;
}
#ifdef  __cplusplus
}
#endif

EXTERN_C
BOOL
oem_printf(FILE *stream, LPCSTR lpMessage, ...);

/* win_error
 */
__inline LPSTR
win_errmsgA(DWORD dwErrNo)
{
  LPSTR errmsg = NULL;
  if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM |
		     FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, dwErrNo,
		     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		     (LPSTR)&errmsg, 0, NULL))
    return errmsg;
  else if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			  FORMAT_MESSAGE_FROM_STRING |
			  FORMAT_MESSAGE_ARGUMENT_ARRAY, "Error %1!i!%n", 0, 0,
			  (LPSTR)&errmsg, 0, (va_list*)&dwErrNo))
    return errmsg;
  else
    return NULL;
}
__inline LPWSTR
win_errmsgW(DWORD dwErrNo)
{
  LPWSTR errmsg = NULL;
  if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM |
		     FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, dwErrNo,
		     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		     (LPWSTR)&errmsg, 0, NULL))
    return errmsg;
  else if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			  FORMAT_MESSAGE_FROM_STRING |
			  FORMAT_MESSAGE_ARGUMENT_ARRAY, L"Error %1!i!%n", 0,
			  0, (LPWSTR)&errmsg, 0, (va_list*)&dwErrNo))
    return errmsg;
  else
    return NULL;
}
#ifdef UNICODE
#define win_errmsg win_errmsgW
#define win_error win_errmsgW(GetLastError())
#define h_error win_errmsgW(WSAGetLastError())
#else
#define win_errmsg win_errmsgA
#define win_error win_errmsgA(GetLastError())
#define h_error win_errmsgA(WSAGetLastError())
#endif

#define win_errorA win_errmsgA(GetLastError())
#define win_errorW win_errmsgW(GetLastError())
#define h_errorA win_errmsgA(WSAGetLastError())
#define h_errorW win_errmsgW(WSAGetLastError())

/* pdh_error
 */
__inline LPSTR pdh_errmsgA(DWORD dwErrorCode)
{
  LPSTR errmsg = NULL;
  if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM|
		     FORMAT_MESSAGE_FROM_HMODULE |
		     FORMAT_MESSAGE_ALLOCATE_BUFFER,
		     GetModuleHandle(PDH_MODULE), dwErrorCode,
		     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		     (LPSTR)&errmsg, 0, NULL))
    return errmsg;
  else if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			  FORMAT_MESSAGE_FROM_STRING |
			  FORMAT_MESSAGE_ARGUMENT_ARRAY, "Error %1!i!%n", 0, 0,
			  (LPSTR)&errmsg, 0, (va_list*)&dwErrorCode))
    return errmsg;
  else
    return NULL;
}
__inline LPSTR pdh_errmsgW(DWORD dwErrorCode)
{
  LPSTR errmsg = NULL;
  if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM|
		     FORMAT_MESSAGE_FROM_HMODULE |
		     FORMAT_MESSAGE_ALLOCATE_BUFFER,
		     GetModuleHandle(PDH_MODULE), dwErrorCode,
		     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		     (LPWSTR)&errmsg, 0, NULL))
    return errmsg;
  else if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			  FORMAT_MESSAGE_FROM_STRING |
			  FORMAT_MESSAGE_ARGUMENT_ARRAY, L"Error %1!i!%n", 0,
			  0, (LPWSTR)&errmsg, 0, (va_list*)&dwErrorCode))
    return errmsg;
  else
    return NULL;
}
#ifdef UNICODE
#define pdh_errmsg pdh_errmsgW
#define pdh_error pdh_errmsgW(GetLastError())
#else
#define pdh_errmsg pdh_errmsgA
#define pdh_error pdh_errmsgA(GetLastError())
#endif

#define pdh_errorA pdh_errmsgA(GetLastError())
#define pdh_errorW pdh_errmsgW(GetLastError())

#ifdef _NTDDK_
/* nt_error
 */
__inline LPSTR nt_errmsgA(NTSTATUS dwErrorCode)
{
  LPSTR errmsg = NULL;
  if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM|
		     FORMAT_MESSAGE_FROM_HMODULE |
		     FORMAT_MESSAGE_ALLOCATE_BUFFER,
		     GetModuleHandle(NTDLL_MODULE), dwErrorCode,
		     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		     (LPSTR)&errmsg, 0, NULL))
    return errmsg;
  else if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			  FORMAT_MESSAGE_FROM_STRING |
			  FORMAT_MESSAGE_ARGUMENT_ARRAY, "Error %1!i!%n", 0, 0,
			  (LPSTR)&errmsg, 0, (va_list*)&dwErrorCode))
    return errmsg;
  else
    return NULL;
}
__inline LPWSTR nt_errmsgW(NTSTATUS dwErrorCode)
{
  LPWSTR errmsg = NULL;
  if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM |
		     FORMAT_MESSAGE_FROM_HMODULE |
		     FORMAT_MESSAGE_ALLOCATE_BUFFER,
		     GetModuleHandle(NTDLL_MODULE), dwErrorCode,
		     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		     (LPWSTR)&errmsg, 0, NULL))
    return errmsg;
  else if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			  FORMAT_MESSAGE_FROM_STRING |
			  FORMAT_MESSAGE_ARGUMENT_ARRAY, L"Error %1!i!%n", 0,
			  0, (LPWSTR)&errmsg, 0, (va_list*)&dwErrorCode))
    return errmsg;
  else
    return NULL;
}
#ifdef UNICODE
#define nt_errmsg nt_errmsgW
#else
#define nt_errmsg nt_errmsgA
#endif
#endif // _NTDDK_

// Use Borland compiler's internal functions
#ifdef __BORLANDC__

# define lstrcmpA   __strcmp__
# define lstrcmpW   __wcscmp__

# define lstrcpyA   __strcpy__
# define lstrcpyW   __wcscpy__

#endif

__inline LPSTR
mprintfA(LPCSTR lpMsg, ...)
{
  va_list param_list;
  LPSTR lpBuf = NULL;

  va_start(param_list, lpMsg);

  if (!FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		      FORMAT_MESSAGE_FROM_STRING, lpMsg, 0, 0, (LPSTR) &lpBuf,
		      0, &param_list))
    return NULL;
  else
    return lpBuf;
}

__inline LPWSTR
mprintfW(LPCWSTR lpMsg, ...)
{
  va_list param_list;
  LPWSTR lpBuf = NULL;

  va_start(param_list, lpMsg);

  if (!FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		      FORMAT_MESSAGE_FROM_STRING, lpMsg, 0, 0, (LPWSTR) &lpBuf,
		      0, &param_list))
    return NULL;
  else
    return lpBuf;
}

#ifdef UNICODE
#define mprintf mprintfW
#else
#define mprintf mprintfA
#endif

EXTERN_C LPSTR
WideToOemAlloc(LPCWSTR lpSrc);

EXTERN_C LPSTR
WideToByteAlloc(LPCWSTR lpSrc);

EXTERN_C LPWSTR
OemToWideAlloc(LPCSTR lpSrc);

EXTERN_C LPWSTR
ByteToWideAlloc(LPCSTR lpSrc);

__inline int
wcscnt(LPCWSTR lp, WCHAR c)
{
  int o = 0;
  while ((lp = wcschr(lp, c)) != NULL)
    {
      lp++;
      o++;
    }
  return o;
}

// This function enables backup and restore priviliges for this process.
__inline
BOOL
EnableBackupPrivileges()
{
  HANDLE hToken;
  BYTE buf[sizeof(TOKEN_PRIVILEGES) * 2];
  TOKEN_PRIVILEGES *tkp = (TOKEN_PRIVILEGES *) buf;

  if (!OpenProcessToken(GetCurrentProcess(),
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    return FALSE;

  if (!LookupPrivilegeValue(NULL, SE_BACKUP_NAME, &tkp->Privileges[0].Luid))
    return FALSE;

  if (!LookupPrivilegeValue(NULL, SE_RESTORE_NAME, &tkp->Privileges[1].Luid))
    return FALSE;

  tkp->PrivilegeCount = 2;
  tkp->Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  tkp->Privileges[1].Attributes = SE_PRIVILEGE_ENABLED;

  return AdjustTokenPrivileges(hToken, FALSE, tkp, sizeof buf, NULL, NULL);
}

#ifdef __cplusplus

#include <winstrct.hpp>

#else /*_cplusplus*/

// Do the best possible if not C++
#  define ActiveWindow     (GetActiveWindow())
#  define CaptureWindow    (GetCapture())
#  define DoubleClickTime  (GetDoubleClickTime())
#  define FocusWindow      (GetFocus())
#  define ForegroundWindow (GetForegroundWindow())
#  define win_errno        (GetLastError())
#  define cmdlg_errno      (CommDlgExtendedError())
#  ifndef h_errno
#     define h_errno       (WSAGetLastError())
#  endif
#  define hStdIn           (GetStdHandle(STD_INPUT_HANDLE))
#  define hStdOut          (GetStdHandle(STD_OUTPUT_HANDLE))
#  define hStdErr          (GetStdHandle(STD_ERROR_HANDLE))

// Win32 version of kbhit()
#define lkbhit()           (WaitForSingleObject(hStdIn,0)==WAIT_OBJECT_0?1:0)

/* win_perror()
 *
 * Used similar to perror() in Unix environments.
 *
 * Used to print to stderr the error message associated with the error code of
 * the most recently non-sucessful call to a Windows API function. The error
 * message buffer is only used internally and is automatically freed by this
 * function.
 */
void win_perrorA(LPCSTR __errmsg);
void win_perrorW(LPCWSTR __errmsg);
#ifdef UNICODE
#define win_perror win_perrorW
#else
#define win_perror win_perrorA
#endif

/* h_perror()
 *
 */
void h_perrorA(LPCSTR __errmsg);
void h_perrorW(LPCWSTR __errmsg);
#ifdef UNICODE
#define h_perror h_perrorW
#else
#define h_perror h_perrorA
#endif

/* pdh_perror()
 *
 */
void pdh_perrorA(LPCSTR __errmsg);
void pdh_perrorW(LPCWSTR __errmsg);
#ifdef UNICODE
#define pdh_perror pdh_perrorW
#else
#define pdh_perror pdh_perrorA
#endif

#ifdef _NTDDK_
/* nt_perror()
 *
 */
void nt_perrorA(NTSTATUS status, LPCSTR __errmsg);
void nt_perrorW(NTSTATUS status, LPCWSTR __errmsg);
#ifdef UNICODE
#define nt_perror nt_perrorW
#else
#define nt_perror nt_perrorA
#endif
#endif // _NTDDK_

#endif /*_cplusplus*/

VOID WINAPI s_perror(LPCSTR szMsg, int iErrNo);

#endif //  !_INC_WINSTRCT_
