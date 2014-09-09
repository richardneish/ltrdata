#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <winstrct.h>
#  include <winsock2.h>
#else
#  include <strings.h>
#  include <sys/types.h>
#  include <arpa/inet.h>
#  include <netinet/in.h>
#  include <sys/socket.h>
#  include <sys/wait.h>
#  include <sys/stat.h>
#  include <unistd.h>
#  include <fcntl.h>
#endif

#define closesocket      close

#if !defined(SOCKLEN_T_DEFINED) && !defined(HAVE_SOCKLEN_T)
   typedef int socklen_t;
#  define SOCKLEN_T_DEFINED 1
#endif

#ifndef _WINSOCKAPI_
#  define SOCKET           int
#  define HANDLE           int
#  define INVALID_SOCKET   -1
#  define SOCKET_ERROR     -1
#endif

#define MAX_LINE_LENGTH 1024
#define BACKLOG 10    /* how many pending connections queue will hold */

using namespace std;

SOCKET sockfd = INVALID_SOCKET;
SOCKET new_fd = INVALID_SOCKET;

const char *filename;
const char *mimetype;

void ConnectionHandler();
void siginthandler(int);

int main(int argc, char **argv, char **)
{

  if( (argc < 3) | (argc > 4) )
    {
      cout << argv[0] << " port file [mimetype]" << endl;
      return 0;
    }

  filename = argv[2];
  mimetype = argv[3] ? argv[3] : "application/octet-stream";

  sockaddr_in my_addr;   /* my address information */
  sockaddr_in their_addr; /* connector's address information */

  if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET )
    {
      perror( "websrv: socket()" );
      return 1;
    }

  bool b = true;
  if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&b, sizeof(b)))
    perror("setsockopt(SO_REUSEADDR)");


  my_addr.sin_family = AF_INET;       /* host byte order */
  my_addr.sin_port = htons( atoi( argv[1] ) );  /* short, network byte order */
  my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
  memset( &(my_addr.sin_zero), '\x0', sizeof(my_addr.sin_zero) );      /* zero the rest of the struct */

  if( bind( sockfd, (sockaddr*)&my_addr, sizeof(sockaddr_in) ) == SOCKET_ERROR )
    {
      perror( "websrv: bind()" );
      return 1;
    }

  if( listen( sockfd, BACKLOG ) == SOCKET_ERROR )
    {
      perror("listen");
      return 1;
    }

  signal(SIGINT, siginthandler);

  while(true)
    {  /* main accept() loop */
      socklen_t sin_size = sizeof(sockaddr_in);
      if( (new_fd = accept( sockfd, (sockaddr*)&their_addr, &sin_size )) ==
	  INVALID_SOCKET )
	{
	  perror( "websrv: accept()" );
	  continue;
	}

      cout << "websrv: got connection from " << inet_ntoa(their_addr.sin_addr) << endl;

      if( !fork() )
	{
	  closesocket(sockfd);
	  ConnectionHandler();
	  return 0;
	}

      closesocket(new_fd);  /* parent doesn't need this */
      while( waitpid(-1, NULL, WNOHANG) > 0 ); /* clean up child processes */
    }
}

void ConnectionHandler()
{
  int infd = open(filename, O_RDONLY);
  if( infd < 0 )
    {
      perror(filename);
      const char http_head[] = "HTTP/1.1 404 File Not Found\n"
	"Server: websrv\n"
	"Content-Type: text/html\n\n"
  "<STRONG>404 File Not Found</STRONG><BR>websrv can't find the requested file.";

      send(new_fd, http_head, sizeof(http_head)-1, 0);
      return;
    }

  const char http_head[] = "HTTP/1.1 200 OK\n"
    "Server: websrv\n"
    "Content-Type: ";

  if( send(new_fd, http_head, sizeof(http_head)-1, 0) == SOCKET_ERROR )
    {
      perror("websrv: send()");
      return;
    }

  if( send(new_fd, mimetype, strlen(mimetype), 0) == SOCKET_ERROR )
    {
      perror("websrv: send()");
      return;
    }

  if( send(new_fd, "\n\n", 2, 0) == SOCKET_ERROR )
    {
      perror("websrv: send()");
      return;
    }

  while(true)
    {
      char recvbuf[MAX_LINE_LENGTH];
      int size_recv = read(infd, recvbuf, sizeof(recvbuf));
      if( size_recv == -1 )
      	{
      	  perror("websrv: read()");
          closesocket(new_fd);
      	  return;
      	}

      if( size_recv < 1 )
	break;
      
      if( send( new_fd, recvbuf, size_recv, 0 ) == SOCKET_ERROR )
      	{
      	  perror( "websrv: send()" );
	        return;
      	}
    }
  
  closesocket( new_fd );
}

void siginthandler(int)
{
  cout << "Quitting..." << endl;
  closesocket(sockfd);
  closesocket(new_fd);
  while( waitpid(-1, NULL, WNOHANG) > 0 );
  exit(0);
}
