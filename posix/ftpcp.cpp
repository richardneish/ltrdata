#include <sstream>
#include <iostream>
using namespace std;

#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "iohlp.h"
#include "sockhlp.h"

bool bVerbose = false;
bool bPassive = false;
bool bModeA = false;
bool bModeB = false;

class ConnectionInfo
{
   char *szUsername;
   char *szPassword;
   char *szServer;
   char *szPort;
   const char *szPath;

   public:
   int hSourcePort;      // Used to exchange info about listening source port
   int hTargetAddr;      // Used to exchange info about target's IP address

   void debug()
   {
      cout <<
         "Username = '" << (szUsername ? szUsername : "null") << "'\n"
         "Password = '" << (szPassword ? szPassword : "null") << "'\n"
         "Server   = '" << (szServer   ? szServer   : "null") << "'\n"
         "Port     = '" << (szPort     ? szPort     : "null") << "'\n"
         "Path     = '" << (szPath     ? szPath     : "null") << '\'' << endl;
   }

   void FillOut(char *ftpconnectstr);

   void PasvThread();
   void PortThread();
} sourcefile, targetfile;

void ConnectionInfo::FillOut(char *ftpstr)
{
   if (strchr(ftpstr, ':') == NULL)
   {
      szServer = NULL;
      szPath = ftpstr;
      return;
   }

   szUsername = strtok(ftpstr, "@");
   szServer = strtok(NULL, "");
   if (szServer == NULL)
   {
      szServer = szUsername;
      szUsername = NULL;
   }

   szServer = strtok(szServer, ":");
   szPort = strtok(NULL, ":");
   szPath = strtok(NULL, "");
   if (szPath == NULL)
   {
      szPath = szPort;
      szPort = NULL;
   }
   if (szPath == NULL)
   {
      szPath = szServer;
      szServer = NULL;
   }

   szUsername = strtok(szUsername, ":");
   szPassword = strtok(NULL, "");

   if (szUsername == NULL)
   {
      szUsername = "anonymous";
      szPassword = "@ftpcp";
   }

   if (szPort == NULL)
      szPort = "ftp";
}

