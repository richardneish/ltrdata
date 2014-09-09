#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <string.h>
#include <winsock.h>

#pragma comment(lib, "wsock32")

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

#define zero(x)		memset(&(x),0,sizeof(x))

#define RECV_READY	1
#define SEND_READY	2
#define ERR_SOCKET	4
#define ANY_SOCKET	-1

int waituntilready(SOCKET, int, int);

int
main(int argc, char **argv, char **)
{
  SOCKET contrconn, dataconn;

  int retval = -1;
  bool loginprompt = false;

  unsigned long dataip;
  unsigned short dataport;
  unsigned short a, b, c, d, e, f;

  int portno = 21;

  char buf[1024];
  char *chrptr;

  sockaddr_in contraddr, dataaddr;

  int recvlen;
  int readyflag;

  // Permanent FTP commands
  const char ftpuser[] = "USER ftp\r\n";
  const char ftppass[] = "PASS a@w.com\r\n";
  const char ftptype[] = "TYPE I\r\n";
  const char ftppasv[] = "PASV\r\n";

  if (argc < 3)
    {
      puts("Usage:\r\n"
	   "GETFTP [?] remotefile localfile [server [port]]\r\n\n"
	   "? prompts for user name and password.\r\n"
	   "Default server is localhost, default port is 21.");
      return -1;
    }

  if (argv[1][0] == '?')
    {
      loginprompt = true;
      ++argv;
      --argc;
    }

  //////////////// Creating output file ////////////////////////

  puts("Creating output file...");
  int fileno =
    open(argv[2], O_WRONLY | O_CREAT | O_EXCL | O_BINARY, S_IREAD | S_IWRITE);
  if (fileno == -1)
    {
      perror(argv[2]);
      return -1;
    }

  //////////////// Initializing WinSock API ////////////////////

  WSADATA wsadata;
  puts("Starting Winsock API...");
  if (WSAStartup(0x0101, &wsadata))
    {
      printf("Unable to initialize Winsock API. (%i)\n", WSAGetLastError());
      close(fileno);
      return -1;
    }

  //////////////// Creating sockets ////////////////////////////

  puts("Creating TCP Sockets...");
  if (((contrconn =
	socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) | ((dataconn =
								socket
								(AF_INET,
								 SOCK_STREAM,
								 0)) ==
							       INVALID_SOCKET))
    {
      printf("Unable to create TCP sockets. (%i)\n", WSAGetLastError());
      goto getout;
    }

  /////////////////////// Connect control connection ///////////

  if (argc > 4)
    portno = strtoul(argv[4], NULL, 0);

  contraddr.sin_family = AF_INET;
  contraddr.sin_port = htons(portno);
  if (argc > 3)
    contraddr.sin_addr.s_addr = inet_addr(argv[3]);
  else
    contraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  zero(contraddr.sin_zero);

  if (contraddr.sin_addr.s_addr == INADDR_NONE)
    {
      printf("Looking up host %s...\n", argv[3]);

      hostent *hostinfo;

      if ((hostinfo = gethostbyname(argv[3])) == NULL)
	{
	  printf("Couldn't find IP address for host name %s\n", argv[3]);
	  goto getout;
	}

      contraddr.sin_addr.s_addr =
	*(unsigned long *)(hostinfo->h_addr_list[0]);

      if (contraddr.sin_addr.s_addr == INADDR_NONE)
	{
	  printf("Invalid IP address: %u.%u.%u.%u\n",
		 hostinfo->h_addr_list[0][0], hostinfo->h_addr_list[0][1],
		 hostinfo->h_addr_list[0][2], hostinfo->h_addr_list[0][3]);
	  goto getout;
	}
    }

  printf("Contacting host %s:%i...\n", inet_ntoa(contraddr.sin_addr), portno);
  if (connect(contrconn, (sockaddr *) & contraddr, sizeof(contraddr)) ==
      SOCKET_ERROR)
    {
      printf("Unable to connect to %s:%i. (%i)\n",
	     inet_ntoa(contraddr.sin_addr), portno, WSAGetLastError());
      goto getout;
    }

  //////////////// Wait for Welcome Message ////////////////////

  puts("Host contacted, waiting for reply...");
  if ((~waituntilready(contrconn, 10, RECV_READY | ERR_SOCKET)) & RECV_READY)
    {
      printf("Timeout waiting for FTP welcome message from %s.\n",
	     inet_ntoa(contraddr.sin_addr), portno);
      goto getout;
    }

  if ((recvlen = recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
    {
      printf("Error reading from socket. (%i)\n", WSAGetLastError());
      goto getout;
    }
  buf[recvlen] = NULL;
  printf(buf);
  if (recvlen < 4)
    {
      puts("Invalid response.");
      goto getout;
    }
  else if (strncmp(buf, "220", 3))
    {
      puts("Not a valid FTP server welcome message.");
      goto getout;
    }

  //////////////// Send User Name //////////////////////////////

  while (waituntilready(contrconn, 1, RECV_READY) & RECV_READY)
    {
      Sleep(0);

      if ((recvlen =
	   recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
	{
	  printf("Error reading from socket. (%i)\n", WSAGetLastError());
	  goto getout;
	}
      buf[recvlen] = NULL;
      printf(buf);
    }

  if ((~waituntilready(contrconn, 10, SEND_READY)) & SEND_READY)
    {
      puts("Timeout waiting to send data.");
      goto getout;
    }

  if (loginprompt)
    {
      strcpy(buf, "USER ");
      printf("Enter your login name: ");
      fgets(buf + 5, sizeof(buf)-8, stdin);
      strcat(buf, "\r\n");
    }
  else
    strcpy(buf, ftpuser);

  printf(buf);
  if (send(contrconn, buf, (int) strlen(buf), 0) != strlen(buf))
    {
      printf("Error sending on socket. (%i)\n", WSAGetLastError());
      goto getout;
    }

  //////////////// Recieve User Name response //////////////////

  if ((~waituntilready(contrconn, 30, RECV_READY)) & RECV_READY)
    {
      printf("Timeout waiting for reply from %s.\n",
	     inet_ntoa(contraddr.sin_addr), portno);
      goto getout;
    }

  if ((recvlen = recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
    {
      printf("Error reading from socket. (%i)\n", WSAGetLastError());
      goto getout;
    }
  buf[recvlen] = NULL;
  printf(buf);
  if (recvlen < 4)
    {
      puts("Invalid response.");
      goto getout;
    }
  else if (strncmp(buf, "331", 3))
    {
      puts("User name not accepted.");
      goto getout;
    }

  //////////////// Send password ///////////////////////////////

  while ((readyflag = waituntilready(contrconn, 10, ANY_SOCKET)) & RECV_READY)
    {
      Sleep(0);

      if ((recvlen =
	   recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
	{
	  printf("Error reading from socket. (%i)\n", WSAGetLastError());
	  goto getout;
	}
      buf[recvlen] = NULL;
      printf(buf);
    }

  if ((~readyflag) & SEND_READY)
    {
      puts("Timeout waiting to send data.");
      goto getout;
    }

  if (loginprompt)
    {
      strcpy(buf, "PASS ");
      printf("Enter your password: ");
      chrptr = buf + 4;
      while (((*(++chrptr) = getch()) != '\r') &
	     (chrptr - buf < sizeof(buf)-8))
	if (*chrptr == NULL)
	  --chrptr;

      *(++chrptr) = '\n';
      *(++chrptr) = NULL;
      puts("\r\nPASS *****");
    }
  else
    {
      strcpy(buf, ftppass);
      printf(buf);
    }

  if (send(contrconn, buf, (int) strlen(buf), 0) != strlen(buf))
    {
      printf("Error sending on socket. (%i)\n", WSAGetLastError());
      goto getout;
    }

  //////////////// Recieve password response ///////////////////

  if ((~waituntilready(contrconn, 30, RECV_READY)) & RECV_READY)
    {
      printf("Timeout waiting for reply from %s.\n",
	     inet_ntoa(contraddr.sin_addr), portno);
      goto getout;
    }

  if ((recvlen = recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
    {
      printf("Error reading from socket. (%i)\n", WSAGetLastError());
      goto getout;
    }
  buf[recvlen] = NULL;
  printf(buf);
  if (recvlen < 4)
    {
      puts("Invalid response.");
      goto getout;
    }
  else if (strncmp(buf, "230", 3))
    {
      puts("Login incorrect.");
      goto getout;
    }

  //////////////// Send TYPE request ///////////////////////////

  while (waituntilready(contrconn, 1, RECV_READY) & RECV_READY)
    {
      Sleep(0);

      if ((recvlen =
	   recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
	{
	  printf("Error reading from socket. (%i)\n", WSAGetLastError());
	  goto getout;
	}
      buf[recvlen] = NULL;
      printf(buf);
    }

  if ((~waituntilready(contrconn, 10, SEND_READY)) & SEND_READY)
    {
      puts("Timeout waiting to send data.");
      goto getout;
    }

  printf(ftptype);
  if (send(contrconn, ftptype, sizeof(ftptype) - 1, 0) != sizeof(ftptype) - 1)
    {
      printf("Error sending on socket. (%i)\n", WSAGetLastError());
      goto getout;
    }

  //////////////// Recieve TYPE response ///////////////////////

  if ((~waituntilready(contrconn, 30, RECV_READY)) & RECV_READY)
    {
      printf("Timeout waiting for reply from %s.\n",
	     inet_ntoa(contraddr.sin_addr), portno);
      goto getout;
    }

  if ((recvlen = recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
    {
      printf("Error reading from socket. (%i)\n", WSAGetLastError());
      goto getout;
    }
  buf[recvlen] = NULL;
  printf(buf);
  if (recvlen < 4)
    {
      puts("Invalid response.");
      goto getout;
    }
  else if (strncmp(buf, "200", 3))
    puts("Warning! Binary transmission not accepted.");

  //////////////// Send PASV request ///////////////////////////

  while (waituntilready(contrconn, 1, RECV_READY) & RECV_READY)
    {
      Sleep(0);

      if ((recvlen =
	   recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
	{
	  printf("Error reading from socket. (%i)\n", WSAGetLastError());
	  goto getout;
	}
      buf[recvlen] = NULL;
      printf(buf);
    }

  if ((~waituntilready(contrconn, 10, SEND_READY)) & SEND_READY)
    {
      puts("Timeout waiting to send data.");
      goto getout;
    }

  printf(ftppasv);
  if (send(contrconn, ftppasv, sizeof(ftppasv) - 1, 0) != sizeof(ftppasv) - 1)
    {
      printf("Error sending on socket. (%i)\n", WSAGetLastError());
      goto getout;
    }

  //////////////// Recieve PASV response ///////////////////////

  if ((~waituntilready(contrconn, 30, RECV_READY)) & RECV_READY)
    {
      printf("Timeout waiting for reply from %s.\n",
	     inet_ntoa(contraddr.sin_addr), portno);
      goto getout;
    }

  if ((recvlen = recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
    {
      printf("Error reading from socket. (%i)\n", WSAGetLastError());
      goto getout;
    }
  buf[recvlen] = NULL;
  printf(buf);
  if (recvlen < 4)
    {
      puts("Invalid response.");
      goto getout;
    }
  else if (strncmp(buf, "227", 3))
    {
      puts("This server does not accept Passive Mode FTP.");
      goto getout;
    }

  /////////////////////// Interprete PASV response /////////////

  if ((chrptr = strchr(buf, '(')) == NULL)
    {
      puts("The server did answer correctly to Passive Mode FTP request.");
      goto getout;
    }

  sscanf(++chrptr, "%hu,%hu,%hu,%hu,%hu,%hu", &a, &b, &c, &d, &e, &f);

  dataip = (unsigned long)a << 24;
  dataip |= (unsigned long)b << 16;
  dataip |= (unsigned long)c << 8;
  dataip |= (unsigned long)d;
  dataport = (unsigned short)e << 8;
  dataport |= (unsigned short)f;

  dataaddr.sin_family = AF_INET;
  dataaddr.sin_addr.s_addr = htonl(dataip);
  dataaddr.sin_port = htons(dataport);
  zero(dataaddr.sin_zero);

  //////////////// Make data connection ////////////////////////

  printf("Contacting host %s:%i...\n", inet_ntoa(dataaddr.sin_addr),
	 dataport);
  if (connect(dataconn, (sockaddr *) & dataaddr, sizeof(dataaddr)) ==
      SOCKET_ERROR)
    {
      printf("Unable make data connection to %s:%i. (%i)\n",
	     inet_ntoa(dataaddr.sin_addr), dataport, WSAGetLastError());
      goto getout;
    }

  //////////////// Send RETR request ///////////////////////////

  while ((readyflag = waituntilready(contrconn, 10, ANY_SOCKET)) & RECV_READY)
    {
      Sleep(0);

      if ((recvlen =
	   recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
	{
	  printf("Error reading from socket. (%i)\n", WSAGetLastError());
	  goto getout;
	}
      buf[recvlen] = NULL;
      printf(buf);
    }

  if ((~readyflag) & SEND_READY)
    {
      puts("Timeout waiting to send data.");
      goto getout;
    }

  _snprintf(buf, sizeof buf, "RETR %s\r\n", argv[1]);
  buf[sizeof(buf)-1] = 0;
  printf(buf);
  if (send(contrconn, buf, (int) strlen(buf), 0) != strlen(buf))
    {
      printf("Error sending on socket. (%i)\n", WSAGetLastError());
      goto getout;
    }

  //////////////// Recieve RETR response ///////////////////////

  if ((~waituntilready(contrconn, 30, RECV_READY)) & RECV_READY)
    {
      printf("Timeout waiting for reply from %s.\n",
	     inet_ntoa(contraddr.sin_addr), portno);
      goto getout;
    }

  if ((recvlen = recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
    {
      printf("Error reading from socket. (%i)\n", WSAGetLastError());
      goto getout;
    }
  buf[recvlen] = NULL;
  printf(buf);
  if (recvlen < 4)
    {
      puts("Invalid response.");
      goto getout;
    }
  else if (strncmp(buf, "125", 3))
    if (strncmp(buf, "150", 3))
      {
	puts("Receive not accepted by the server.");
	goto getout;
      }

  //////////////// Recieve from data connection ////////////////

moretoget:
  while ((readyflag =
	  waituntilready(dataconn, 1, RECV_READY | ERR_SOCKET)) & RECV_READY)
    {
      Sleep(0);

      if ((recvlen = recv(dataconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
	{
	  printf("Error reading data stream. (%i)\n", WSAGetLastError());
	  goto getout;
	}

      if (recvlen == 0)
	{
	  readyflag &= ~RECV_READY;
	  break;
	}

      write(fileno, buf, recvlen);
    }

  if (readyflag)
    {
      printf("Error on data connection socket. (%i)\n", WSAGetLastError());
      goto getout;
    }

  if (!waituntilready(contrconn, 0, RECV_READY | ERR_SOCKET))
    goto moretoget;

  //////////////// Recieve transfer complete on control ////////

  if ((recvlen = recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
    {
      printf("Error reading from socket. (%i)\n", WSAGetLastError());
      goto getout;
    }

  buf[recvlen] = NULL;
  printf(buf);
  if (recvlen < 4)
    puts("Invalid response.");
  else if (strncmp(buf, "226", 3))
    puts("Error receiving data.");
  else
    {
      puts("Receive successful.");
      retval = 0;
    }

getout:
  puts("Closing sockets...");
  closesocket(contrconn);
  closesocket(dataconn);
  puts("Closing output file and Winsock API...");
  if (close(fileno))
    retval = -1;
  if (retval)
    {
      puts("Deleting output file...");
      unlink(argv[2]);
    }
  return retval;
}

int
waituntilready(SOCKET s, int seconds, int events)
{
  timeval tv;
  fd_set recvfds;
  fd_set sendfds;
  fd_set errfds;

  tv.tv_sec = seconds;
  tv.tv_usec = 0;
  FD_ZERO(&recvfds);
  FD_SET(s, &recvfds);
  FD_ZERO(&sendfds);
  FD_SET(s, &sendfds);
  FD_ZERO(&errfds);
  FD_SET(s, &errfds);
  select(0,
	 (events & RECV_READY) ? &recvfds : NULL,
	 (events & SEND_READY) ? &sendfds : NULL,
	 (events & ERR_SOCKET) ? &errfds : NULL, &tv);
  if (!FD_ISSET(s, &recvfds))
    events &= ~RECV_READY;
  if (!FD_ISSET(s, &sendfds))
    events &= ~SEND_READY;
  if (!FD_ISSET(s, &errfds))
    events &= ~ERR_SOCKET;
  return events;
}
