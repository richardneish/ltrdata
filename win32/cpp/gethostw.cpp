#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>
#include <winsock.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "wsock32.lib")

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

int PASCAL
WinMain(HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int)
{
  int cou;
  WSADATA wd;

  if ((lpCmdLine[0] | 2) == '/')
    {
      MessageBox(NULL,
		 "GetHost for Win32 is free software by Olof Lagerkvist\r\n"
		 "http://www.ltr-data.se      olof@ltr-data.se\r\n"
		 "Please read gethostw.txt for more info.\r\n\n"
		 "Syntax:\r\n"
		 "gethostw [ip]\r\n\n"
		 "IP can be an IP number or host name. If omitted, the local host will be looked up.",
		 "About GetHost for Windows",
		 MB_ICONINFORMATION);
      return 0;
    }

  if (WSAStartup(0x0101, &wd))
    {
      MessageBox(NULL, "Socket error", "GetHost for Windows", MB_ICONSTOP);
      return -1;
    }

  char buf[260];
  char *hname;
  if (lpCmdLine[0] == NULL)
    {
      if (gethostname(buf, sizeof(buf) - 1))
	{
	  MessageBox(NULL, "Can't resolve local host name",
		     "GetHost for Windows", MB_ICONEXCLAMATION);
	  return 0;
	}
      hname = buf;
    }
  else
    hname = lpCmdLine;

  hostent *hent;
  unsigned long haddr = inet_addr(hname);
  if (haddr == INADDR_NONE)
    hent = gethostbyname(hname);
  else
    hent = gethostbyaddr((char *)&haddr, 4, 2);

  if (!hent)
    {
      MessageBox(NULL, "Host name lookup failed.", "GetHost for Windows",
		 MB_ICONEXCLAMATION);
      return -1;
    }

  char msgbuf[260], *msgidx = msgbuf;
  int iLen;
  iLen = _snprintf(msgidx, sizeof(msgbuf) - (msgidx - msgbuf), "Host:\t%s",
		   hent->h_name);
  if (iLen <= 0)
    return -1;
  msgidx += iLen;

  cou = 0;
  while (hent->h_aliases[cou])
    {
      iLen = _snprintf(msgidx, sizeof(msgbuf) - (msgidx - msgbuf),
		       "\nAlias:\t%s",
		       hent->h_aliases[cou]);
      if (iLen <= 0)
	return -1;
      msgidx += iLen;
      ++cou;
    }
  iLen = _snprintf(msgidx, sizeof(msgbuf) - (msgidx - msgbuf), "\r\nIP:\t",
		   hent->h_addrtype);
  if (iLen <= 0)
    return -1;
  msgidx += iLen;

  cou = 0;
  while (hent->h_addr_list[cou])
    {
      unsigned char *chrptr = (unsigned char *)hent->h_addr_list[cou++];
      iLen = _snprintf(msgidx, sizeof(msgbuf) - (msgidx - msgbuf),
		       "%hu.%hu.%hu.%hu\t", chrptr[0], chrptr[1], chrptr[2],
		       chrptr[3]);
      if (iLen <= 0)
	return -1;
      msgidx += iLen;
    }

  MessageBox(NULL, msgbuf, "GetHost for Windows", MB_ICONINFORMATION);
  return 0;
}