void ConnectionInfo::PasvThread()
{
   char cBuffer[2048];

   if (szServer)
   {
      if (bVerbose)
         cout << "Connecting to " << szServer << ':' <<
            szPort << endl;

      int hControl = ConnectTCP(szServer, szPort);
      if (hControl == -1)
         cerr << "Connect to " << szServer << ':' <<
            szPort << " failed." << endl;

      if (bVerbose)
      {
         sockaddr_in sin;
         socklen_t socklen = sizeof sin;
         getpeername(hControl, (sockaddr*)&sin, &socklen);
         cout << "PasvThread() connected to " << inet_ntoa(sin.sin_addr) <<
            ':' << ntohs(sin.sin_port) << "..." << endl;
      }

      while (LineRecv(hControl, cBuffer, sizeof cBuffer))
      {
         if (bVerbose)
            if (cBuffer[3] == '-')
               cout << cBuffer+4 << '\n';
            else
               cout << cBuffer << '\n';

         if (cBuffer[3] == ' ')
            break;
      }

      if (strncmp(cBuffer, "220", 3) != 0)
         exit(1);

      if (bVerbose)
         cout << "USER " << szUsername << endl;

      if (!StrSend(hControl, "USER "))
         exit(1);
      if (!StrSend(hControl, szUsername))
         exit(1);
      if (!StrSend(hControl, "\n"))
         exit(1);

      while (LineRecv(hControl, cBuffer, sizeof cBuffer))
      {
         if (bVerbose)
            if (cBuffer[3] == '-')
               cout << cBuffer+4 << '\n';
            else
               cout << cBuffer << '\n';

         if (cBuffer[3] == ' ')
            break;
      }

      if (strncmp(cBuffer, "331", 3) != 0)
      {
         cerr << "Login to " << szServer << " failed: " << cBuffer << endl;
         exit(1);
      }

      if (bVerbose)
         cout << "PASS ****" << endl;

      if (!StrSend(hControl, "PASS "))
         exit(1);
      if (!StrSend(hControl, szPassword))
         exit(1);
      if (!StrSend(hControl, "\n"))
         exit(1);

      while (LineRecv(hControl, cBuffer, sizeof cBuffer))
      {
         if (bVerbose)
            if (cBuffer[3] == '-')
               cout << cBuffer+4 << '\n';
            else
               cout << cBuffer << '\n';

         if (cBuffer[3] == ' ')
            break;
      }

      if (strncmp(cBuffer, "230", 3) != 0)
      {
         cerr << "Login to " << szServer << " failed: " << cBuffer << endl;
         exit(1);
      }

      if (bModeA)
      {
         if (bVerbose)
            cout << "TYPE A" << endl;

         if (!StrSend(hControl, "TYPE A\n"))
            exit(1);

         while (LineRecv(hControl, cBuffer, sizeof cBuffer))
         {
            if (bVerbose)
               if (cBuffer[3] == '-')
                  cout << cBuffer+4 << '\n';
               else
                  cout << cBuffer << '\n';

            if (cBuffer[3] == ' ')
               break;
         }
      }

      if (bModeB)
      {
         if (bVerbose)
            cout << "TYPE I" << endl;

         if (!StrSend(hControl, "TYPE I\n"))
            exit(1);

         while (LineRecv(hControl, cBuffer, sizeof cBuffer))
         {
            if (bVerbose)
               if (cBuffer[3] == '-')
                  cout << cBuffer+4 << '\n';
               else
                  cout << cBuffer << '\n';

            if (cBuffer[3] == ' ')
               break;
         }
      }

      if (bVerbose)
         cout << "PASV" << endl;

      if (!StrSend(hControl, "PASV\n"))
         exit(1);

      while (LineRecv(hControl, cBuffer, sizeof(cBuffer)-128))
      {
         if (bVerbose)
            if (cBuffer[3] == '-')
               cout << cBuffer+4 << '\n';
            else
               cout << cBuffer << '\n';

         if (cBuffer[3] == ' ')
            break;
      }

      if (strncmp(cBuffer, "227", 3) != 0)
      {
         cerr << "PASV mode failed: " << cBuffer << endl;
         exit(1);
      }

      strtok(cBuffer, "(");
      char *szPortStr = strtok(NULL, ")");

      if (szPortStr == NULL)
      {
         cerr << "Error in PASV response." << endl;
         exit(1);
      }

      if (bVerbose)
         cout << "Ok, PORT string is " << szPortStr << endl;

      strcat(szPortStr, "\n");
      if (!StrSend(hSourcePort, szPortStr))
      {
         perror("Pipe write failed");
         exit(1);
      }

      if (bVerbose)
         cout << "RETR " << szPath << endl;

      if (!StrSend(hControl, "RETR "))
         exit(1);
      if (!StrSend(hControl, szPath))
         exit(1);
      if (!StrSend(hControl, "\n"))
         exit(1);

      while (LineRecv(hControl, cBuffer, sizeof cBuffer))
      {
         if (bVerbose)
            if (cBuffer[3] == '-')
               cout << cBuffer+4 << '\n';
            else
               cout << cBuffer << '\n';

         if (cBuffer[3] == ' ')
            break;
      }

      if ((strncmp(cBuffer, "125", 3) != 0) & (strncmp(cBuffer, "150", 3) != 0)) 
      {
         cerr << "RETR failed: " << cBuffer << endl;
         exit(1);
      }

      while (LineRecv(hControl, cBuffer, sizeof cBuffer))
      {
         if (bVerbose)
            if (cBuffer[3] == '-')
               cout << cBuffer+4 << '\n';
            else
               cout << cBuffer << '\n';

         if (cBuffer[3] == ' ')
            break;
      }

      if (strncmp(cBuffer, "226", 3) != 0)
      {
         cerr << "Transfer not complete." << endl;
         exit(1);
      }

      if (bVerbose)
         cout << "QUIT" << endl;

      if (StrSend(hControl, "QUIT\n"))
         while (LineRecv(hControl, cBuffer, sizeof cBuffer))
         {
            if (bVerbose)
               if (cBuffer[3] == '-')
                  cout << cBuffer+4 << '\n';
               else
                  cout << cBuffer << '\n';

            if (cBuffer[3] == ' ')
               break;
         }

      close(hControl);
   }
   else
   {
      // Local sender
      int hFile;
      if ((szPath[0] == '-') & (szPath[1] == 0))
         hFile = 0;
      else
      {
         if (bVerbose)
            cout << "Opening input file '" << szPath << "'..." << endl;

         hFile = open(szPath, O_RDONLY);
         if (hFile == -1)
         {
            perror(szPath);
            exit(1);
         }
      }

      if (!BufRecv(hTargetAddr, cBuffer, sizeof(in_addr)))
      {
         perror("Pipe read (target connection interface) failed");
         exit(1);
      }

      sockaddr_in addr;
      int hListener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if (hListener == -1)
      {
         perror("Socket failed");
         exit(1);
      }

      bool b = true;
      setsockopt(hListener, SOL_SOCKET, SO_REUSEADDR, (const char*)&b, sizeof(b));

      addr.sin_family = AF_INET;
      addr.sin_port = 0;
      addr.sin_addr.s_addr = *(u_long*)cBuffer;
      bzero(&(addr.sin_zero), sizeof(addr.sin_zero));

      if (bind(hListener, (sockaddr*)&addr, sizeof(addr)) == -1)
      {
         perror("Port bind failed");
         exit(1);
      }

      if (listen(hListener, 1) == -1)
      {
         perror("Listen failed");
         exit(1);
      }

      socklen_t iSockAddrSize = sizeof addr;
      if (getsockname(hListener, (sockaddr*)&addr, &iSockAddrSize) == -1)
      {
         perror("getsockname() failed");
         exit(1);
      }

      strcpy(cBuffer, inet_ntoa(addr.sin_addr));
      int i;
      for (i = 0; cBuffer[i]; i++)
         if (cBuffer[i] == '.')
            cBuffer[i] = ',';

      sprintf(cBuffer+i, ",%i,%i\n",
         (int)((u_char*)&addr.sin_port)[0],
         (int)((u_char*)&addr.sin_port)[1]);

      if (bVerbose)
         cout << "Listening on " << cBuffer << "..." << endl;

      if (!StrSend(hSourcePort, cBuffer))
      {
         perror("Pipe write failed");
         exit(1);
      }

      iSockAddrSize = sizeof addr;
      int hSender = accept(hListener, (sockaddr*)&addr, &iSockAddrSize);
      if (hSender == -1)
	   {
         perror("Accept failed");
   	   exit(1);
	   }

      close(hListener);

      if (bVerbose)
         cout << "Got connection from " << inet_ntoa(addr.sin_addr) << "\n"
            "Uploading " << szPath << "..." << endl;

      for (;;)
      {
         int iReadLen = read(hFile, cBuffer, sizeof cBuffer);
         if (iReadLen < 0)
         {
            perror(szPath);
            exit(1);
         }

         if (iReadLen == 0)
            break;

         if (!BufSend(hSender, cBuffer, iReadLen))
         {
            perror("Not all data was sent");
            exit(1);
         }
      }

      close(hFile);
      close(hSender);
   }
}

