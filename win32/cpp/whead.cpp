#ifdef _WIN32

#  define WIN32_LEAN_AND_MEAN
#  include <stdio.h>
#  include <stdlib.h>
#  include <winstrct.h>
#  include <winsock2.h>
#  include <wio.h>
#  include <wthread.hpp>
#  include <wsocket.hpp>
#  include <tcproute.hpp>

#  pragma comment(lib, "wsock32.lib")

#  if defined(_DLL) && !defined(_WIN64)
#    pragma comment(lib, "minwcrt")
#  endif

#else // _WIN32

#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>
#  include <strings.h>
#  include <unistd.h>
#  include <errno.h>

#  include "iohlp.h"
#  include "sockhlp.h"

// Windows-like function and type names
#  define strnicmp strncasecmp
#  define stricmp strcasecmp
#  define closesocket close
#  define h_perror perror
#  define win_errno errno
typedef int SOCKET;
typedef int HANDLE;
typedef unsigned long DWORD;
#  define MAX_PATH 260
#  ifndef INADDR_NONE
#     define INADDR_NONE (-1L)
#  endif
#  define INVALID_SOCKET (-1)
#  define INVALID_HANDLE_VALUE (-1)
#  define SOCKET_ERROR (-1)
#  define Sleep(n) sleep(n)
#  define win_perror perror
#  define WriteFile(a,b,c,d,e) write(a,b,c)
#  define CloseHandle close
#  define hStdIn  (0)
#  define hStdOut (1)
#  define hStdErr (2)

#endif

#define BUFFER_SIZE 512

int
main(int argc, char **argv)
{
  char *host;
  char *port;
  char *filepath;
  if (argc == 4)
    {
      host = argv[1];
      port = argv[2];
      filepath = argv[3];
    }
  else if (argc == 3)
    {
      host = argv[1];
      port = "80";
      filepath = argv[2];
    }
  else if (argc == 2)
    {
      char *protdelim = strstr(argv[1], "://");

      if (protdelim == NULL)
	host = argv[1];
      else
	{
	  host = protdelim + 3;
	  *protdelim = 0;
	  if (stricmp(argv[1], "http") != 0)
	    {
	      printf("Invalid URL. Only http is supported.\r\n");
	      return 10;
	    }
	}

      char *filepathstart = strchr(host, '/');

      if (filepathstart == NULL)
	filepath = "/";
      else
	{
	  filepath = strdup(filepathstart);
	  *filepathstart = 0;
	}

      host = strtok(host, ":");
      port = strtok(NULL, "");
      if (port == NULL)
	port = "80";
    }
  else
    {
      printf("Utility to get server headers for a file on a web server\n"
	     "Copyright (C) Olof Lagerkvist 2011\n"
	     "\n"
	     "Usage:\n"
	     "\n"
	     "whead host [port] filepath\n"
	     "whead url\n");
      return 10;
    }

#ifdef _WIN32
  WSADATA wd;
  if (WSAStartup(0x0101, &wd))
    {
      win_perror();
      return -1;
    }
#endif

  SOCKET sConnection = ConnectTCP(host, port);
  if (sConnection == INVALID_SOCKET)
    {
      h_perror("Connection error");
      closesocket(sConnection);
      return 1;
    }

  HANDLE hConnection = (HANDLE) sConnection;

  if (!StrSend(hConnection, "HEAD "))
    {
      h_perror("Error sending request to server");
      closesocket(sConnection);
      return 2;
    }

  if (!StrSend(hConnection, filepath))
    {
      h_perror("Error sending request to server");
      closesocket(sConnection);
      return 2;
    }

  if (!StrSend(hConnection, " HTTP/1.0\r\nConnection: close\r\nHost: "))
    {
      h_perror("Error sending request to server");
      closesocket(sConnection);
      return 2;
    }

  if (!StrSend(hConnection, host))
    {
      h_perror("Error sending request to server");
      closesocket(sConnection);
      return 2;
    }

  if (!StrSend(hConnection, "\r\n\r\n"))
    {
      h_perror("Error sending request to server");
      closesocket(sConnection);
      return 2;
    }

  char recvbuf[BUFFER_SIZE];
  for (;;)
    {
      DWORD dwRecvLength = BufRecv(hConnection, recvbuf, sizeof recvbuf);
      if (dwRecvLength == 0)
	break;

      if (!BufSend(hStdOut, recvbuf, dwRecvLength))
	{
	  win_perror("Error writing to stdout");
	  closesocket(sConnection);
	  return 3;
	}

      Sleep(0);
    }

  switch (win_errno)
    {
#ifdef _WIN32
    case ERROR_HANDLE_EOF:
#endif
    case 0:
      return 0;

    default:
      win_perror("Error receiving from server");
    }

  return 0;
}
