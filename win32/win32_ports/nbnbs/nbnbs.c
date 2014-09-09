/*
 *  nbnbs.c (c) 2000 by Mixter <mixter@newyorkoffice.com>
 *
 *          http://mixter.warrior2k.com
 *
 *  NetBios Name Bulk Scanner  (for Samba and Windows)
 *
 *  Based on nbname.c by mynock (members.xoom.com/retarded),
 *  parts of the code copyright by the ADM Crew.
 *
 *  I just wrote this little tool, since a netbios long range scanner for
 *  UNIX wasn't available yet (not that I'm aware of). This program does
 *  basically the same as "Legion" for Windows by Rhino9. It can scan from
 *  standard input, from a host list, or a whole A class. See usage for more.
 *  PS: This program will never support multi-threaded netbios scans, since
 *  every process needs to bind to the same udp port (137) to make all
 *  clients (some windows versions are strict about that source port) answer.
 *
 *  This program is distributed for educational purposes and without any
 *  explicit or implicit warranty; in no event shall the author or
 *  contributors be liable for any direct, indirect or incidental damages
 *  arising in any way out of the use of this software.
 *
 */

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <errno.h>
#if defined(USE_MINWCRT) && defined(_DLL)
#include <minwcrtlib.h>
#else
#include <stdlib.h>
#endif
#include <string.h>
#include <unistd.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <sys/time.h>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <netdb.h>
#include <getopt.h>

#include <winsock.h>
#include <winstrct.h>

#if defined(USE_MINWCRT) && defined(_DLL)
#pragma comment(lib, "minwcrt")
#endif
#pragma comment(lib, "wsock32")

int deftimeout = 5;
FILE *output;

#define SERV_PORT 137
#define QUERYLEN 50

char nbquery[] =
"\x03\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x20\x43\x4b\x41\x41"
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x00\x00\x21\x00\x00";

int resolve (char *hostname, struct sockaddr_in *addr, unsigned short defport);

int 
nbname (char *target)
{
  SOCKET sockfd;
  int i, n, x = 0;
  struct sockaddr_in servaddr, myaddr;
  char buf[1024], buf2[1024];
  fd_set readfs;
  struct timeval tv;

  if (!resolve (target, &servaddr, SERV_PORT))
    {
      fprintf (stderr, "can't resolve %s\n", target);
    }
  if ((sockfd = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
    {
      h_perror ("socket()");
      return 0;
    }
  memset (&myaddr, 0, sizeof (myaddr));
  myaddr.sin_family = AF_INET;
  myaddr.sin_port = htons (SERV_PORT);
  myaddr.sin_addr.s_addr = htonl (INADDR_ANY);
  if (bind (sockfd, (struct sockaddr *) &myaddr, sizeof (myaddr)) ==
      SOCKET_ERROR)
    {
      h_perror ("Couldn't bind");
      fprintf (stderr, "couldn't bind to port %i: Win95 might not answer\n", SERV_PORT);
    }

  while (1)
    {
      if (sendto (sockfd, nbquery, QUERYLEN, 0, (struct sockaddr *) &servaddr, sizeof (struct sockaddr_in)) < 0)
	{
	  h_perror ("sendto()");
	  return 0;
	}
      FD_ZERO (&readfs);
      FD_SET (sockfd, &readfs);
      tv.tv_sec = deftimeout;
      tv.tv_usec = 0;
      n = select (sockfd + 1, &readfs, NULL, NULL, &tv);

      switch (n)
	{
	case 1:
	  if ((i = recvfrom (sockfd, buf, 800, 0, NULL, NULL)) < 0)
	    {
	      h_perror ("receive error\n");
	      close (sockfd);
	      return 0;
	    }
	  break;
	case 0:
	  fprintf (stderr, "timeout\n");
	  close (sockfd);
	  return (0);
	  break;
	default:
	  h_perror ("select()");
	  close (sockfd);
	  return (0);
	  break;
	}
      if (n == 1)
	break;
    }
  for (n = 57; (n < i) && (buf[n] != ' '); n++)
    {
      buf2[x] = buf[n];
      x++;
    }
  fprintf (output, "%s - nbname is '%s'\n", target, buf2);
  fflush (output);
  close (sockfd);
  return 1;
}

int 
resolve (char *hostname, struct sockaddr_in *addr, unsigned short defport)
{
  char *p;
  struct hostent *he;

  ZeroMemory (addr, sizeof (struct sockaddr_in));
  addr->sin_family = AF_INET;
  if ((p = strchr (hostname, ':')))
    {
      *p++ = '\0';
      addr->sin_port = htons (atoi (p));
    }
  else
    {
      addr->sin_port = htons (defport);
    }
  addr->sin_addr.s_addr = inet_addr(hostname);
  if (addr->sin_addr.s_addr == INADDR_NONE)
    {
      if (!(he = gethostbyname (hostname)))
	{
	  return 0;
	}
      memcpy (&addr->sin_addr, he->h_addr, he->h_length);
    }
  return 1;
}

int
incip (char *ip)
{
  int a, b, c, d;
  sscanf (ip, "%d.%d.%d.%d", &a, &b, &c, &d);

  if (b >= 255)
    return 0;
  if (c >= 255)
    {
      c = 0;
      b++;
    }
  if (d >= 255)
    {
      d = 0;
      c++;
    }

  d++;

  sprintf (ip, "%d.%d.%d.%d", a, b, c, d);
  return 1;
}

void 
usage (char *av)
{
  fprintf (stderr, "usage: %s [options]\n", av);
  fprintf (stderr, "   -f <file>  - read hosts from file\n");
  fprintf (stderr, "   -i         - read hosts from standard input\n");
  fprintf (stderr, "   -s <class> - scan A class subnet #\n");
  fprintf (stderr, "   -o <file>  - output scan results to file\n");
  fprintf (stderr, "   -t <secs>  - set the query timeout in seconds\n");
  exit (0);
}

int
main (int argc, char **argv)
{
  FILE *input = stdin;
  int opt = 0, class = 0, mod = 0;
  char buffer[512];
  WSADATA wsadata;
  WSAStartup(0x0101, &wsadata);

  fprintf (stderr, "\t\tnetbios name bulk scanner 0.2\n\n");

  output = stdout;

  while ((opt = getopt (argc, argv, "f:is:o:t:")) != EOF)
    switch (opt)
      {
      case 'f':
	fprintf (stderr, "Reading hosts from %s...\n", optarg);
	mod = 1;
	if ((input = fopen (optarg, "r")) == NULL)
	  usage (argv[0]);
	break;
      case 'i':
	mod = 1;
	fprintf (stderr, "Reading hosts from standard input...\n");
	break;
      case 's':
	fprintf (stderr, "Scanning A Class %s...\n", optarg);
	mod = 2;
	if ((class = atoi (optarg)) < 1)
	  usage (argv[0]);
	break;
      case 'o':
	fprintf (stderr, "Output file: %s...\n", optarg);
	if ((output = fopen (optarg, "a")) == NULL)
	  usage (argv[0]);
	break;
      case 't':
	deftimeout = atoi (optarg);
	fprintf (stderr, "Timeout set to %d seconds\n", deftimeout);
	break;
      default:
	usage (argv[0]);
      }

  if (mod == 1)
    {
      while ((fgets (buffer, 512, input)) != NULL)
	{
	  buffer[strlen (buffer) - 1] = '\0';
	  nbname (buffer);
	}
    }
  else if (mod == 2)
    {
      sprintf (buffer, "%d.0.0.0", class);
      while ((incip (buffer)))
	nbname (buffer);
    }
  else
    usage (argv[0]);

  return 0;
}
