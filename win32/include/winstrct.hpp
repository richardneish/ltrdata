/* winstrct.h
 *
 * Defines Windows "Unix-like" error number and error message variables and
 * functions.
 * C++: Encapsulates in C++ objects some Windows API get/set function pairs.
 * Written by Olof Lagerkvist 2000-2005.
 */

#ifndef _INC_WINSTRCT_HPP_
#define _INC_WINSTRCT_HPP_

#ifndef _INC_WINSTRCT_
#include <winstrct.h>
#endif

#if __BCPLUSPLUS__ > 0x0520
#pragma option push -Ve -Vx
#endif

/* ActiveWindow
 *
 * Used to get or set the active window. Gets the currently active window when
 * converted to HWND, sets a new active window when used with the assignment
 * operator =.
 *
 * To make sure it is not taken for a real variable, the address-of operator &
 * returns just type void, which is illegal in any assignment.
 */
extern const class _class_ActiveWindow
{
public:
  HWND operator=(const HWND &hWnd) const
  {
    return SetActiveWindow(hWnd);
  }
  void operator&() const{}
  operator HWND() const
  {
    return GetActiveWindow();
  }
} ActiveWindow;

/* CaptureWindow
 *
 * Used to get or set the mouse capture window. Gets the currently capture
 * window when converted to HWND, sets a new capture window when used with the
 * assignment operator =.
 *
 * To make sure it is not taken for a real variable, the address-of operator &
 * returns just type void, which is illegal in any assignment.
 */
extern const class _class_CaptureWindow
{
public:
  HWND operator=(const HWND &hWnd) const
  {
    return SetCapture(hWnd);
  }
  void operator&() const{}
  operator HWND() const
  {
    return GetCapture();
  }
} CaptureWindow;

extern const class _class_DoubleClickTime
{
public:
  BOOL operator=(const UINT &uInterval) const
  {
    return SetDoubleClickTime(uInterval);
  }
  void operator&() const{}
  operator UINT() const
  {
    return GetDoubleClickTime();
  }
} DoubleClickTime;

extern const class _class_FocusWindow
{
public:
  HWND operator=(const HWND &hWnd) const
  {
    return SetFocus(hWnd);
  }
  void operator&() const{}
  operator HWND() const
  {
    return GetFocus();
  }
} FocusWindow;

extern const class _class_ForegroundWindow
{
public:
  BOOL operator=(const HWND &hWnd) const
  {
    return SetForegroundWindow(hWnd);
  }
  void operator&() const{}
  operator HWND() const
  {
    return GetForegroundWindow();
  }
} ForegroundWindow;

/* hStdIn, hStdOut, hStdErr
 *
 * Used to get or set the standard input, output or error handle respectively.
 *
 * To make sure it is not taken for a real variable, the address-of operator &
 * returns just type void, which is illegal in any assignment.
 */
extern const class _class_hStdIn
{
public:
  HANDLE operator=(const HANDLE &hNew) const
  {
    if (SetStdHandle(STD_INPUT_HANDLE, hNew))
      return hNew;
    else
      return INVALID_HANDLE_VALUE;
  }
    
  void operator&() const{}
  
  operator HANDLE() const
  {
    return GetStdHandle(STD_INPUT_HANDLE);
  }
} hStdIn;

extern const class _class_hStdOut
{
public:
  HANDLE operator=( const HANDLE &hNew ) const
  {
    if( SetStdHandle( STD_OUTPUT_HANDLE, hNew ) )
      return hNew;
    else
      return INVALID_HANDLE_VALUE;
  }
  void operator&() const{}
  operator HANDLE() const
  {
    return GetStdHandle( STD_OUTPUT_HANDLE );
  }
} hStdOut;

extern const class _class_hStdErr
{
public:
  HANDLE operator=( const HANDLE &hNew ) const
  {
    if( SetStdHandle( STD_ERROR_HANDLE, hNew ) )
      return hNew;
    else
      return INVALID_HANDLE_VALUE;
  }
  void operator&() const{}
  operator HANDLE() const
  {
    return GetStdHandle( STD_ERROR_HANDLE );
  }
} hStdErr;

