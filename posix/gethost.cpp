#include <iostream.h>

#ifdef _Windows
#	include <windows.h>
#else
#	include <netdb.h>
#	include <unistd.h>
#	include <netinet/in.h>
#	include <sys/socket.h>
#	include <arpa/inet.h>
#	define WSACleanup()
#endif

#ifndef INADDR_NONE
#	define INADDR_NONE -1
#endif

main( int, char **argv )
{
  if( argv[1] )
    if( (argv[1][0] | 2) == '/' )
      {
	cout <<
	  "GETHOST is free software by Olof Lagerkvist" << endl <<
	  "http://www.ltr-data.se" << endl <<
	  "Please read GETHOST.TXT for more info." << endl <<
	  endl <<
	  "Syntax:" << endl <<
	  "GETHOST [IP]" << endl <<
	  endl <<
	  "IP can be an IP number or host name. If omitted, the" << endl <<
	  "local host will be looked up." << endl;
	return 0;
      }

#ifdef _Windows
  if( WSAStartup( 0x0101, new WSADATA ) )
    {
      cerr << "Winsock error" << endl;
      return -1;
    }
#endif

  char buf[260];
  char *hname;
  if( argv[1] == NULL )
   {
     if( gethostname( buf, sizeof(buf) - 1 ) )
       {
	 cerr << "Can't resolve local host name" << endl;
         WSACleanup();
         return 0;
       }
     hname = buf;
   }
  else
    hname = argv[1];
  
  hostent *hent;
  unsigned long haddr = inet_addr( hname );
  if( haddr == INADDR_NONE )
    hent = gethostbyname( hname );
  else
    hent = gethostbyaddr( (char*)&haddr, 4, 2 );
  
  if( !hent )
    {
      cerr << "Host name lookup failed." << endl;
      WSACleanup();
      return -1;
    }

  cout << "Host:\t" << hent->h_name << endl;

  int cou = 0;
  while( hent->h_aliases[cou] )
    cout << "Alias:\t" << hent->h_aliases[cou++] << endl;

  cout << "IP:\t";

  cou = 0;
  while( hent->h_addr_list[cou] )
    cout << inet_ntoa( *(in_addr*)hent->h_addr_list[cou++] ) << '\t';

  cout << endl;

  WSACleanup();
  return 0;
}
