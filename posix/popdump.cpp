#ifdef _WIN32
   // Win32 header files
#  define WIN32_LEAN_AND_MEAN
#  include <stdlib.h>

#  include <winsock.h>
#  include <winstrct.h>
#  include <wsocket.h>
#  include <wio.h>
#  include <wfilever.h>

#  ifdef _DLL
#    pragma comment(lib, "minwcrt.lib")
#  endif

#  pragma comment(lib, "kernel32.lib")
#  pragma comment(lib, "wsock32.lib")
#  pragma comment(lib, "version.lib")

#  define StrSend (ol.StrSend)

  char cFileNameBuf[MAX_PATH];
#else
   // Unix header files
#  include <stdio.h>
#  include <errno.h>
#  include <stdlib.h>
#  include <strings.h>
#  include <unistd.h>
#  include <netdb.h>
#  include <sys/types.h>
#  include <netinet/in.h>
#  include <sys/socket.h>
#  include <arpa/inet.h>
#  include <sys/wait.h>
#  include <sys/stat.h>
#  include <fcntl.h>

#  include "iohlp.h"
#  include "sockhlp.h"

// Windows-like function and type names
#  define closesocket(s) close(s)
#  define h_perror(s) perror(s)
typedef int SOCKET;
typedef int HANDLE;
typedef unsigned long DWORD;
#  define MAX_PATH 260
#  ifndef INADDR_NONE
#     define INADDR_NONE (-1L)
#  endif
#  define INVALID_SOCKET (-1)
#  define INVALID_HANDLE_VALUE (-1)
#  define SOCKET_ERROR (-1)
#  define Sleep(n) sleep(n)
#  define win_perror perror
#  define WriteFile(a,b,c,d,e) write(a,b,c)
#  define CloseHandle close
#endif

#define MAX_LINE        32768

#define szPOPServer     argv[1]
#define szPOPUsername   argv[2]
#define szPOPPassword   argv[3]

char cBuff[MAX_LINE];