/* win_errno
 *
 * Used similar to errno in Unix environments.
 *
 * Used to get or set the error code from the most recent non-successful call
 * to a Windows API function. Gets the value when converted to DWORD, sets the
 * value when used with the assignment operator =.
 *
 * To make sure it is not taken for a real variable, the address-of operator &
 * returns just type void, which is illegal in any assignment.
 */
extern const class _class_win_errno
{
public:
  DWORD operator=( const DWORD& new_win_errno ) const
  {
    SetLastError( new_win_errno );
    return new_win_errno;
  }
  void operator&() const{}
  operator DWORD() const
  {
    return GetLastError();
  }
} win_errno;

// winsock.h and winsock2.h defines h_errno to be WSAGetLastError(). In this
// header, we provide a better way to implement h_errno
#	ifdef h_errno
#		undef h_errno
#	endif

#ifdef _WINSOCKAPI_
/* h_errno
 *
 * Used similar to h_errno in Unix environments.
 *
 * Used to get or set the error code from the most recently non-successful call
 * to a Winsock API function. Gets the value when converted to int, sets the
 * value when used with the assignment operator =.
 *
 * To make sure it is not taken for a real variable, the address-of operator &
 * returns just type void, which is illegal in any assignment.
 */
extern const class _class_h_errno
{
public:
  int operator=( const int& new_wsa_errno ) const
  {
    WSASetLastError( new_wsa_errno );
    return new_wsa_errno;
  }
  void operator&() const{}
  operator int() const
  {
    return WSAGetLastError();
  }
} h_errno;
#endif

#ifdef _INC_COMMDLG
/* cmdlg_errno
 *
 * Used to get the error code from the most recently called Windows Common
 * Dialog API function. Gets the value when converted to int, the value can't
 * be set.
 *
 * To make sure it is not taken for a real variable, the address-of operator &
 * returns just type void, which is illegal in any assignment.
 */
extern const class _class_cmdlg_errno
{
public:
  void operator&() const{}
  operator int() const
  {
    return CommDlgExtendedError();
  }
} cmdlg_errno;
#endif

#ifdef _WINNETWK_
/* wnet_errno
 *
 * Used to get the most recent error code with description reported by a
 * Windows network provider.
 */
extern const class _class_wnet_errno
{
public:
  void operator&() const{}
  operator DWORD() const
  {
    DWORD dwError = 0;
    TCHAR cBuf[2];

    WNetGetLastError(&dwError, cBuf, sizeof cBuf, cBuf, sizeof cBuf);

    return dwError;
  }
  DWORD operator()(LPTSTR pcErrorBuf = NULL, DWORD dwErrorBufSize = 0,
		   LPTSTR pcNameBuf = NULL, DWORD dwNameBufSize = 0)
  {
    DWORD  dwError;
    TCHAR cBuf[2];

    WNetGetLastError(&dwError,
		     pcErrorBuf && dwErrorBufSize ? pcErrorBuf : cBuf,
		     pcErrorBuf && dwErrorBufSize ? dwErrorBufSize :
		     sizeof cBuf,
		     pcNameBuf && dwNameBufSize ? pcNameBuf : cBuf,
		     pcNameBuf && dwNameBufSize ? dwNameBufSize : sizeof cBuf);

    return dwError;
  }
} wnet_errno;
#endif

class WEnvironmentStrings
{
public:
  operator LPTSTR()
  {
    return (LPTSTR)this;
  }

  void *operator new(size_t)
  {
    return GetEnvironmentStrings();
  }

  void operator delete(void *__this)
  {
    FreeEnvironmentStrings((LPTSTR)__this);
  }
};

struct WSystemInfo : public SYSTEM_INFO
{
  WSystemInfo()
  {
    GetSystemInfo(this);
  }
};

class WMem
{
protected:
  void *ptr;

public:
  operator bool()
  {
    return ptr != NULL;
  }

  bool operator!()
  {
    return ptr == NULL;
  }

  operator LPSTR()
  {
    return (LPSTR)ptr;
  }

  operator LPWSTR()
  {
    return (LPWSTR)ptr;
  }

