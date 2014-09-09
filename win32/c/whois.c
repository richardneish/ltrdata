/*  Universal-OS Freeware FINGER by Olof Lagerkvist 2000
 *  http://here.is/olof
 *
 *  Compiled on i386 with Windows NT 4.0 using Borland C++ 5.02 using the
 *  Winsock 2.0 library.
 *
 *  Compiled on i386 with RedHat Linux 5.1 using the GNU C++ compiler.
 *
 *  Compiled on Sun SPARCstation with SunOS 5.6 using the GNU C++ compiler
 *  with the libsocket.so.1 and libxnet.so.1 libraries.
 */

#include <stdio.h>
#include <string.h>

#ifdef _WIN32             // Windows
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   include <winsock.h>
#   pragma comment(lib, "user32.lib")
#   pragma comment(lib, "wsock32.lib")
#   ifdef _DLL
#      pragma comment(lib, "minwcrt.lib")
#   endif
#else                       // Unix
#   include <errno.h>
#   include <stdlib.h>
#   include <strings.h>
#   include <unistd.h>
#   include <netdb.h>
#   include <sys/types.h>
#   include <netinet/in.h>
#   include <sys/socket.h>
#   include <arpa/inet.h>
#   include <sys/wait.h>
#   define closesocket close
typedef int SOCKET;
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;
#endif

#ifndef TRUE
#   define TRUE 1
#endif

#ifndef FALSE
#   define FALSE 0
#endif

#ifndef _BOOL_DEFINED
typedef int BOOL;
#   define _BOOL_DEFINED
#endif

#ifndef INVALID_SOCKET
#   define INVALID_SOCKET -1
#endif

#ifndef INADDR_NONE
#   define INADDR_NONE -1
#endif

#ifndef SOCKET_ERROR
#   define SOCKET_ERROR -1
#endif

#define PACKET_SIZE 8192

const char stdserver[] = "whois.internic.net";

void usagemsg(char *appname);

char packet[PACKET_SIZE];

#ifdef _WIN32
void displayerrmsg(const char *errdsc);
#else
#define displayerrmsg(msg) perror(msg)
#endif

int main(int argc, char **argv)
{
  struct servent *service;
  SOCKET sd;
  BOOL userhostflag = FALSE;      // -h option
  const char *hname;
  unsigned long haddr;
  struct hostent *hent;
  SOCKADDR_IN addr;
  int msglen;
      
#ifdef _WIN32
  WSADATA wd;
  int en;
  if ((en = WSAStartup(0x0002, &wd)) != 0)
    {
      WSASetLastError(en);
      displayerrmsg("whois: WSAStartup");
      return -1;
    }
#endif

  // Get port name for service
  service = getservbyname("whois", "tcp");
  if (!service)
    {
      displayerrmsg("whois: Can't resolve service name");
      return -1;
    }

  // Check syntax
  if (argc > 3)
    if (strcmp( argv[1], "-h" ) == 0)
      {
        userhostflag = TRUE;
        argc--;
        argv++;
      }

  if (argc <= 1)
    {
      usagemsg(argv[0]);
      return -1;
    }

  do
    {
      if (userhostflag)
        {
          if (argc <= 2)       // We must have two parameters left if -h
            {                   // option is active
              return 0;
            }

          hname = argv[1];
          argv++;
          argc--;
        }
      else
        hname = stdserver;

      // Open socket
      if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
        {
          displayerrmsg("whois: socket");
          return -1;
        }

      // Get server address
      haddr = inet_addr(hname);

      if (haddr == INADDR_NONE)
        {
          hent = gethostbyname(hname);
          if (!hent)
            {
              fprintf(stderr, "whois: Host %s not found.\n", hname);
              closesocket(sd);
              continue;
            }

          hname = hent->h_name;
          haddr = *(unsigned long*)hent->h_addr;
        }

      fprintf(stdout, "\n[%s]\n", hname);

      addr.sin_family = AF_INET;
      addr.sin_port = service->s_port;
      addr.sin_addr.s_addr = haddr;
      memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

      if (connect(sd, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
        {
          displayerrmsg("whois: connect");
          closesocket(sd);
          continue;
        }

      *packet = 0;
      msglen = strlen(argv[1]);
      if (msglen > PACKET_SIZE - 2)
        {
          msglen = PACKET_SIZE - 2;
          argv[1][msglen] = '\x0';
        }

      strcpy(packet, argv[1]);
      packet[msglen++] = '\n';
      packet[msglen] = '\x0';

      if (send(sd, packet, msglen, 0) != msglen)
        {
          displayerrmsg("whois: send");
          closesocket(sd);
          continue;
        }

      while ((msglen = recv( sd, packet, PACKET_SIZE-1, 0 )) != 0)
        {
          if (msglen == SOCKET_ERROR)
            {
	      puts("");
	      displayerrmsg("whois: recv");
	      break;
	    }
          packet[msglen] = '\x0';
#ifdef _WIN32
	  CharToOem(packet, packet);
#endif
	  fputs(packet, stdout);
        }

      closesocket(sd);
    } while(argc > 1 ? ((--argc - 1) && (++argv)[1]) : FALSE);

  return 0;
}

void usagemsg(char *appname)
{
  printf("Freeware whois by Olof Lagerkvist 2000\r\n"
	 "http://www.ltr-data.se      olof@ltr-data.se\r\n"
	 "\n"
	 "Usage:\r\n"
	 "%s [-h server] domain [...]\r\n", appname);
}

#ifdef _WIN32
void displayerrmsg(const char *errdsc)
{
  char *errmsg = NULL;
  int win_herrno = WSAGetLastError();

  if (errdsc)
    if (*errdsc)
      fprintf(stderr, "%s: ", errdsc);

  if (FormatMessage(78 |
		    FORMAT_MESSAGE_FROM_SYSTEM |
		    FORMAT_MESSAGE_ALLOCATE_BUFFER,
		    NULL,
		    win_herrno,
		    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		    (LPSTR)&errmsg,
		    0,
		    NULL))
    {
      CharToOem(errmsg, errmsg);
      fprintf(stderr, "%s\n", errmsg);
      LocalFree(errmsg);
    }
  else
    fprintf(stderr, "Winsock error %i\n", win_herrno);
}
#endif