main(int argc, char **argv)
{
  puts("POPDUMP, by Olof Lagerkvist.\r\n"
       "http://www.ltr-data.se");

#ifdef _WIN32
  // About message from file version resource
  WFileVerInfo *pfvi = new WFileVerInfo;
  if (*pfvi)
    {
      char *pcDescr = (char*)pfvi->QueryValue();
      char *pcVer =
	(char*)pfvi->QueryValue("\\StringFileInfo\\040904E4\\FileVersion");
      if ((pcDescr != NULL) & (pcVer != NULL))
	printf("%s ver %s, compile date %s\n", pcDescr, pcVer, __DATE__);
      else
	printf("Compile date: %s\n", __DATE__);
      delete pfvi;
    }
#else
  printf("Compile date: %s\n", __DATE__);
#endif

  if ((argc < 4) | (argc > 4))
    {
      puts
	("Utility to receive messages from a POP server and save them as "
	 ".eml files.\r\n"
	 "\r\n"
	 "Usage:\r\n"
	 "popdump server username password");
      return 0;
    }

#ifdef _WIN32
#if 0
{
#endif
  WSADATA wd;
  if (WSAStartup(0x0101, &wd))
    {
      fputs("Winsock initialization failure.\r\n", stderr);
      return -1;
    }

  WOverlapped ol;
#endif

  if (strlen(szPOPServer) + strlen(szPOPUsername) + 100 > sizeof cBuff)
    {
      fputs("Too long server name or user name.\r\n", stderr);
      return -1;
    }

  printf("Connecting to POP server %s...\n", szPOPServer);
  HANDLE hPOP = (HANDLE)ConnectTCP(szPOPServer, "pop3");
  if (hPOP == INVALID_HANDLE_VALUE)
    {
      h_perror(NULL);
      return 1;
    }

  puts("Connected to POP server.");

  LineRecv(hPOP, cBuff, sizeof cBuff);
  puts(cBuff);
  if (cBuff[0] != '+')
    {
      fputs("Unknown response from server.\r\n", stderr);
      return 1;
    }

  puts("Sending POP user name...");
  if (!StrSend(hPOP, "USER "))
    return 1;
  if (!StrSend(hPOP, szPOPUsername))
    return 1;
  if (!StrSend(hPOP, "\r\n"))
    return 1;
  do
    {
      if (LineRecv(hPOP, cBuff, sizeof cBuff) == 0)
	return 1;

      puts(cBuff);

      if (cBuff[0] == '-')
	return 1;
    } while (cBuff[0] != '+');

  puts("Sending POP password...");
  if (!StrSend(hPOP, "PASS "))
    return 1;
  if (!StrSend(hPOP, szPOPPassword))
    return 1;
  if (!StrSend(hPOP, "\r\n"))
    return 1;
  do
    {
      if (LineRecv(hPOP, cBuff, sizeof cBuff) == 0)
	return 1;

      puts(cBuff);

      if (cBuff[0] == '-')
	return 1;
    } while (cBuff[0] != '+');

  if (!StrSend(hPOP, "STAT\r\n"))
    return 1;

  if (LineRecv(hPOP, cBuff, sizeof cBuff) == 0)
    return 1;

  puts(cBuff);
  if (cBuff[0] == '-')
    return 1;

  if (strtok(cBuff, " ") == NULL)
    return 1;

  char *szNumMail = strtok(NULL, " ");
  if (szNumMail == NULL)
    return 1;

  DWORD dwNumMail = strtoul(szNumMail, NULL, 10);
  if (dwNumMail == 0)
    {
      puts("No mail.");
      return 0;
    }

  char *szBytes = strtok(NULL, " ");
  if (szBytes == NULL)
    return 1;

  DWORD dwBytes = strtoul(szBytes, NULL, 10);

  printf("Found %u mail, %u bytes total.\n", dwNumMail, dwBytes);

  for (DWORD dwMailCounter = 1; dwMailCounter <= dwNumMail; dwMailCounter++)
    {
      printf("Receiving mail %u...\n", dwMailCounter);
      sprintf(cBuff, "RETR %u\r\n", dwMailCounter);

      StrSend(hPOP, cBuff);
      LineRecv(hPOP, cBuff, sizeof cBuff);
      puts(cBuff);
      if (cBuff[0] != '+')
	break;

      sprintf(cBuff, "%s@%s[%u].eml", szPOPUsername, szPOPServer,
	       dwMailCounter);
      printf("Saving file %s...\n", cBuff);

#ifdef _WIN32
      HANDLE hFile = CreateFile(cBuff, GENERIC_WRITE, 0, NULL,
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#else
      int hFile = open(cBuff, O_WRONLY|O_CREAT|O_TRUNC);
#endif

      if (hFile == INVALID_HANDLE_VALUE)
	{
	  win_perror(cBuff);
	  break;
	}

      for(;;)
	{
	  DWORD dwLineLength = LineRecv(hPOP, cBuff, sizeof(cBuff)-3);
	  if ((cBuff[0] == '.') & (cBuff[1] == 0))
            break;

	  strcat(cBuff, "\r\n");
	  if (!WriteFile(hFile, cBuff, dwLineLength+2, &dwLineLength, NULL))
	    {
	      win_perror(NULL);
	      return 1;
	    }

	  Sleep(0);
	}

      CloseHandle(hFile);

      puts("Deleting mail on POP server...");
      sprintf(cBuff, "DELE %u\r\n", dwMailCounter);

      StrSend(hPOP, cBuff);
      LineRecv(hPOP, cBuff, sizeof cBuff);
      puts(cBuff);
      if (cBuff[0] != '+')
	break;

      Sleep(0);
    }

  StrSend(hPOP, "QUIT\r\n");
  LineRecv(hPOP, cBuff, sizeof cBuff);
  puts(cBuff);
}

