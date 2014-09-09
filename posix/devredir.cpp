#include <iostream.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <netinet/in.h>

#define PACKET_SIZE 128

int main( int argc, char **argv, char **envp )
{
	if( argc != 3 )
	{
		cout << "Syntax:" << endl <<
			argv[0] << " /dev/x n" << endl << endl <<
         	"where x is a device name, i.e. /dev/cua0 and n is a TCP/IP" << endl <<
            "port to redirect data to." << endl;

		return -1;
	}

   int portno = atoi( argv[2] );
   if( !portno )
   {
		cout << "Use a port between 1024 and 65535." << endl;
      return -1;
   }

	cout << "<< DEVICE TO TCP/IP REDIRECTOR >>" << endl;

   int iodev = open( argv[1], O_RDWR | O_NOCTTY | O_NDELAY );
   if( iodev == -1 )
   {
   	perror( "open()" );
      return 1;
   }

	int originalsocket = socket( AF_INET, SOCK_STREAM, 0 );
   if( originalsocket == -1 )
	{
		perror( "socket()" );
		return 2;
	}

	sockaddr_in localaddress;
	localaddress.sin_family = AF_INET;
	localaddress.sin_port = htons( portno );
   localaddress.sin_addr.s_addr = 0;
   bzero( &(localaddress.sin_zero), sizeof(localaddress.sin_zero) );
	if( bind( originalsocket, (sockaddr*)&localaddress,
   	sizeof(localaddress) ) )
   {
   	perror( "bind()" );
      close( originalsocket );
      return 2;
   }

   if( listen( originalsocket, 0 ) )
   {
   	perror( "listen()" );
      close( originalsocket );
      return 2;
   }

   while(true)
   {
		cout << argv[0] << ": waiting for connection on port " << portno << "..." << endl;
	   sockaddr_in remoteaddress;
      int connsocket;
   	if( (connsocket = accept( originalsocket,
      	(sockaddr*)&remoteaddress, &((*new unsigned)=sizeof(remoteaddress)) )) == -1 )
      {
			perror( "accept()" );
         close( originalsocket );
         return 2;
      }

      cout << argv[0] << ": connection from " <<
      	inet_ntoa(remoteaddress.sin_addr) << endl;

	   int hfds = (connsocket > iodev) ? connsocket+1 : iodev+1;
      while(true)
      {
	      char packet[PACKET_SIZE];
			timeval time5min = { 300, 0 };
			timeval time1min = { 60, 0 };
         fd_set recvfds, sendfds, excfds;

		   FD_ZERO( &recvfds );
		   FD_SET( connsocket, &recvfds );
		   FD_SET( iodev, &recvfds );
		   FD_ZERO( &excfds );
		   FD_SET( connsocket, &excfds );
		   FD_SET( iodev, &excfds );
         select( hfds, &recvfds, NULL, &excfds, &time5min );

      	if( FD_ISSET( connsocket, &excfds ) )
         {
         	cerr << "Socket I/O exception." << endl;
            break;
         }
         if( !FD_ISSET( iodev, &recvfds ) & !FD_ISSET( connsocket, &recvfds ) )
         {
         	cout << "Connection timed out." << endl;
            break;
         }

         if( FD_ISSET( iodev, &excfds ) )
         {
         	cerr << "Device I/O exception." << endl;
         	break;
         }

      	int packlen;
         if( FD_ISSET( connsocket, &recvfds ) )
         {
         	packlen = recv( connsocket, packet, PACKET_SIZE, 0 );
	         if( packlen == -1 )
   	      {
      	   	perror( "recv()" );
         		break;
	         }
         	if( packlen == 0 )
            {
            	cout << "Client closed connection." << endl;
         		break;
				}

         	FD_ZERO( &sendfds );
            FD_SET( iodev, &sendfds );
            FD_ZERO( &excfds );
            FD_SET( iodev, &excfds );

            select( hfds, NULL, &sendfds, &excfds, &time1min );
            if( FD_ISSET( iodev, &excfds ) )
            {
            	cerr << "Device I/O exception." << endl;
               break;
            }
            if( !FD_ISSET( iodev, &sendfds ) )
            {
            	cerr << "Timed out waiting for Device Ready To Send signal." << endl;
               break;
            }

            size_t count;
            while( packlen )
					if( (count = write( iodev, packet, packlen )) == -1 )
               {
                  perror( "write()" );
                  break;
               }
               else
               	packlen -= count;

            if( packlen )
            	break;
         }

         if( FD_ISSET( iodev, &recvfds ) )
         {
         	packlen = read( iodev, packet, PACKET_SIZE );
            if( packlen == -1 )
            {
            	perror( "read()" );
               break;
            }
            if( packlen == 0 )
            {
            	cerr << "NULL data read from " << argv[1] << endl;
               break;
            }

         	FD_ZERO( &sendfds );
            FD_SET( connsocket, &sendfds );
            FD_ZERO( &excfds );
            FD_SET( connsocket, &excfds );

            select( hfds, NULL, &sendfds, &excfds, &time1min );
            if( FD_ISSET( connsocket, &excfds ) )
            {
            	cerr << "Socket I/O exception." << endl;
               break;
            }
            if( !FD_ISSET( connsocket, &sendfds ) )
            {
            	cerr << "Timed out waiting for Socket Ready To Send signal." << endl;
               break;
            }

            size_t count;
            while( packlen )
					if( (count = send( connsocket, packet, packlen, 0 )) == -1 )
               {
                  perror( "send()" );
                  break;
               }
               else
               	packlen -= count;

            if( packlen )
            	break;
         }
      }

      cout << argv[0] << ": closing connection..." << endl;
      close( connsocket );
	}
}

