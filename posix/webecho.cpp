#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#if !defined(SOCKLEN_T_DEFINED) && !defined(HAVE_SOCKLEN_T)
   typedef int socklen_t;
#  define SOCKLEN_T_DEFINED 1
#endif

#define MAX_LINE_LENGTH 1024
#define BACKLOG 10    /* how many pending connections queue will hold */

using namespace std;

int sockfd = -1;
   
void siginthandler( int )
{
  close( sockfd );
  exit( 0 );
}

int main( int argc, char **argv, char ** )
{
  if( argc != 2 )
    {
      cout << argv[0] << " port" << endl;
      return 0;
    }
  
  sockaddr_in my_addr;   /* my address information */
  sockaddr_in their_addr; /* connector's address information */
  
  if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {
      perror( "websrv: socket()" );
      return 1;
    }

  my_addr.sin_family = AF_INET;       /* host byte order */
  my_addr.sin_port = htons( atoi( argv[1] ) );  /* short, network byte order */
  my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
  memset( &(my_addr.sin_zero), '\x0', sizeof(my_addr.sin_zero) );      /* zero the rest of the struct */

  if( bind( sockfd, (sockaddr*)&my_addr, sizeof(sockaddr_in) ) == -1 )
    {
      perror( "websrv: bind()" );
      return 1;
    }

  if( listen( sockfd, BACKLOG ) == -1 )
    {
      perror("listen");
      return 1;
    }

  signal( SIGINT, siginthandler );
  while(true)
    {  /* main accept() loop */
      socklen_t sin_size = sizeof(sockaddr_in);
      int new_fd;
      if( (new_fd = accept( sockfd, (sockaddr*)&their_addr, &sin_size )) == -1 )
	{
	  perror( "websrv: accept()" );
	  continue;
	}
      cout << "websrv: got connection from " << inet_ntoa(their_addr.sin_addr) << endl;
      if( !fork() )
	{
	  const char http_head[] = "HTTP/1.1 200 OK\n"
	    "Server: Testserver\n"
	    "Content-Type: text/plain\n\n";

	  if( send( new_fd, http_head, sizeof(http_head)-1, 0 ) == -1 )
	    {
	      perror( "websrv: send()" );
	      exit(-1);
	    }

	  while(true)
	    {
	      char recvbuf[MAX_LINE_LENGTH];
	      int size_recv = recv( new_fd, recvbuf, sizeof(recvbuf), 0 );
	      if( size_recv == -1 )
		{
		  perror( "websrv: recv()" );
		  exit(-1);
		}

	      if( size_recv < 4 )
                break;

	      if( send( new_fd, recvbuf, size_recv, 0 ) == -1 )
		{
		  perror( "websrv: send()" );
		  exit(-1);
		}
	    }

	  close( new_fd );

	  exit(0);
	}
      close( new_fd );  /* parent doesn't need this */

      while( waitpid( -1, NULL, WNOHANG ) > 0 ); /* clean up child processes */
    }
}
