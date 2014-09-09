#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <winstrct.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

struct
{
  char *szUsername;
  char *szPassword;
  char *szServer;
  char *szPort;
  char *szPath;

  void fill(char *ftpstr)
  {
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

    if (szPort == NULL)
      szPort = "ftp";
  }

  void debug()
  {
    printf("Username = '%s'\n"
	   "Password = '%s'\n"
	   "Server   = '%s'\n"
	   "Port     = '%s'\n"
	   "Path     = '%s'\n",
	   szUsername, szPassword, szServer, szPort, szPath);
  }
} sourcefile, targetfile;

int
main(int argc, char **argv)
{
  bool bNeedHelp = true;

  if (argc > 1)
    bNeedHelp = false;

  if (bNeedHelp)
    {
      fputs("ftpcp - Cross-FTP Copy program by Olof Lagerkvist 2003.\r\n"
	    "http://www.ltr-data.se\r\n"
	    "\n"
	    "ftpcp [-abp] [[username[:password]@]sourcesrv[:port]:]/path/file\r\n"
	    "    [[username[:password]@]targetsrv[:port]:]/path/file\r\n"
	    "\n"
	    "-a      Ascii transfer mode.\r\n"
	    "-b      Binary transfer mode.\r\n"
	    "-p      Normal ftpcp uses PASV for downloads and PORT for uploads.\r\n"
	    "        This flag reverses this.\r\n", stderr);

      return 0;
    }

  sourcefile.fill(argv[1]);

  argc--;
  argv++;

  if (argc < 2)
    targetfile.fill(sourcefile.szPath);
  else
    targetfile.fill(argv[1]);

//   sourcefile

  ExitThread(0);
}
