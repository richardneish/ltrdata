#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <string.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "wsock32.lib")
#ifdef _DLL
#pragma comment(lib, "ntdllp.lib")
#endif

#pragma comment(linker, "/subsystem:windows")

#define zero(x)		memset(&(x),0,sizeof(x))

#define SOCK_RTR		1
#define SOCK_RTS		2
#define SOCK_EXC		4
#define SOCK_ANY		-1

enum
{
  FTP_OK,			// Funktionen utfördes felfritt
  FTP_BadArgument,		// Felaktigt argument skickat
  FTP_WinSockAPIError,		// Kunde inte kommunicera med WinSock
  FTP_CantCreateSocket,		// Kunde inte skapa socket
  FTP_ReceiveError,		// Kunde inte ta emot data
  FTP_SendError,		// Kunde inte skicka data
  FTP_TimeOut,			// Timeout vid väntan på svar
  FTP_ServerNotFound,		// Hittar inte angiven server
  FTP_CantConnect,		// Kan inte ansluta till angiven server
  FTP_InvalidResponse,		// Ogiltigt svar från servern (ej FTP?)
  FTP_LoginNotAllowed,		// Användarnamnet accepteras inte
  FTP_AccessDenied,		// Fel vid inloggning, filhämtning m m
  FTP_TypeNotAccepted,		// Servern accepterar inte given filtyp
  FTP_PassiveNotAccepted,	// Servern accepterar inte passiv FTP
  FTP_CantOpenLocalFile,	// Kunde inte öppna lokal fil
  FTP_CommandNotAccepted,	// Servern accepterade inte RETR/STOR
  FTP_TransferNotComplete,	// Överföringen avbröts innan klarsignal
  FTP_FileIOError		// Kunde inte läsa/skriva lokal fil
};

int waituntilready(SOCKET, int, int);

// Typical FTP commands
const char ftpuser[] = "USER ftp\r\n";
const char ftppasv[] = "PASV\r\n";

// Versionskontroll
extern "C" int FAR PASCAL
FTPDLLVersion(void)
{
  return 0x0110;		// Build 1, version 1.00
}

// Initiera WinSock
extern "C" int FAR PASCAL
FTPInit(void)
{
  WSADATA wsadata;
  if (WSAStartup(0x0101, &wsadata))
    return FTP_WinSockAPIError;

  return FTP_OK;
}

// Avinitiera WinSock
extern "C" int FAR PASCAL
FTPUninit(void)
{
  if (WSACleanup())
    return FTP_WinSockAPIError;

  return FTP_OK;
}

