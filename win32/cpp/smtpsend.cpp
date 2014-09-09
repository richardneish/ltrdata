#ifdef _WIN32
   // Win32 header files
#  define WIN32_LEAN_AND_MEAN
#  include <stdlib.h>

#  include <winsock.h>
#  include <winstrct.h>
#  include <wsocket.h>
#  include <wio.h>
#  include <wfilever.h>
#  include <spsleep.h>

#  if defined(_DLL) && !defined(_WIN64)
#    pragma comment(lib, "minwcrt.lib")
#  endif

#  pragma comment(lib, "kernel32.lib")
#  pragma comment(lib, "wsock32.lib")
#  pragma comment(lib, "version.lib")

#  define LineRecv (ol.LineRecv)
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

char cBuff[MAX_LINE];

int
main(int argc, char **argv)
{
  puts("SMTPSEND, by Olof Lagerkvist.\r\n"
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

  if (argc < 4)
    {
      puts
	("Utility to send an e-mail from standard input to an SMTP server.\r\n"
	 "\r\n"
	 "Usage:\r\n"
	 "smtpsend server heloname sender receipent1 [receipent2 ...]");
      return 0;
    }

  // In case "sender" is blank, pre-scan e-mail header for Reply-To or From
  // lines.
  char *sender = argv[3];
  if (*sender == 0)
    {
    }

#ifdef _WIN32
  WSADATA wd;
  if (WSAStartup(0x0101, &wd))
    {
      fputs("Winsock initialization failure.\r\n", stderr);
      return -1;
    }

  WOverlapped ol;
#endif

  printf("Connecting to SMTP server %s...\n", szPOPServer);
  HANDLE hPOP = (HANDLE)ConnectTCP(szPOPServer, "smtp");
  if (hPOP == INVALID_HANDLE_VALUE)
    {
      h_perror(NULL);
      return 1;
    }

  puts("Connected to SMTP server.");

  LineRecv(hPOP, cBuff, sizeof cBuff);
  puts(cBuff);
  if (strcmp(strtok(cBuff, " "), "220") != 0)
    {
      fputs("Unknown response from server.\r\n", stderr);
      return 1;
    }

  printf("Sending HELO %s...\n", argv[2]);
  if (!StrSend(hPOP, "HELO "))
    return 1;
  if (!StrSend(hPOP, argv[2]))
    return 1;
  if (!StrSend(hPOP, "\r\n"))
    return 1;

  if (LineRecv(hPOP, cBuff, sizeof cBuff) == 0)
    return 1;

  puts(cBuff);

  if (strcmp(strtok(cBuff, " "), "250") != 0)
    {
      fputs("Unknown response from server.\r\n", stderr);
      return 1;
    }

  printf("Sending MAIL FROM:<%s>...\n", argv[3]);
  if (!StrSend(hPOP, "MAIL FROM:<"))
    return 1;
  if (!StrSend(hPOP, sender))
    return 1;
  if (!StrSend(hPOP, ">\r\n"))
    return 1;

  if (LineRecv(hPOP, cBuff, sizeof cBuff) == 0)
    return 1;

  puts(cBuff);

  if (strcmp(strtok(cBuff, " "), "250") != 0)
    {
      fputs("Unknown response from server.\r\n", stderr);
      return 1;
    }

  while (argc > 4)
    {
      printf("Sending RCPT TO:<%s>...\n", argv[4]);
      if (!StrSend(hPOP, "RCPT TO:<"))
	return 1;
      if (!StrSend(hPOP, argv[4]))
	return 1;
      if (!StrSend(hPOP, ">\r\n"))
	return 1;

      argv++;
      argc--;

      if (LineRecv(hPOP, cBuff, sizeof cBuff) == 0)
	return 1;

      puts(cBuff);

      if (strcmp(strtok(cBuff, " "), "250") != 0)
	{
	  fputs("Unknown response from server.\r\n", stderr);
	  return 1;
	}
    }

  if (!StrSend(hPOP, "DATA\r\n"))
    return 1;

  if (LineRecv(hPOP, cBuff, sizeof cBuff) == 0)
    return 1;

  puts(cBuff);

  if (strcmp(strtok(cBuff, " "), "354") != 0)
    {
      fputs("Unknown response from server.\r\n", stderr);
      return 1;
    }

  HANDLE hIn = hStdIn;

  for (;;)
    {
      DWORD dwLineLength;
      if (!ReadFile(hIn, cBuff, sizeof cBuff, &dwLineLength, NULL))
	{
	  win_perror(NULL);
	  return 2;
	}

      if (dwLineLength == 0)
	break;

      if (!BufSend(hPOP, cBuff, dwLineLength))
	{
	  win_perror(NULL);
	  return 1;
	}

      YieldSingleProcessor();
    }

  StrSend(hPOP, ".\r\n");
  LineRecv(hPOP, cBuff, sizeof cBuff);
  puts(cBuff);

  if (strcmp(strtok(cBuff, " "), "250") != 0)
    {
      fputs("Unknown response from server.\r\n", stderr);
      return 1;
    }

  StrSend(hPOP, "QUIT\r\n");
  LineRecv(hPOP, cBuff, sizeof cBuff);
  puts(cBuff);

  return 0;
}