void ConnectionInfo::PortThread()
{
   char cBuffer[2048];

   if (szServer)
   {
      if (bVerbose)
         cout << "Connecting to " << szServer << ':' <<
            szPort << endl;

      int hControl = ConnectTCP(szServer, szPort);
      if (hControl == -1)
         cerr << "Connect to " << szServer << ':' <<
            szPort << " failed." << endl;

      if (bVerbose)
      {
         sockaddr_in sin;
         socklen_t socklen = sizeof sin;
         getpeername(hControl, (sockaddr*)&sin, &socklen);
         cout << "PortThread() connected to " << inet_ntoa(sin.sin_addr) <<
            ':' << ntohs(sin.sin_port) << "..." << endl;
      }

      sockaddr_in addr;
      socklen_t iSockAddrLen = sizeof addr;
      if (getsockname(hControl, (sockaddr*)&addr, &iSockAddrLen) == -1)
      {
         perror("getsockname() failed");
         exit(1);
      }

      if (!BufSend(hTargetAddr, &(addr.sin_addr),
         sizeof(addr.sin_addr)))
      {
         perror("Pipe write failed");
         exit(1);
      }

      while (LineRecv(hControl, cBuffer, sizeof cBuffer))
      {
         if (bVerbose)
            if (cBuffer[3] == '-')
               cout << cBuffer+4 << '\n';
            else
               cout << cBuffer << '\n';

         if (cBuffer[3] == ' ')
            break;
      }

      if (strncmp(cBuffer, "220", 3) != 0)
         exit(1);

      if (bVerbose)
         cout << "USER " << szUsername << endl;

      if (!StrSend(hControl, "USER "))
         exit(1);
      if (!StrSend(hControl, szUsername))
         exit(1);
      if (!StrSend(hControl, "\n"))
         exit(1);

      while (LineRecv(hControl, cBuffer, sizeof cBuffer))
      {
         if (bVerbose)
            if (cBuffer[3] == '-')
               cout << cBuffer+4 << '\n';
            else
               cout << cBuffer << '\n';

         if (cBuffer[3] == ' ')
            break;
      }

      if (strncmp(cBuffer, "331", 3) != 0)
      {
         cerr << "Login to " << szServer << " failed: " << cBuffer << endl;
         exit(1);
      }

      if (bVerbose)
         cout << "PASS ****" << endl;

      if (!StrSend(hControl, "PASS "))
         exit(1);
      if (!StrSend(hControl, szPassword))
         exit(1);
      if (!StrSend(hControl, "\n"))
         exit(1);

      while (LineRecv(hControl, cBuffer, sizeof cBuffer))
      {
         if (bVerbose)
            if (cBuffer[3] == '-')
               cout << cBuffer+4 << '\n';
            else
               cout << cBuffer << '\n';

         if (cBuffer[3] == ' ')
            break;
      }

      if (strncmp(cBuffer, "230", 3) != 0)
      {
         cerr << "Login (target server) failed: " << cBuffer << endl;
         exit(1);
      }

      if (bModeA)
      {
         if (bVerbose)
            cout << "TYPE A" << endl;

         if (!StrSend(hControl, "TYPE A\n"))
            exit(1);

         while (LineRecv(hControl, cBuffer, sizeof cBuffer))
         {
            if (bVerbose)
               if (cBuffer[3] == '-')
                  cout << cBuffer+4 << '\n';
               else
                  cout << cBuffer << '\n';

            if (cBuffer[3] == ' ')
               break;
         }
      }

      if (bModeB)
      {
         if (bVerbose)
            cout << "TYPE I" << endl;

         if (!StrSend(hControl, "TYPE I\n"))
            exit(1);

         while (LineRecv(hControl, cBuffer, sizeof cBuffer))
         {
            if (bVerbose)
               if (cBuffer[3] == '-')
                  cout << cBuffer+4 << '\n';
               else
                  cout << cBuffer << '\n';

            if (cBuffer[3] == ' ')
               break;
         }
      }

      if (LineRecv(hSourcePort, cBuffer, sizeof cBuffer) == 0)
      {
         perror("Pipe read (PORT string) failed");
         exit(1);
      }

      if (bVerbose)
         cout << "PORT " << cBuffer << endl;

      if (!StrSend(hControl, "PORT "))
         exit(1);
      if (!StrSend(hControl, cBuffer))
         exit(1);
      if (!StrSend(hControl, "\n"))
         exit(1);

      while (LineRecv(hControl, cBuffer, sizeof(cBuffer)-128))
      {
         if (bVerbose)
            if (cBuffer[3] == '-')
               cout << cBuffer+4 << '\n';
            else
               cout << cBuffer << '\n';

         if (cBuffer[3] == ' ')
            break;
      }

      if (strncmp(cBuffer, "200", 3) != 0)
      {
         cerr << "PORT failed: " << cBuffer << endl;
         exit(1);
      }

      if (bVerbose)
         cout << "STOR " << szPath << endl;

      if (!StrSend(hControl, "STOR "))
         exit(1);
      if (!StrSend(hControl, szPath))
         exit(1);
      if (!StrSend(hControl, "\n"))
         exit(1);

      while (LineRecv(hControl, cBuffer, sizeof cBuffer))
      {
         if (bVerbose)
            if (cBuffer[3] == '-')
               cout << cBuffer+4 << '\n';
            else
               cout << cBuffer << '\n';

         if (cBuffer[3] == ' ')
            break;
      }

      if ((strncmp(cBuffer, "125", 3) != 0) & (strncmp(cBuffer, "150", 3) != 0))
      {
         cerr << "STOR failed: " << cBuffer << endl;
         exit(1);
      }

      while (LineRecv(hControl, cBuffer, sizeof cBuffer))
      {
         if (bVerbose)
            if (cBuffer[3] == '-')
               cout << cBuffer+4 << '\n';
            else
               cout << cBuffer << '\n';

         if (cBuffer[3] == ' ')
            break;
      }

      if (strncmp(cBuffer, "226", 3) != 0)
      {
         cerr << "Transfer not complete." << endl;
         exit(1);
      }

      if (bVerbose)
         cout << "QUIT" << endl;

      if (StrSend(hControl, "QUIT\n"))
         while (LineRecv(hControl, cBuffer, sizeof cBuffer))
         {
            if (bVerbose)
               if (cBuffer[3] == '-')
                  cout << cBuffer+4 << '\n';
               else
                  cout << cBuffer << '\n';

            if (cBuffer[3] == ' ')
               break;
         }

      close(hControl);
   }
   else
   {
      // Local receiver
      int hFile;
      if ((szPath[0] == '-') & (szPath[1] == 0))
         hFile = 1;
      else
      {
         if (bVerbose)
            cout << "Creating output file '" << szPath << "'..." << endl;

         hFile = creat(szPath, 0777);
         if (hFile == -1)
         {
            perror(szPath);
            exit(1);
         }
      }

      int hReceiver = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if (hReceiver == -1)
      {
         perror("Socket failed");
         exit(1);
      }

      char cLocalHost[] = { 127, 0, 0, 1 };
      if (!BufSend(hTargetAddr, cLocalHost, sizeof cLocalHost))
      {
         perror("Pipe write failed");
         exit(1);
      }

      if (LineRecv(hSourcePort, cBuffer, sizeof cBuffer) == 0)
      {
         perror("Pipe read (PORT string) failed");
         exit(1);
      }

      int i1, i2, i3, i4, i5, i6;
      sscanf(cBuffer, "%i,%i,%i,%i,%i,%i\n", &i1, &i2, &i3, &i4, &i5, &i6);

      sockaddr_in addr;
      addr.sin_family = AF_INET;
      ((u_char*)&addr.sin_addr)[0] = i1;
      ((u_char*)&addr.sin_addr)[1] = i2;
      ((u_char*)&addr.sin_addr)[2] = i3;
      ((u_char*)&addr.sin_addr)[3] = i4;
      ((u_char*)&addr.sin_port)[0] = i5;
      ((u_char*)&addr.sin_port)[1] = i6;
      bzero(&(addr.sin_zero), sizeof(addr.sin_zero));

      if (connect(hReceiver, (sockaddr*)&addr, sizeof addr) == -1)
	   {
         perror("Connect failed");
   	   exit(1);
	   }

      if (bVerbose)
         cout << "Connected to " << inet_ntoa(addr.sin_addr) << "\n"
            "Downloading to " << szPath << "..." << endl;

      for (;;)
      {
         int iReadLen = read(hReceiver, cBuffer, sizeof cBuffer);
         if (iReadLen < 0)
         {
            perror("Download error");
            exit(1);
         }

         if (iReadLen == 0)
            break;

         if (!BufSend(hFile, cBuffer, iReadLen))
         {
            perror("Not all data was written to file");
            exit(1);
         }
      }

      close(hFile);
      close(hReceiver);
   }
}

