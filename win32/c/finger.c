/*  Universal-OS Freeware FINGER by Olof Lagerkvist 2000
 *  olof.lagerquist@data.ing.hb.se
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
#   include <winstrct.h>
#   pragma comment(lib, "user32")
#   pragma comment(lib, "wsock32")
#   if defined(_DLL) && !defined(_WIN64)
#       pragma comment(lib, "minwcrt")
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

char buf[160];
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
  char *hname = NULL;
  unsigned long haddr;
  struct hostent *hent;
  SOCKADDR_IN addr;
  int msglen;
      
#ifdef _WIN32
  WSADATA wd;
  int en;

  SetOemPrintFLineLength(GetStdHandle(STD_ERROR_HANDLE));

  if ((en = WSAStartup(0x0002, &wd)) != 0)
    {
      WSASetLastError(en);
      displayerrmsg("whois: WSAStartup");
      return -1;
    }
#endif

  // Get port name for service
  service = getservbyname("finger", "tcp");
  if (!service)
    {
      displayerrmsg("finger: Can't resolve service name");
      return -1;
    }

  // Check syntax
  if (argc > 1 ? ((argv[1][0] | 0x02) == '/') : FALSE)
    {
      usagemsg(argv[0]);
      return -1;
    }

  do
    {
      // Open socket
      if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
        {
          displayerrmsg("finger: socket");
          return -1;
        }

      // Get server address
      if (argc > 1 ? (hname = strchr(argv[1], '@')) != NULL ? (BOOL)hname[1] : FALSE : FALSE)
         *(hname++) = '\x0';
      else
        {
          if (hname)
            *hname = '\x0';

          if (gethostname(buf, sizeof(buf) - 1))
            {
              displayerrmsg("finger: Can't resolve local host name");
              closesocket(sd);
              continue;
            }
          hname = buf;
        }

      haddr = inet_addr( hname );

      if (haddr == INADDR_NONE)
        {
          hent = gethostbyname(hname);
          if (!hent)
            {
              fprintf(stderr, "finger: Host %s not found.\n", hname);
              closesocket(sd);
              continue;
            }

          hname = hent->h_name;
          haddr = *(unsigned long*)hent->h_addr;
        }

      fprintf(stdout, "[%s]\n", hname);

      addr.sin_family = AF_INET;
      addr.sin_port = service->s_port;
      addr.sin_addr.s_addr = haddr;
      memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

      if (connect(sd, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
        {
          displayerrmsg("finger: connect");
          closesocket(sd);
          continue;
        }

      *packet = 0;
      msglen = 1;
      if (argc > 1)
        {
          msglen = (int) strlen(argv[1]);
          if (msglen > PACKET_SIZE - 2)
            {
              msglen = PACKET_SIZE - 2;
              argv[1][msglen] = '\x0';
            }

          strcpy(packet, argv[1]);
          packet[msglen++] = '\n';
          packet[msglen] = '\x0';
        }

      if (send(sd, packet, msglen, 0) != msglen)
        {
          displayerrmsg("finger: send");
          closesocket(sd);
          continue;
        }

      while ((msglen = recv(sd, packet, PACKET_SIZE-1, 0)) != 0)
        {
          if (msglen == SOCKET_ERROR)
            {
	      puts("");
              displayerrmsg("finger: recv");
              break;
	    }
          packet[msglen] = '\x0';
          fputs(packet, stdout);
        }

      closesocket(sd);
    } while(argc > 1 ? ((--argc - 1) && (++argv)[1]) : FALSE);

  return 0;
}

void usagemsg(char *appname)
{
  printf("Freeware finger by Olof Lagerkvist 2000\n"
	 "http://www.ltr-data.se\n"
	 "\n"
	 "Usage:\n"
	 "%s [user][@host] [...]\n", appname);
}

#ifdef _WIN32
void displayerrmsg(const char *__errmsg)
{
  typedef VOID (WINAPI * s_perror_t)(LPCSTR,int);

  DWORD __h_errno = WSAGetLastError();
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
	      s_perror(__errmsg ? __errmsg : "", __h_errno);
	      FreeLibrary(hMSWSOCKLib);
	      return;
	    }
	  FreeLibrary(hMSWSOCKLib);
	}
    }

  if (__errmsg ? !!*__errmsg : FALSE)
    if (errmsg != NULL)
      oem_printf(stderr, "%1: %2\n", __errmsg, errmsg);
    else
      fprintf(stderr, "%s: Winsock error %u\n", __errmsg, (DWORD) __h_errno);
  else
    if (errmsg != NULL)
      oem_printf(stderr, "%1\n", errmsg);
    else
      fprintf(stderr, "Winsock error %u\n", (DWORD) __h_errno);

  if (errmsg != NULL)
    LocalFree(errmsg);
}
#endif