  operator LPBYTE()
  {
    return (LPBYTE)ptr;
  }

  char& operator[](int i)
  {
    return ((char*)ptr)[i];
  }

  char *operator+(int i)
  {
    return (char*)ptr+i;
  }

  char *operator-(int i)
  {
    return (char*)ptr-i;
  }

  LPTSTR operator =(void *pBlk)
  {
    Free();
    return (LPTSTR)(ptr = pBlk);
  }

  DWORD_PTR GetSize() const
  {
    if (ptr == NULL)
      return 0;
    else
      return LocalSize(ptr);
  }

  /* WMem::ReAlloc()
   *
   * Note that this function uses LocalReAlloc() which makes it lose the
   * data if the block must be moved to increase.
   */
  LPTSTR ReAlloc(DWORD &dwAllocSize)
  {
    void *newblock = LocalReAlloc(ptr, dwAllocSize, LMEM_ZEROINIT);
    if (newblock)
      return (LPTSTR)(ptr = newblock);
    else
      return NULL;
  }

  LPTSTR Free()
  {
    return (LPTSTR)(ptr = LocalFree(ptr));
  }

  explicit WMem(DWORD dwAllocSize)
    : ptr(LocalAlloc(LPTR, dwAllocSize))
  {
  }

  explicit WMem(void *pBlk) : ptr(pBlk)
  {
  }

  ~WMem()
  {
    Free();
  }
};

class WHeapMem
{
protected:
  void *ptr;

public:
  operator bool()
  {
    return ptr != NULL;
  }

  bool operator!()
  {
    return ptr == NULL;
  }

  operator LPSTR()
  {
    return (LPSTR)ptr;
  }

  operator LPWSTR()
  {
    return (LPWSTR)ptr;
  }

  operator LPBYTE()
  {
    return (LPBYTE)ptr;
  }

  char& operator[](int i)
  {
    return ((char*)ptr)[i];
  }

  char *operator+(int i)
  {
    return (char*)ptr+i;
  }

  char *operator-(int i)
  {
    return (char*)ptr-i;
  }

  LPTSTR operator =(void *pBlk)
  {
    Free();
    return (LPTSTR)(ptr = pBlk);
  }

  DWORD_PTR GetSize() const
  {
    if (ptr == NULL)
      return 0;
    else
      return hsize(ptr);
  }

  /* WHeapMem::ReAlloc()
   *
   * This function uses HeapReAlloc() which makes it preserve the data if the
   * block must be moved to increase.
   */
  LPTSTR ReAlloc(DWORD &dwAllocSize)
  {
    void *newblock = hrealloc(ptr, dwAllocSize);
    if (newblock)
      return (LPTSTR)(ptr = newblock);
    else
      return NULL;
  }

  LPTSTR Free()
  {
    if (hfree(ptr))
      return NULL;
    else
      return (LPTSTR)ptr;
  }

  explicit WHeapMem(DWORD dwAllocSize) : ptr(halloc(dwAllocSize)) { }

  explicit WHeapMem(void *pBlk) : ptr(pBlk) { }

  ~WHeapMem()
  {
    Free();
  }
};

/* WErrMsg objects are auto-initialized with a buffer to win_error message or
 * any error message.
 *
 * The buffer is automatically freed by destructor.
 */
class WErrMsgA : public WMem
{
public:
  LPSTR operator =(DWORD dwErrno)
  {
    Free();
    return (LPSTR)(ptr = win_errmsgA(dwErrno));
  }

  WErrMsgA() : WMem(win_errmsgA(GetLastError())) { }

  explicit WErrMsgA(DWORD dwErrno) : WMem(win_errmsgA(dwErrno)) { }
};
class WErrMsgW : public WMem
{
public:
  LPWSTR operator =(DWORD dwErrno)
  {
    Free();
    return (LPWSTR)(ptr = win_errmsgW(dwErrno));
  }

  WErrMsgW() : WMem(win_errmsgW(GetLastError())) { }

