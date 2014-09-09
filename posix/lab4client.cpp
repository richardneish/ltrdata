#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <stdio.h>
#include <iostream.h>

#define TCPPORT 1300

// RSA kryptering/dekryptering
int rsacrypt(int,int,int);

main(int argc, char **argv)
{
  if( argc != 4 )
    {
      cout << "Syntax: " << argv[0] << " host D N" << endl;
      return 1;
    }

  int D=atoi(argv[2]);
  int N=atoi(argv[3]);

  // Tag info om dator 
  hostent *hp;
  if( (hp = gethostbyname(argv[1])) == NULL)
    {
      cerr << argv[1] << " unknown host!" << endl;
      exit(1);
    }

  // Öppna socket.
  int s;
  if( (s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      cerr << "Bad socket" << endl;
      exit(1); 
    }

  // Initiera sockaddr_in-objekt för TCP/IP
  sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(TCPPORT);
  bcopy(hp->h_addr, &sin.sin_addr, hp->h_length);

  // Upprätta forbindelse
  if( connect(s, (sockaddr *)&sin, sizeof(sin)) < 0)
    {
      perror("Cannot connect to host");
      exit(1); 
    }

  cout << "Connected to " << argv[1] << ". Waiting for hello message..." << endl;

  char buffer[1024];
  ssize_t readlen;
  if( (readlen = read(s, buffer, sizeof(buffer)-1)) <= 0 )
    {
      perror("read()");
      return -1;
    }

  buffer[readlen] = 0;
  if( strcasecmp(buffer, "HELLO, TELL ME YOUR NAME\n") != 0 )
    {
      cerr << "Fel svar från servern." << endl;
      return 1;
    }

  cout << "OK. Handshaking..." << endl;

  const char machinename[] = "KALLE\n";
  write(s, machinename, sizeof(machinename)-1);

  // Ta emot krypterat slumptal och dekryptera och skicka tillbaka
  int cryptnr;
  read(s, &cryptnr, sizeof(cryptnr));
  int clearnr = rsacrypt(cryptnr, D, N);
  write(s, &clearnr, sizeof(clearnr));

  cout << "Auth key: " << cryptnr << "->" << clearnr << endl;

  cout << "Waiting for welcome message..." << endl;
  
  char *bufptr;
  for( bufptr = buffer; bufptr < buffer + sizeof(buffer) - 1; bufptr++ )
    {
      if( read(s, bufptr, 1) < 0 )
	return 1;
      if( *bufptr == '\n' )
	break;
    }
  *bufptr = 0;

  cout << "Message from server: '" << buffer << "'" << endl;

  if( strcasecmp(buffer, "ACCESS DENIED!") == 0 )
    {
      cerr << "Access denied by host." << endl;
      return 1;
    }

  cout << "Authenticated OK." << endl;

  int datalength = atoi(strstr(buffer, "I HAVE ")+7) / atoi(strstr(buffer, "BYTES ")+6);

  long double sum = 0;
  long long intnrs = 0;

  cout << "Receiving " << datalength << " integers from host..." << endl;

  // Ta emot blockvis 256*sizeof(int) bytes stora block
  while( intnrs < datalength )
    {
      int i[256];
      readlen = read(s, i, sizeof(i)) / sizeof(int);

      if( readlen < 0 )
	break;

      if( readlen == 0 )
	usleep(1);
      
      for( int ic = 0; ic < readlen; ic++ )
	sum += i[ic];

      intnrs += readlen;
    }

  cout << "Received " << intnrs << " integers. Calculating results..." << endl;

  long double avg = sum / intnrs;

  char strbuffer[2048];
  sprintf(strbuffer, "SUM=%Lf AVG=%Lf\n", sum, avg);

  cout << strbuffer << "Returning results..." << endl;

  write(s, strbuffer, strlen(strbuffer)-1);

  close(s); // Stäng socket.

  cout << "All done!" << endl;
}

// RSA kryptering/dekryptering
int rsacrypt(int BC, int ED, int N)
{
  unsigned long long ST = 1;
  while( ED-- )
    ST *= BC;

  ST %= N;

  return ST;
}
