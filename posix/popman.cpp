#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
   // WinStructured library header files
#  define WIN32_LEAN_AND_MEAN
#  include <winsock2.h>
#  include <winstrct.h>
#  include <wconsole.h>
#  include <wfilever.h>
#  include <wio.h>
#  include <wsocket.h>
#else
   // Unix header files
#  include <errno.h>
#  include <strings.h>
#  include <unistd.h>
#  include <netdb.h>
#  include <sys/types.h>
#  include <netinet/in.h>
#  include <sys/socket.h>
#  include <arpa/inet.h>
#  include <sys/wait.h>
#  include "iohlp.h"
#  include "sockhlp.h"
   // Windows-like function and type names
#  define closesocket(s) close(s)
#  define h_perror(s) perror(s)
typedef int SOCKET;
typedef int HANDLE;
#  define MAX_PATH 260
#  ifndef INADDR_NONE
#     define INADDR_NONE (-1L)
#  endif
#  define INVALID_SOCKET (-1)
#  define INVALID_HANDLE_VALUE (-1)
#  define SOCKET_ERROR (-1)
#  define Sleep(n) sleep(n)
#  define strcmpi(a,b) strcasecmp(a,b)
#endif

main(int, char **argv)
{
#ifdef _WIN32
   WSADATA wd;
   if( WSAStartup( 0x0101, &wd ) )
     {
       fputs("Winsock initialization failure.", stderr);
       return -1;
     }
#endif

   char *szServer = NULL;
   char *szName = NULL;
   char *szPassword = NULL;

   puts("POPMAN, by Olof Lagerkvist.\r\n"
	"http://www.ltr-data.se");

#ifdef _WIN32
  char cFileNameBuf[MAX_PATH];
  if (GetModuleFileName(NULL, cFileNameBuf,
			sizeof cFileNameBuf))
    {
      // About message from file version resource
      WFileVerInfo *pfvi = new WFileVerInfo(cFileNameBuf);
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

   if (argv[1])
   {
      szName = strtok(argv[1], "@");
      szServer = strtok(NULL, "");
      strtok(szName, ":");
      szPassword = strtok(NULL, "");
   }

   char cServer[MAX_PATH] = "";
   if (szServer == NULL)
     {
       printf("POP server name: ");
       fgets(cServer, sizeof cServer, stdin);
       cServer[strlen(cServer)-1] = 0;
       if (cServer[0] == '\x00')
         return 1;

      szServer = cServer;
   }

   // Connect
   HANDLE s = (HANDLE)ConnectTCP(szServer, htons(110));

   if (s == INVALID_HANDLE_VALUE)
     {
       h_perror("connect() failed");
       return -1;
     }

   char cBuf[MAX_PATH] = "";

   do
   {
      if (LineRecv((HANDLE)s, cBuf, sizeof cBuf) == 0)
	{
	  fputs("Unknown response from server.\r\n", stderr);
	  return 1;
	}

      puts(cBuf);

      if (cBuf[0] == '-')
         return 1;
   } while (cBuf[0] != '+');

   char cName[MAX_PATH] = "";
   if (szName == NULL)
     {
       printf("User name: ");
       fgets(cName, sizeof cName, stdin);
       cName[strlen(cName)-1] = 0;
       szName = cName;
     }

   puts("Sending user name...");
   if (!StrSend(s, "USER "))
      return 1;
   if (!StrSend(s, szName))
      return 1;
   if (!StrSend(s, "\r\n"))
      return 1;
   do
   {
      if (LineRecv(s, cBuf, sizeof cBuf) == 0)
         return 1;

      puts(cBuf);

      if (cBuf[0] == '-')
         return 1;
   } while (cBuf[0] != '+');

   char cPassword[MAX_PATH] = "";
   if (szPassword == NULL)
     {
       printf("Password: ");
#ifdef _WIN32
       wcStdIn.UnsetMode(ENABLE_ECHO_INPUT|ENABLE_LINE_INPUT);
       for(char *ptr = cPassword; ptr < cPassword + sizeof(cPassword)-2; )
	 {
	   DWORD dwReadLen;
	   if (!ReadFile(hStdIn, ptr, 1, &dwReadLen, NULL))
	     return -1;

	   if (dwReadLen != 1)
	     return 1;
	   
	   if ((*ptr == '\r') | (*ptr == '\n'))
	     {
	       *ptr = '\x00';
	       break;
	     }

	   if (*ptr == 8)
	     {
	       if (ptr > cPassword)
		 {
		   fputs("\x08 \x08", stdout);
		   --ptr;
		 }
	       continue;
	     }
	   
	   fputc('*', stdout);
	   ++ptr;
	 }
       wcStdIn.SetMode(ENABLE_ECHO_INPUT|ENABLE_LINE_INPUT);
       puts("");
#else
       fgets(cPassword, sizeof cPassword, stdin);
       cPassword[strlen(cPassword)-1] = 0;
#endif

      szPassword = cPassword;
   }

   puts("Sending password...");
   if (!StrSend(s, "PASS "))
      return 1;
   if (!StrSend(s, szPassword))
      return 1;
   if (!StrSend(s, "\r\n"))
      return 1;
   do
   {
      if (LineRecv(s, cBuf, sizeof cBuf) == 0)
         return 1;

      puts(cBuf);

      if (cBuf[0] == '-')
         return 1;
   } while (cBuf[0] != '+');

   /*
   puts("Server: '" << szServer << "'\r\n"
      "User name: '" << szName << "'\r\n"
      "Password: '" << szPassword << "'");
   */

   if (!StrSend(s, "LIST\r\n"))
      return 1;

   if (LineRecv(s, cBuf, sizeof cBuf) == 0)
      return 1;

   puts(cBuf);

   int iMailCount = 0;
   for(; ; iMailCount++)
   {
      Sleep(0);

      if (LineRecv(s, cBuf, sizeof cBuf) == 0)
         return 1;

      if (cBuf[0] == '.')
         break;

      strtok(cBuf, " ");
      char *msg = strtok(NULL, "");
      printf("Message no %s: %s bytes.\n", cBuf, msg);
   }

   if (iMailCount == 0)
   {
      puts("No messages in mailbox.");
      return 0;
   }

   for(;;)
   {
      puts("Type the number of the message you want to delete,\r\n"
         "or exit when done.");

      Sleep(0);

      fgets(cBuf, sizeof cBuf, stdin);
      cBuf[strlen(cBuf)-1] = 0;

      if (cBuf[0] == '\x00')
         break;

      if (strcmpi(cBuf, "EXIT") == 0)
         break;

      printf("Deleting message no %s\n", cBuf);
      if (!StrSend(s, "DELE "))
         return 1;
      if (!StrSend(s, cBuf))
         return 1;
      if (!StrSend(s, "\r\n"))
         return 1;
      do
      {
	 if (LineRecv(s, cBuf, sizeof cBuf) == 0)
            return 1;

         puts(cBuf);

         if (cBuf[0] == '-')
            break;
      } while (cBuf[0] != '+');
   }

   StrSend(s, "QUIT\r\n");
   while (LineRecv(s, cBuf, sizeof cBuf))
      puts(cBuf);
}