// Anslut till FTP-server
extern "C" int FAR PASCAL
FTPConnect(SOCKET FAR * contrconn, char FAR * server, u_short portno)
{
  char buf[1024];
  int recvlen;

  // Kontrollera argumenten
  if (contrconn == NULL)
    return FTP_BadArgument;
  if (server == NULL)
    return FTP_BadArgument;
  if (*server == NULL)
    return FTP_BadArgument;
  if (portno == 0)
    portno = 21;

  //////////////// Creating socket for control connection //////

  if (((*contrconn = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET))
    return FTP_CantCreateSocket;

  /////////////////////// Connect control connection ///////////

  sockaddr_in contraddr;
  contraddr.sin_family = AF_INET;
  contraddr.sin_port = htons(portno);
  contraddr.sin_addr.s_addr = inet_addr(server);
  zero(contraddr.sin_zero);

  if (contraddr.sin_addr.s_addr == INADDR_NONE)
    {
      hostent *hostinfo;

      if ((hostinfo = gethostbyname(server)) == NULL)
	return FTP_ServerNotFound;

      contraddr.sin_addr.s_addr = *(u_long *) (hostinfo->h_addr_list[0]);

      if (contraddr.sin_addr.s_addr == INADDR_NONE)
	return FTP_ServerNotFound;
    }

  if (connect(*contrconn, (sockaddr *) & contraddr, sizeof(contraddr)) ==
      SOCKET_ERROR)
    return FTP_CantConnect;

  //////////////// Wait for Welcome Message ////////////////////

  if ((~waituntilready(*contrconn, 10, SOCK_RTR | SOCK_EXC)) & SOCK_RTR)
    return FTP_InvalidResponse;

  if ((recvlen = recv(*contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
    return FTP_ReceiveError;

  buf[recvlen] = NULL;
  if (recvlen < 4)
    return FTP_InvalidResponse;
  else if (strncmp(buf, "220", 3))
    return FTP_InvalidResponse;

  return FTP_OK;
}

// Koppla från FTP-anslutning
extern "C" int FAR PASCAL
FTPDisconnect(SOCKET s)
{
  return closesocket(s);
}

// Logga in på FTP-server
extern "C" int FAR PASCAL
FTPLogin(SOCKET contrconn, char FAR * username, char FAR * password)
{
  char buf[1024];
  int recvlen;

  //////////////// Send User Name //////////////////////////////

  while (waituntilready(contrconn, 1, SOCK_RTR) & SOCK_RTR)
    {
      Sleep(1);
      if ((recvlen =
	   recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
	return FTP_ReceiveError;
    }

  if ((~waituntilready(contrconn, 10, SOCK_RTS)) & SOCK_RTS)
    return FTP_TimeOut;

  if (username)
    if (*username)
      {
	strcpy(buf, "USER ");
	strncat(buf, username, sizeof(buf)-10);
	buf[sizeof(buf)-6] = 0;
	strcat(buf, "\r\n");
      }
    else
      strcpy(buf, ftpuser);
  else
    strcpy(buf, ftpuser);

  if (send(contrconn, buf, (int) strlen(buf), 0) != (int)strlen(buf))
    return FTP_SendError;

  //////////////// Recieve User Name response //////////////////

  if ((~waituntilready(contrconn, 30, SOCK_RTR)) & SOCK_RTR)
    return FTP_TimeOut;

  if ((recvlen = recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
    return FTP_ReceiveError;

  buf[recvlen] = NULL;
  if (recvlen < 4)
    return FTP_InvalidResponse;
  else if (strncmp(buf, "331", 3))
    return FTP_LoginNotAllowed;

  //////////////// Send password ///////////////////////////////

  int readyflag;
  while ((readyflag = waituntilready(contrconn, 10, SOCK_ANY)) & SOCK_RTR)
    {
      Sleep(1);
      if ((recvlen =
	   recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
	return FTP_ReceiveError;
    }

  if ((~readyflag) & SOCK_RTS)
    return FTP_TimeOut;

  strcpy(buf, "PASS ");

  if (password)
    {
      strncat(buf, password, sizeof(buf)-10);
      buf[sizeof(buf)-6] = 0;
    }

  strcat(buf, "\r\n");

  if (send(contrconn, buf, (int) strlen(buf), 0) != (int)strlen(buf))
    return FTP_SendError;

  //////////////// Recieve password response ///////////////////

  if ((~waituntilready(contrconn, 30, SOCK_RTR)) & SOCK_RTR)
    return FTP_TimeOut;

  if ((recvlen = recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
    return FTP_ReceiveError;

  buf[recvlen] = NULL;
  if (recvlen < 4)
    return FTP_InvalidResponse;
  else if (strncmp(buf, "230", 3))
    return FTP_AccessDenied;

  return FTP_OK;
}

// Hämta fil från FTP-server
extern "C" int FAR PASCAL
FTPRetrieve(SOCKET contrconn, char FAR * remotefile, char FAR * localfile,
	    char FAR * filetype)
{
  char buf[1024];
  int recvlen;

  if (remotefile == NULL)
    return FTP_BadArgument;
  if (*remotefile == NULL)
    return FTP_BadArgument;
  if (localfile == NULL)
    return FTP_BadArgument;
  if (*localfile == NULL)
    return FTP_BadArgument;

  //////////////// Creating socket for data connection /////////

  SOCKET dataconn;
  if (((dataconn = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET))
    return FTP_CantCreateSocket;

  //////////////// Creating output file ////////////////////////

  HANDLE hFile = CreateFile(localfile, GENERIC_WRITE,
			    FILE_SHARE_READ, NULL, CREATE_NEW,
			    FILE_ATTRIBUTE_NORMAL, NULL);

  if (hFile == INVALID_HANDLE_VALUE)
    {
      closesocket(dataconn);
      return FTP_CantOpenLocalFile;
    }

  if (filetype)
    if (*filetype)
      {
	//////////////// Send TYPE request ///////////////////////////

	while (waituntilready(contrconn, 1, SOCK_RTR) & SOCK_RTR)
	  {
	    Sleep(1);
	    if ((recvlen =
		 recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
	      {
		closesocket(dataconn);
		CloseHandle(hFile);
		return FTP_ReceiveError;
	      }
	  }

	if ((~waituntilready(contrconn, 10, SOCK_RTS)) & SOCK_RTS)
	  {
	    closesocket(dataconn);
	    CloseHandle(hFile);
	    return FTP_TimeOut;
	  }

	strcpy(buf, "TYPE ");
	strncat(buf, filetype, sizeof(buf)-10);
	buf[sizeof(buf)-6] = 0;
	strcat(buf, "\r\n");
	if (send(contrconn, buf, (int) strlen(buf), 0) != (int)strlen(buf))
	  {
	    closesocket(dataconn);
	    CloseHandle(hFile);
	    return FTP_SendError;
	  }

	//////////////// Recieve TYPE response ///////////////////////

	if ((~waituntilready(contrconn, 30, SOCK_RTR)) & SOCK_RTR)
	  {
	    closesocket(dataconn);
	    CloseHandle(hFile);
	    return FTP_TimeOut;
	  }

	if ((recvlen =
	     recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
	  {
	    closesocket(dataconn);
	    CloseHandle(hFile);
	    return FTP_ReceiveError;
	  }

	buf[recvlen] = NULL;
	if (recvlen < 4)
	  {
	    closesocket(dataconn);
	    CloseHandle(hFile);
	    return FTP_InvalidResponse;
	  }
	else if (strncmp(buf, "200", 3))
	  {
	    closesocket(dataconn);
	    CloseHandle(hFile);
	    return FTP_TypeNotAccepted;
	  }
      }

  //////////////// Send PASV request ///////////////////////////

  while (waituntilready(contrconn, 1, SOCK_RTR) & SOCK_RTR)
    {
      Sleep(1);
      if ((recvlen =
	   recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
	{
	  closesocket(dataconn);
	  CloseHandle(hFile);
	  return FTP_ReceiveError;
	}
    }

  if ((~waituntilready(contrconn, 10, SOCK_RTS)) & SOCK_RTS)
    {
      closesocket(dataconn);
      CloseHandle(hFile);
      return FTP_TimeOut;
    }

  if (send(contrconn, ftppasv, sizeof(ftppasv) - 1, 0) != sizeof(ftppasv) - 1)
    {
      closesocket(dataconn);
      CloseHandle(hFile);
      return FTP_SendError;
    }

  //////////////// Recieve PASV response ///////////////////////

  if ((~waituntilready(contrconn, 30, SOCK_RTR)) & SOCK_RTR)
    {
      closesocket(dataconn);
      CloseHandle(hFile);
      return FTP_TimeOut;
    }

  if ((recvlen = recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
    {
      closesocket(dataconn);
      CloseHandle(hFile);
      return FTP_ReceiveError;
    }

  buf[recvlen] = NULL;
  if (recvlen < 4)
    {
      closesocket(dataconn);
      CloseHandle(hFile);
      return FTP_InvalidResponse;
    }
  else if (strncmp(buf, "227", 3))
    {
      closesocket(dataconn);
      CloseHandle(hFile);
      return FTP_PassiveNotAccepted;
    }

  /////////////////////// Interprete PASV response /////////////

  u_long dataip;
  u_short dataport;
  u_short a, b, c, d, e, f;

  sockaddr_in dataaddr;

  char *chrptr;
  if ((chrptr = strchr(buf, '(')) == NULL)
    {
      closesocket(dataconn);
      CloseHandle(hFile);
      return FTP_InvalidResponse;
    }

  sscanf(++chrptr, "%hu,%hu,%hu,%hu,%hu,%hu", &a, &b, &c, &d, &e, &f);

  dataip = (u_long) a << 24;
  dataip |= (u_long) b << 16;
  dataip |= (u_long) c << 8;
  dataip |= (u_long) d;
  dataport = (u_short) (e << 8);
  dataport |= (u_short) f;

  dataaddr.sin_family = AF_INET;
  dataaddr.sin_addr.s_addr = htonl(dataip);
  dataaddr.sin_port = htons(dataport);
  zero(dataaddr.sin_zero);

  //////////////// Connect passive data connection /////////////

  if (connect(dataconn, (sockaddr *) & dataaddr, sizeof(dataaddr)) ==
      SOCKET_ERROR)
    {
      closesocket(dataconn);
      CloseHandle(hFile);
      return FTP_CantConnect;
    }

  //////////////// Send RETR request ///////////////////////////

  int readyflag;
  while ((readyflag = waituntilready(contrconn, 10, SOCK_ANY)) & SOCK_RTR)
    {
      Sleep(1);
      if ((recvlen =
	   recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
	{
	  closesocket(dataconn);
	  CloseHandle(hFile);
	  return FTP_ReceiveError;
	}
    }

  if ((~readyflag) & SOCK_RTS)
    {
      closesocket(dataconn);
      CloseHandle(hFile);
      return FTP_TimeOut;
    }

  _snprintf(buf, sizeof buf, "RETR %s\r\n", remotefile);
  buf[sizeof(buf)-1] = 0;
  if (send(contrconn, buf, (int) strlen(buf), 0) != (int)strlen(buf))
    {
      closesocket(dataconn);
      CloseHandle(hFile);
      return FTP_SendError;
    }

  //////////////// Recieve RETR response ///////////////////////

  if ((~waituntilready(contrconn, 30, SOCK_RTR)) & SOCK_RTR)
    {
      closesocket(dataconn);
      CloseHandle(hFile);
      return FTP_TimeOut;
    }

  if ((recvlen = recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
    {
      closesocket(dataconn);
      CloseHandle(hFile);
      return FTP_ReceiveError;
    }

  buf[recvlen] = NULL;
  if (recvlen < 4)
    {
      closesocket(dataconn);
      CloseHandle(hFile);
      return FTP_InvalidResponse;
    }
  else if (strncmp(buf, "550", 3) == 0)
    {
      closesocket(dataconn);
      CloseHandle(hFile);
      return FTP_AccessDenied;
    }
  else if (strncmp(buf, "125", 3))
    if (strncmp(buf, "150", 3))
      {
	closesocket(dataconn);
	CloseHandle(hFile);
	return FTP_CommandNotAccepted;
      }

  //////////////// Recieve from data connection ////////////////

  // While the data connection is receive ready and we receive
  // something but NULL
  while ((readyflag =
	  waituntilready(dataconn, 10, SOCK_RTR | SOCK_EXC)) & SOCK_RTR)
    {
      // Wait 1 millisecond
      Sleep(1);

      // Receive the packet
      if ((recvlen = recv(dataconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
	{
	  closesocket(dataconn);
	  CloseHandle(hFile);
	  return FTP_ReceiveError;
	}

      // If we received EOF, then the transfer is complete.
      if (recvlen < 1)
	break;

      // Write the data to the output file
      DWORD dwWriteLen;
      WriteFile(hFile, buf, recvlen, &dwWriteLen, NULL);

      if (dwWriteLen != (DWORD)recvlen)
	{
	  closesocket(dataconn);
	  CloseHandle(hFile);
	  return FTP_FileIOError;
	}
    }

  if (readyflag & SOCK_EXC)
    {
      closesocket(dataconn);
      CloseHandle(hFile);
      return FTP_ReceiveError;
    }

  // Now close the data connection and output file
  closesocket(dataconn);
  CloseHandle(hFile);

  //////////////// Receive transfer complete on control ////////

  while (waituntilready(contrconn, 0, SOCK_RTR))
    if ((recvlen = recv(contrconn, buf, sizeof(buf) - 1, 0)) == SOCKET_ERROR)
      return FTP_ReceiveError;

  buf[recvlen] = NULL;
  if (recvlen < 4)
    return FTP_InvalidResponse;
  // Should be "226 Transfer Complete"
  else if (strncmp(buf, "226", 3))
    return FTP_TransferNotComplete;

  return FTP_OK;
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
	 (events & SOCK_RTR) ? &recvfds : NULL,
	 (events & SOCK_RTS) ? &sendfds : NULL,
	 (events & SOCK_EXC) ? &errfds : NULL, &tv);
  if (!FD_ISSET(s, &recvfds))
    events &= ~SOCK_RTR;
  if (!FD_ISSET(s, &sendfds))
    events &= ~SOCK_RTS;
  if (!FD_ISSET(s, &errfds))
    events &= ~SOCK_EXC;
  return events;
}
