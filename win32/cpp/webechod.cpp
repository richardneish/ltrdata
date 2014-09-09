#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <winstrct.h>
#include <wio.h>

#define MAX_LINE_LENGTH 1024

char cBuffer[8192];

#pragma comment(lib, "ws2_32.lib")

#pragma comment(linker, "/subsystem:windows")

EXTERN_C
__declspec(dllexport)
  DWORD WINAPI WebEcho(HANDLE s, LPCSTR)
{
  WOverlapped ol;

  const char http_head[] = "HTTP/1.0 200 OK\r\n"
      "Server: Web Echo Server\r\n"
      "Connection: close\r\n"
      "Pragma: no-cache\r\n"
      "Content-Type: text/plain\r\n"
      "\r\n";
      "You are connecting from:\n";

  if (!ol.BufSend(s, http_head, sizeof(http_head)-1))
    {
      h_perror();
      return (DWORD)-1;
    }

  // Get peer IP and name
  sockaddr_in sin;
  int sin_length = sizeof sin;
  if (getpeername((SOCKET)s, (sockaddr*)&sin, &sin_length) == SOCKET_ERROR)
    {
      h_perror();
      return (DWORD)-1;
    }
  hostent *hent = gethostbyaddr((char*)&sin.sin_addr, 4, 2);
  if (!hent)
    {
      const char cNoNameMsg[] = "Host name lookup failed.\n";
      if (!ol.BufSend(s, cNoNameMsg, sizeof(cNoNameMsg)-1))
        {
          h_perror();
          return (DWORD)-1;
        }
    }
  else
    {
      const char cHostMsg[] = "Host:\t";
      if (!ol.BufSend(s, cHostMsg, sizeof(cHostMsg)-1))
        {
          h_perror();
          return (DWORD)-1;
        }
      if (!ol.BufSend(s, hent->h_name, (DWORD) strlen(hent->h_name)))
        {
          h_perror();
          return (DWORD)-1;
        }
      if (!ol.BufSend(s, "\n", 1))
        {
          h_perror();
          return (DWORD)-1;
        }

      const char cAliasMsg[] = "Alias:\t";
      int cou = -1;
      while (hent->h_aliases[++cou])
        {
          Sleep(0);
          if (!ol.BufSend(s, cAliasMsg, sizeof(cAliasMsg)-1))
            {
              h_perror();
              return (DWORD)-1;
            }
          if (!ol.BufSend(s, hent->h_aliases[cou],
			  (DWORD) strlen(hent->h_aliases[cou])))
            {
              h_perror();
              return (DWORD)-1;
            }
          if (!ol.BufSend(s, "\n", 1))
            {
              h_perror();
              return (DWORD)-1;
            }
        }

      const char cIPMsg[] = "IP:\t";
      cou = 0;
      while (hent->h_addr_list[cou])
        {
          Sleep(0);
          if (!ol.BufSend(s, cIPMsg, sizeof(cIPMsg)-1))
            {
              h_perror();
              return (DWORD)-1;
            }
          char *IPAlias = inet_ntoa(*(in_addr*)hent->h_addr_list[cou++]);
          if (!ol.BufSend(s, IPAlias, (DWORD) strlen(IPAlias)))
            {
              h_perror();
              return (DWORD)-1;
            }
          if (!ol.BufSend(s, "\n", 1))
            {
              h_perror();
              return (DWORD)-1;
            }
        }
    }

  const char cHeaderMsg[] = "\nClient headers:\n";
  if (!ol.BufSend(s, cHeaderMsg, sizeof(cHeaderMsg)-1))
    {
      h_perror();
      return (DWORD)-1;
    }

  for(;;)
    {
      Sleep(0);

      char recvbuf[MAX_LINE_LENGTH];
      int size_recv = recv((SOCKET)s, recvbuf, sizeof(recvbuf), 0);
      if (size_recv == SOCKET_ERROR)
        {
          h_perror();
          return (DWORD)-1;
        }

      if (size_recv < 1)
        break;

      if (!ol.BufSend(s, recvbuf, size_recv))
        {
          h_perror();
          return (DWORD)-1;
        }
    }

  shutdown((SOCKET)s, 0x01);
  return 0;
}