void *sourcethread(void *)
{
   sourcefile.PasvThread();
   return NULL;
}

main(int argc, char **argv)
{
   bool bNeedHelp = false;

   // Nice argument parse loop :)
   if (argv[1] ? argv[1][0] == '-' : false)
      if (argv[1][1] != 0)
         while (argv[1] ? argv[1][0] ? (argv[1][0]) == '-' : false : false)
         {
   	      while ((++argv[1])[0])
            	switch (argv[1][0])
		      	{
   	   	      case 'a':
            	   	bModeA = true;
               	   break;
   	   	      case 'b':
            	   	bModeB = true;
               	   break;
   	   	      case 'p':
            	      bPassive = true;
               	   break;
   	   	      case 'v':
            	   	bVerbose = true;
               	   break;
            	   default:
               		bNeedHelp = true;
      	      }

            --argc;
            ++argv;
         }

   if (bPassive)
   {
      cerr << "-p not implemented yet." << endl;
      return 1;
   }

   if (argc < 2)
      bNeedHelp = true;

   if (bNeedHelp)
   {
      cout <<
         "ftpcp - Cross-FTP file copy utility. (Ver 1.10 " << __DATE__ << ")\n"
         "Copyright (C) Olof Lagerkvist 1998-2003.\n"
         "http://move.to/olof\n"
         "\n"
         "ftpcp [-abpv] [[username[:password]@]sourcesrv[:port]:]/path/file\n"
         "    [[username[:password]@]targetsrv[:port]:]/path/file\n"
         "\n"
         "-a      Ascii transfer mode.\n"
         "-b      Binary transfer mode.\n"
         "-p      Without this flag, ftpcp uses PASV for downloads and PORT\n"
         "        for uploads. The -p flag reverses this.\n"
         "-v      Verbose mode. Displays all server commands and responses and\n"
         "        other useful debug info."
         << endl;

      return 0;
   }

   if (bPassive)
      targetfile.FillOut(argv[1]);
   else
      sourcefile.FillOut(argv[1]);

   argc--;
   argv++;

   if (argc < 2)
      if (bPassive)
         sourcefile.FillOut("-");
      else
         targetfile.FillOut("-");
   else
      if (bPassive)
         sourcefile.FillOut(argv[1]);
      else
         targetfile.FillOut(argv[1]);


   int hPipe[2];
   if (pipe(hPipe))
   {
      perror("Pipe failed");
      return 1;
   }
   sourcefile.hSourcePort = hPipe[1];
   targetfile.hSourcePort = hPipe[0];
   if (pipe(hPipe))
   {
      perror("Pipe failed");
      return 1;
   }
   sourcefile.hTargetAddr = hPipe[0];
   targetfile.hTargetAddr = hPipe[1];

   pthread_t hSourceThread;
   if (pthread_create(&hSourceThread, NULL, sourcethread, NULL))
   {
      perror("Threading failed");
      return 1;
   }

   targetfile.PortThread();

   pthread_join(hSourceThread, NULL);

   return 0;
}

