#ifdef _WIN32
#       define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#       include <winsock.h>
#       pragma comment(lib, "wsock32.lib")
#       if defined(_DLL) && !defined(_WIN64)
#               pragma comment(lib, "minwcrt.lib")
#       endif
#else
#	include <netdb.h>
#	include <unistd.h>
#	include <netinet/in.h>
#	include <sys/socket.h>
#	include <arpa/inet.h>
typedef struct hostent HOSTENT;
typedef struct in_addr IN_ADDR;
#endif

#include <stdio.h>

#ifndef INADDR_NONE
#	define INADDR_NONE -1
#endif

int
main(int argc, char **argv)
{
  HOSTENT *hent;
  u_long haddr;
  char buf[260];
  char *hname;
  int cou = 0;
#ifdef _WIN32
  WSADATA wsa;
  if (WSAStartup(0x0101, &wsa))
    {
      fputs("Winsock initalization error.", stderr);
      return -1;
    }
#endif


  if (argv[1])
    if ((argv[1][0] | 2) == '/')
      {
	puts("GETHOST is free software by Olof Lagerkvist\r\n"
	     "http://www.ltr-data.se      olof@ltr-data.se\r\n"
	     "Please read GETHOST.TXT for more info.\r\n"
	     "\n"
	     "Syntax:\r\n"
	     "gethost [ip]\r\n"
	     "\n"
	     "IP can be an IP number or host name. If omitted, the\r\n"
	     "local host will be looked up.");
	
	return 0;
      }

  if (argv[1] == NULL)
    {
      if (gethostname( buf, sizeof(buf) - 1))
       {
         fputs("Error getting local host name\r\n", stderr);
         return 0;
       }
      hname = buf;
    }
  else
    hname = argv[1];
  
  haddr = inet_addr(hname);
  if (haddr == INADDR_NONE)
    hent = gethostbyname(hname);
  else
    hent = gethostbyaddr((char*)&haddr, 4, 2);
  
  if (!hent)
    {
      fputs("Host name lookup failed.\r\n", stderr);
      return -1;
    }

  printf("Host:\t%s\n", hent->h_name);

  while (hent->h_aliases[cou])
    printf("Alias:\t%s\n", hent->h_aliases[cou++]);

  printf("IP:\t");

  cou = 0;
  while (hent->h_addr_list[cou])
    printf("%s\t", inet_ntoa(*(IN_ADDR*)hent->h_addr_list[cou++]));

  puts("");

  return 0;
}
