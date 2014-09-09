#define INCL_WINSOCK_API_TYPEDEFS 1
#include <winsock2.h>
#include <winstrct.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

char cBuf[4192];

int WINAPI WinMain(HINSTANCE hCur, HINSTANCE hPrev, LPSTR lpCmdLine, int iShow)
{
  WSADATA wsadata;
  int iWSAError;
  LPFN_WSASTARTUP pWSAStartup;
  HINSTANCE hDLL;
  LPSTR msg;

  if (lpCmdLine ? !lpCmdLine[0] : TRUE)
    {
      MessageBox(NULL, "WinSock Check (C) Olof Lagerkvist, 2002.\r\n\n"
		 "Example syntax: CHKWSDLL C:\\WINDOWS\\SYSTEM\\WSOCK32.DLL",
		 "WinSock Check", MB_ICONINFORMATION);
      return 4;
    }

  hDLL = LoadLibrary(lpCmdLine);
  if (hDLL == NULL)
    {
      MessageBox(NULL, win_error, "Error loading DLL", MB_ICONEXCLAMATION);
      return 3;
    }

  pWSAStartup = (LPFN_WSASTARTUP) GetProcAddress(hDLL, "WSAStartup");
  if (pWSAStartup == NULL)
    {
      MessageBox(NULL, "The module is not a 32-bit WinSock DLL.",
		 "WinSock Check Error", MB_ICONEXCLAMATION);
      return 2;
    }

  if (!GetModuleFileName(hDLL, cBuf, sizeof cBuf))
    msg = mprintf("Error getting full path to DLL:%n%1", win_error);
  else
    msg = mprintf("Module file: %1", cBuf);

  iWSAError = pWSAStartup(0x0070, &wsadata);
  if (iWSAError)
    msg = mprintf("%1%n"
		  "%n"
		  "WinSock API Startup failed:%n"
		  "%2",
		  msg, win_errmsg(iWSAError));
  else
    {
      msg = mprintf("%1%n"
		    "%n"
		    "Driver name: %2\r\n"
		    "Driver status: %3\r\n"
		    "Highest WinSock API version level supported: %4!u!.%5!u!",
		    msg,
		    wsadata.szDescription ?
		    wsadata.szDescription : "(Unknown)",
		    wsadata.szSystemStatus ? wsadata.szSystemStatus : "(None)",
		    (DWORD) (wsadata.wHighVersion & 0x00FF),
		    (DWORD) (wsadata.wHighVersion >> 8));

      // Prior to version 2.0, there were two additional fields.
      if ((wsadata.wVersion & 0x00FF) < 0x02)
	msg = mprintf("%1%n"
		      "Max sockets: %2!u!\r\n"
		      "Max UDP datagram size: %3!u!",
		      (DWORD) wsadata.iMaxSockets,
		      (DWORD) wsadata.iMaxUdpDg);
    }

  MessageBox(NULL, msg, "WinSock Check", MB_ICONINFORMATION);
  return 0;
}
