// Ursprungligen exempel av Martin Lagerqvist
// ML's webklient
// Ändrad till C++ av Olof Lagerkvist + tillägg för valfri GET-instruktion.
// Kompileras: g++ -lxnet -o webclient webclient.cpp
// Syntax: webclient www.ing.hb.se
#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <winsock.h>
#  include <winstrct.h>
#else
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <netdb.h>
#  include <strings.h>
#  include <unistd.h>
#  include <errno.h>
#  include <stdlib.h>
#
#  define SOCKET_ERROR -1
#  define INVALID_SOCKET -1
#endif

#ifdef _WIN32
#  include <minwcrtlib.h>
#  pragma comment(lib, "wsock32")
#  if defined(_DLL) && !defined(_WIN64)
#    pragma comment(lib, "minwcrt")
#  endif
#else
typedef SOCKET int;
#endif

// 32 K buffer size
char cBuf[32768];

int
main(int argc, char **argv)
{
#ifdef _WIN32
   WSADATA wsadata;
   if( WSAStartup( 0x0001, &wsadata ) )
     return -1;
#endif

  register SOCKET s;
  sockaddr_in sin;

  if ( argc < 2 )
    {
      puts("Usage: webclient address [path [port]]");
      exit(1);
    }
        
  // Tag info om dator 
  hostent *hp;
  if( (hp = gethostbyname(argv[1])) == NULL)
    {
      fprintf(stderr, "%s unknown host!\n", argv[1]);
      exit(1);
    }

  // Oppna socket.
  if( (s = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
    {
      fputs("Bad socket\n", stderr);
      exit(1); 
    }

  // Initiera struct, internet protokoll, port 80
  sin.sin_family = AF_INET;
  if (argc > 3)
    sin.sin_port = htons((u_short)strtoul(argv[3], NULL, 0));
  else
    sin.sin_port = htons(80);
#ifdef _WIN32
  CopyMemory(&sin.sin_addr, hp->h_addr, hp->h_length);
#else
  bcopy(hp->h_addr, &sin.sin_addr, hp->h_length);
#endif

  // Uppratta forbindelse
  if( connect(s, (sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
      fprintf(stderr, "Cannot connect to %s\n", argv[1]);
      exit(1);
    }

  // Skicka GET
  if(argc > 2)
    {
      send(s, "GET ", 4, 0);
      send(s, argv[2], (int) strlen(argv[2]), 0);
      send(s, " HTTP/1.0\n", 10, 0);
    }
  else
    send(s, "GET / HTTP/1.0\n", 15, 0);

  char c;

  while((c = (char)fgetc(stdin)) != EOF)
    send(s,&c,1,0);

  // Las och skriv HTTP-header...
  bool lastnewline = false;
  while(recv(s,&c,1,0) == 1)
    {
      if( c == '\r' )
         continue;

      if( (c == '\n') & lastnewline )
         break;
         
      lastnewline = c == '\n';
      fputc(c, stderr);
    }

#ifdef _WIN32
  HANDLE hOut = hStdOut;
#endif

  // Las och skriv HTTP-data...
  for(;;)
  {
    int i = recv(s,cBuf,sizeof cBuf,0);
    if (i<=0)
      break;

#ifdef _WIN32
    DWORD dw;
    WriteFile(hOut, cBuf, i, &dw, NULL);
    if (dw != (DWORD)i)
      break;
#else
    if (write(1,cBuf,i) != i)
      break;
#endif
  }

#ifdef _WIN32
  closesocket(s);
#else
  close(s); // Stang socket.
#endif
}

