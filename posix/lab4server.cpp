#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <iostream.h>

#define MYPORT 1300    /* the port users will be connecting to */

#define BACKLOG 10     /* how many pending connections queue will hold */

int rsacrypt(int,int,int);   // Funktion för kryptering/dekryptering
void *subthread(int);        // Tråden som behandlar varje anslutning

main()
{
  int sockfd;
  // Skapa socket
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
      perror("socket");
      exit(1);
    }

  // Initiera  sockaddr_in-objekt
  socklen_t sin_size;
  sockaddr_in my_addr;    /* my address information */
  sockaddr_in their_addr; /* connector's address information */
  my_addr.sin_family = AF_INET;         /* host byte order */
  my_addr.sin_port = htons(MYPORT);     /* short, network byte order */
  my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
  bzero(&(my_addr.sin_zero), 8);        /* zero the rest of the struct */

  // Bind socket till port
  if (bind(sockfd, (sockaddr *)&my_addr, sizeof(sockaddr)) == -1)
    {
      perror("bind");
      exit(1);
    }

  cout << "Lab4Server started. Listening on port " << MYPORT << "." << endl;

  // Lyssna på port
  if (listen(sockfd, BACKLOG) == -1) 
    {
      perror("listen");
      exit(1);
    }

  // accept()-loop
  for(;;)
    {
      // Ta emot anslutning från klientdator
      sin_size = sizeof(sockaddr_in);
      int new_fd;
      if ((new_fd = accept(sockfd, (sockaddr *)&their_addr, &sin_size)) == -1) 
	{
	  perror("accept");
	  return -1;
	}
      
      cout << "lab4server: got connection from " << inet_ntoa(their_addr.sin_addr) << endl;

      // Skapa ny tråd för att hantera anslutningen till klientdatorn
      pthread_t tid;
      pthread_create(&tid, NULL, (void*(*)(void*))subthread, (void*)new_fd);
    }
}

// Enkel liten funktion för att läsa till radslut från socket eller fil m m
int readline(int fd, char *recvbuf, int bufsiz)
{
  int bc = 0;
  while( bc < bufsiz )
    {
      ssize_t size_recv = read(fd, recvbuf+bc, 1);
      if( size_recv == 0 )
	{
	  recvbuf[bc] = '\n';
	  break;
	}

      if( size_recv < 0 )
	return -1;

      if( (size_recv > 0) & (recvbuf[bc] == '\n') )
        break;

      bc += size_recv;
    }

  if( recvbuf[bc] != '\n' )
    {
      char c;
      while( read(fd, &c, 1) > 0 )
	if( c == '\n' )
	  break;
    }

  recvbuf[bc] = 0;
  return 0;
}

void *subthread(int new_fd)
{ /* this is the session thread */
  cout << "Sending hello message..." << endl;
  const char hello[] = "HELLO, TELL ME YOUR NAME\n";
  if (write(new_fd, hello, sizeof(hello)-1) != sizeof(hello)-1)
    perror("send");
	  
  cout << "Receiving client's name..." << endl;
  char clientname[1024];
  readline(new_fd, clientname, sizeof(clientname));

  cout << "Client name is " << clientname << ", opening pass key file..." << endl;

  int pwdfd = open("lab4server.passwords", O_RDONLY);
  if( pwdfd < 0 )
    {
      perror("lab4server: Error opening file lab4server.passwords");
      close(new_fd);
      return NULL;
    }

  cout << "Searching pass key for " << clientname << "..." << endl;

  // Läs krypteringsnyckel från nyckelfilen
  char pwdfileline[256];
  do
    {
      if( readline(pwdfd, pwdfileline, sizeof(pwdfileline)) )
	{
	  perror("lab4server: Error reading pass key file");
	  close(pwdfd);
	  close(new_fd);
	  return NULL;
	}

      if( strtok(pwdfileline, ":") == NULL )
	{
	  cerr << "lab4server: Keys for " << clientname << " not in password file." << endl;
	  const char denymsg[] = "ACCESS DENIED!\n";
	  write(new_fd, denymsg, sizeof(denymsg)-1);
	  close(pwdfd);
	  close(new_fd);
	  return NULL;
	}
    }
  while( strcasecmp(pwdfileline, clientname) != 0 );

  close(pwdfd);

  int E = atoi(strtok(NULL, ":"));
  int N = atoi(strtok(NULL, ":"));

  cout << "Got pass key for " << clientname << ". Calculating random key..." << endl;

  // Hitta på ett slumptal, kryptera och se om klienten kunde dekryptera
  int randnr, cryptnr;
  do
    {
      srand(time(NULL));
      randnr = rand() % 12;
      cryptnr = rsacrypt(randnr,E,N);
    }
  while( randnr == cryptnr );

  cout << "Handshaking..." << endl;

  if(write(new_fd, &cryptnr, sizeof(cryptnr)) != sizeof(cryptnr))
    {
      close(new_fd);
      return NULL;
    }

  int clearnr;
  if(read(new_fd, &clearnr, sizeof(clearnr)) != sizeof(clearnr))
    {
      close(new_fd);
      return NULL;
    }

  cout << "Auth key: " << randnr << "->" << cryptnr << "->" << clearnr << endl;

  if(clearnr != randnr)
    {
      const char denymsg[] = "ACCESS DENIED!\n";
      write(new_fd, denymsg, sizeof(denymsg)-1);
      cerr << "Access was denied." << endl;
      close(new_fd);
      return NULL;
    }
	  
  cout << "Auth passed. Sending welcome message..." << endl;

  srand(time(NULL));
  int datalength = ((rand() % 20) + 20) << 20;
  char buffer[2048];
  sprintf(buffer, "WELCOME %s, I HAVE %i BYTES %i BYTES EACH\n",
	  clientname, datalength, sizeof(int));
  write(new_fd, buffer, strlen(buffer));

  datalength /= sizeof(int);
  cout << "Sending data stream of " << datalength << " integers..." << endl;

  // Skriv information blockvis, 256*sizeof(int) bytes stora block.
  int i[256];
  int nblocks = datalength * sizeof(int) / sizeof(i);
  for( int c=0; c < nblocks; c++ )
    {
      for( int ic=0; ic < sizeof(i) / sizeof(int); ic++ )
	i[ic] = rand();
      
      if( write(new_fd, i, sizeof(i)) != sizeof(i) )
	{
	  cerr << "write failed." << endl;
	  close(new_fd);
	  return NULL;
	}
    }

  ssize_t readlen;	  
  cout << "Receiving results..." << endl;
  if ((readlen = read(new_fd, buffer, sizeof(buffer)-1)) <= 0)
    {
      close(new_fd);
      return NULL;
    }
	    
  buffer[readlen] = 0;
  cout << buffer << endl;
  
  close(new_fd);
  return NULL;
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
