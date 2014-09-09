#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <stdlib.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

#pragma comment(lib, "advapi32.lib")

char cBuffer[8192];

__declspec(noreturn) void usage()
{
  fputs("Command line tool to use named pipes to transfer data between two\r\n"
	"applications.\r\n"
	"Written by Olof Lagerkvist 2004.\r\n"
	"http://www.ltr-data.se     olof@ltr-data.se\r\n"
	"\n"
	"Syntax:\r\n"
	"fwpipes [-aw] inboundpipe [-aw] outboundpipe\r\n"
	"\n"
	"Creates two named pipes, one inbound and one outbound, waits for\r\n"
	"clients to connect and forward data from the inbound one to the\r\n"
	"outbound.\r\n"
	"\n"
	"The pipe names may be an empty string, \"\", to disable creation\r\n"
	"of a new pipe and to use stdin or stdout instead. Otherwise, to\r\n"
	"create a pipe, the syntax for the pipe name is:\r\n"
	"\\\\.\\pipe\\<pipename>\r\n"
	"where <pipename> may be any valid name of a Windows named pipe,\r\n"
	"i.e. same naming rules as for filenames.\r\n"
	"\n"
	"Switches:\r\n"
	"-a         Creates an anonymous pipe that any user can access.\r\n"
	"-w         Creates a non-buffered network pipe.\r\n",
	stderr);

  exit(0);
}

int
main(int argc, char **argv)
{
  DWORD dwOpenMode = 0;
  HANDLE hSource;
  HANDLE hTarget;
  DWORD dwReadLength;
  DWORD dwWriteLength;
  BOOL bAnonymous = FALSE;
  SECURITY_DESCRIPTOR secdescr;
  SECURITY_ATTRIBUTES secattr;

  if (argc < 2)
    usage();

  // Nice argument parse loop :)
  while (argv[1] ? argv[1][0] ? (argv[1][0]|0x02) == '/' : FALSE : FALSE)
    {
      while( (++argv[1])[0] )
	switch( argv[1][0]|0x20 )
	  {
	  case 'a':
	    bAnonymous = TRUE;
	    break;
	  case 'w':
	    dwOpenMode |= FILE_FLAG_WRITE_THROUGH;
	    break;
	  default:
	    usage();
	  }

      --argc;
      ++argv;
    }

  if (argv[1][0])
    {
      if (bAnonymous)
	{
	  InitializeSecurityDescriptor(&secdescr,
				       SECURITY_DESCRIPTOR_REVISION);
	  SetSecurityDescriptorDacl(&secdescr, TRUE, NULL, FALSE);
	  secattr.nLength = sizeof secattr;
	  secattr.lpSecurityDescriptor = &secdescr;
	  secattr.bInheritHandle = FALSE;
	}

      hSource = CreateNamedPipe(argv[1], PIPE_ACCESS_INBOUND|dwOpenMode, 0,
				PIPE_UNLIMITED_INSTANCES, 0, 0, 0,
				bAnonymous ? &secattr : NULL);
    }
  else
    hSource = GetStdHandle(STD_INPUT_HANDLE);

  if (hSource == INVALID_HANDLE_VALUE)
    {
      win_perror(argv[1]);
      return 1;
    }

  dwOpenMode = 0;
  bAnonymous = FALSE;

  ++argv;
  --argc;
  if (argc < 2)
    usage();

  // Nice argument parse loop :)
  while (argv[1] ? argv[1][0] ? (argv[1][0]|0x02) == '/' : FALSE : FALSE)
    {
      while( (++argv[1])[0] )
	switch( argv[1][0]|0x20 )
	  {
	  case 'a':
	    bAnonymous = TRUE;
	    break;
	  case 'w':
	    dwOpenMode |= FILE_FLAG_WRITE_THROUGH;
	    break;
	  default:
	    usage();
	  }

      --argc;
      ++argv;
    }

  if (argv[1][0])
    {
      if (bAnonymous)
	{
	  InitializeSecurityDescriptor(&secdescr,
				       SECURITY_DESCRIPTOR_REVISION);
	  SetSecurityDescriptorDacl(&secdescr, TRUE, NULL, FALSE);
	  secattr.nLength = sizeof secattr;
	  secattr.lpSecurityDescriptor = &secdescr;
	  secattr.bInheritHandle = FALSE;
	}

      hTarget = CreateNamedPipe(argv[1], PIPE_ACCESS_OUTBOUND|dwOpenMode, 0,
				PIPE_UNLIMITED_INSTANCES, 0, 0, 0,
				bAnonymous ? &secattr : NULL);
    }
  else
    hTarget = GetStdHandle(STD_OUTPUT_HANDLE);

  if (hTarget == INVALID_HANDLE_VALUE)
    {
      win_perror(argv[1]);
      return 1;
    }

  ConnectNamedPipe(hSource, NULL);
  ConnectNamedPipe(hTarget, NULL);

  dwReadLength = sizeof cBuffer;
  while (ReadFile(hSource, cBuffer, dwReadLength, &dwReadLength, NULL))
    {
      if (dwReadLength == 0)
	return 0;

      if (!WriteFile(hTarget, cBuffer, dwReadLength, &dwWriteLength, NULL))
	break;

      if (dwReadLength != dwWriteLength)
	fprintf(stderr, "Warning: Lost %u bytes.\n",
		dwReadLength - dwWriteLength);
    }

  win_perror(NULL);
  return 1;
}