  explicit WErrMsgW(DWORD dwErrno) : WMem(win_errmsgW(dwErrno)) { }
};
#ifdef UNICODE
#define WErrMsg WErrMsgW
#else
#define WErrMsg WErrMsgA
#endif

#ifdef _WINSOCKAPI_
/* WSockErrMsg completes the WErrMsg class with error messages from Windows
 * Socket 32-bit system.
 */
class WSockErrMsg : public WMem
{
public:
  LPTSTR operator =(DWORD dwErrno)
  {
    Free();
    return (LPTSTR)(ptr = win_errmsg(dwErrno));
  }

  WSockErrMsg() : WMem((LPTSTR)h_error) { }

  explicit WSockErrMsg(DWORD dwErrno) : WMem(win_errmsg(dwErrno)) { }
};
#endif

/* WPDHErrMsg completes the WErrMsg class with error messages from Performance
 * Data Helper module.
 */
class WPDHErrMsg : public WMem
{
public:
  LPTSTR operator =(DWORD dwErrno)
  {
    Free();
    return (LPTSTR)(ptr = pdh_errmsg(dwErrno));
  }

  WPDHErrMsg() : WMem((LPTSTR)pdh_error) { }

  explicit WPDHErrMsg(DWORD dwErrno) : WMem(pdh_errmsg(dwErrno)) { }
};

#ifdef _NTDDK_
/* WNTErrMsg completes the WErrMsg class with error messages from Windows NT
 * status codes.
 */
class WNTErrMsgA : public WMem
{
public:
  LPSTR operator =(NTSTATUS status)
  {
    Free();
    return (LPSTR)(ptr = nt_errmsgA(status));
  }

  explicit WNTErrMsgA(NTSTATUS status) : WMem(nt_errmsgA(status)) { }
};
class WNTErrMsgW : public WMem
{
public:
  LPWSTR operator =(NTSTATUS status)
  {
    Free();
    return (LPWSTR)(ptr = nt_errmsgW(status));
  }

  explicit WNTErrMsgW(NTSTATUS status) : WMem(nt_errmsgW(status)) { }
};
#ifdef UNICODE
#define WNTErrMsg WNTErrMsgW
#else
#define WNTErrMsg WNTErrMsgA
#endif
#endif

#ifdef UNICODE
class WMsgOEM : public WHeapMem
{
public:
  explicit WMsgOEM(WMem &mem);
};
#endif

/* win_perror()
 *
 * Used similar to perror() in Unix environments.
 *
 * Used to print to stderr the error message associated with the error code of
 * the most recently non-sucessful call to a Windows API function. The error
 * message buffer is only used internally and is automatically freed by this
 * function.
 */
EXTERN_C void win_perrorA(LPCSTR __errmsg = NULL);
EXTERN_C void win_perrorW(LPCWSTR __errmsg = NULL);
#ifdef UNICODE
#define win_perror win_perrorW
#else
#define win_perror win_perrorA
#endif

/* h_perror()
 *
 */
EXTERN_C void h_perrorA(LPCSTR __errmsg = NULL);
EXTERN_C void h_perrorW(LPCWSTR __errmsg = NULL);
#ifdef UNICODE
#define h_perror h_perrorW
#else
#define h_perror h_perrorA
#endif

/* pdh_perror()
 *
 */
EXTERN_C void pdh_perrorA(LPCSTR __errmsg = NULL);
EXTERN_C void pdh_perrorW(LPCWSTR __errmsg = NULL);
#ifdef UNICODE
#define pdh_perror pdh_perrorW
#else
#define pdh_perror pdh_perrorA
#endif

#ifdef _NTDDK_
/* nt_perror()
 *
 */
EXTERN_C void nt_perrorA(NTSTATUS status, LPCSTR __errmsg = NULL);
EXTERN_C void nt_perrorW(NTSTATUS status, LPCWSTR __errmsg = NULL);
#ifdef UNICODE
#define nt_perror nt_perrorW
#else
#define nt_perror nt_perrorA
#endif
#endif // _NTDDK_

#if __BCPLUSPLUS__ > 0x0520
#pragma option pop
#pragma option pop
#endif

#endif  // _INC_WINSTRCT_HPP_
