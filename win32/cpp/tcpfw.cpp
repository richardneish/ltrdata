#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <winstrct.h>
#include <winsock2.h>
#include <wio.h>
#include <wthread.hpp>
#include <wsocket.hpp>
#include <tcproute.hpp>

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 66000

INT WINAPI
AsyncBufferedSwitch(HANDLE hLocal, HANDLE hRemote,
		    LPBYTE pPacket, DWORD dwPacketSize,
		    PULARGE_INTEGER InPacketCount,
		    PULARGE_INTEGER OutPacketCount)
{
  DWORD dwBufferSize = dwPacketSize >> 1;
  LPBYTE pLocalBuffer = pPacket;
  LPBYTE pRemoteBuffer = pPacket + dwBufferSize;
  bool bLocalReadPending = false;
  bool bRemoteReadPending = false;

  WOverlapped olLocal;
  WOverlapped olRemote;
  WOverlapped olSend;

  if ((!olLocal) | (!olRemote) | (!olSend))
    {
      win_perror("AsyncBufferedSwitch: Event creation error");
      return 1;
    }

  if (InPacketCount != NULL)
    InPacketCount->QuadPart = 0;
  if (OutPacketCount != NULL)
    OutPacketCount->QuadPart = 0;

  HANDLE hWait[] = { olLocal.hEvent, olRemote.hEvent };
  bool bCancelling = false;

  while (!bCancelling)
    {
      DWORD dwResult;
      if (!bLocalReadPending)
	{
	  bLocalReadPending = true;
	  olLocal.ResetEvent();
	  if (!ReadFile(hLocal, pLocalBuffer, dwBufferSize, &dwResult,
			&olLocal))
	    switch (GetLastError())
	      {
	      case ERROR_IO_PENDING:
		break;
	      case ERROR_OPERATION_ABORTED:
	      case ERROR_NETNAME_DELETED:
	      case ERROR_BROKEN_PIPE:
	      case ERROR_INVALID_HANDLE:
		return 1;
	      default:
		win_perror("AsyncBufferedSwitch: "
			   "Read failed on local connection");
		return 1;
	      }
	}

      if (!bRemoteReadPending)
	{
	  bRemoteReadPending = true;
	  olRemote.ResetEvent();
	  if (!ReadFile(hRemote, pRemoteBuffer, dwBufferSize, &dwResult,
			&olRemote))
	    switch (GetLastError())
	      {
	      case ERROR_IO_PENDING:
		break;
	      case ERROR_OPERATION_ABORTED:
	      case ERROR_NETNAME_DELETED:
	      case ERROR_BROKEN_PIPE:
	      case ERROR_INVALID_HANDLE:
		return 2;
	      default:
		win_perror("AsyncBufferedSwitch: "
			   "Read failed on remote connection");
		return 2;
	      }
	}

      switch (WaitForMultipleObjects(sizeof(hWait)/sizeof(*hWait), hWait,
				     FALSE, INFINITE))
	{
	case WAIT_OBJECT_0:
	case WAIT_OBJECT_0 + 1:
	  break;
	default:
	  win_perror("AsyncBufferedSwitch: Wait failed");
	  return 0;
	}

      if (!bCancelling)
	{
	  if (olLocal.GetResult(hLocal, &dwResult, FALSE))
	    {
	      if (dwResult == 0)
		bCancelling = true;
	      else
		{
		  if (!olSend.BufSend(hRemote, pLocalBuffer, dwResult))
		    {
		      win_perror("AsyncBufferedSwitch: "
				 "Write error on remote connection");
		      bCancelling = true;
		    }
		  else
		    {
		      if (OutPacketCount != NULL)
			++(OutPacketCount->QuadPart);

		      bLocalReadPending = false;
		    }
		}
	    }
	  else
	    switch (GetLastError())
	      {
	      case ERROR_IO_INCOMPLETE:
		break;
	      case ERROR_OPERATION_ABORTED:
	      case ERROR_NETNAME_DELETED:
	      case ERROR_BROKEN_PIPE:
	      case ERROR_INVALID_HANDLE:
		bCancelling = true;
		break;
	      default:
		win_perror("AsyncBufferedSwitch: "
			   "Read error on local connection");
		bCancelling = true;
		break;
	      }

	  olLocal.ResetEvent();
	}

      if (olRemote.GetResult(hRemote, &dwResult, FALSE))
	{
	  if (dwResult == 0)
	    return 2;
	  else
	    {
	      if (!olSend.BufSend(hLocal, pRemoteBuffer, dwResult))
		{
		  win_perror("AsyncBufferedSwitch: "
			     "Write error on local connection");
		  return 2;
		}

	      if (InPacketCount != NULL)
		++(InPacketCount->QuadPart);
	      
	      bRemoteReadPending = false;
	    }
	}
      else
	switch (GetLastError())
	  {
	  case ERROR_IO_INCOMPLETE:
	    break;
	  case ERROR_OPERATION_ABORTED:
	  case ERROR_NETNAME_DELETED:
	  case ERROR_BROKEN_PIPE:
	  case ERROR_INVALID_HANDLE:
	    return 2;
	  default:
	    win_perror("AsyncBufferedSwitch: Read error on local connection");
	    return 2;
	  }
    }

  return 0;
}

union DataBlock
{
  struct
  {
    SOCKET s1;
    SOCKET s2;
  };

  struct
  {
    HANDLE h1;
    HANDLE h2;
  };
};

UINT
WINAPI
Thread(LPVOID data)
{
  DataBlock *block = (DataBlock *) data;

  BYTE buffer[BUFFER_SIZE];
  ULARGE_INTEGER InCounter;
  ULARGE_INTEGER OutCounter;

  AsyncBufferedSwitch(block->h1, block->h2, buffer, BUFFER_SIZE,
		      &InCounter, &OutCounter);

  printf("Switched %I64u buffers in and %I64u buffers out.\n",
	 InCounter, OutCounter);

  shutdown(block->s1, SD_BOTH);
  CloseHandle(block->h1);
  shutdown(block->s2, SD_BOTH);
  CloseHandle(block->h2);

  delete block;

  return 0;
}

LONG
WINAPI
ExceptionFilter(PEXCEPTION_POINTERS exception_pointers)
{
  if (exception_pointers->ExceptionRecord->ExceptionCode ==
      EXCEPTION_ACCESS_VIOLATION)
    printf("Access violation at %p %s %p.\n",
	   exception_pointers->ExceptionRecord->ExceptionAddress,
	   exception_pointers->ExceptionRecord->ExceptionInformation[0] ?
	   "writing" : "reading",
	   exception_pointers->ExceptionRecord->ExceptionInformation[1]);
  else
    {
      char *msg;
      switch (exception_pointers->ExceptionRecord->ExceptionCode)
	{
	case EXCEPTION_BREAKPOINT:
	case EXCEPTION_SINGLE_STEP:
	  msg = "Breakpoint";
	  break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
	case EXCEPTION_PRIV_INSTRUCTION:
	  msg = "Illegal or privileged instruction";
	  break;
	case EXCEPTION_IN_PAGE_ERROR:
	  msg = "In-page error";
	  break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
	case EXCEPTION_FLT_OVERFLOW:
	case EXCEPTION_FLT_UNDERFLOW:
	  msg = "Division by zero or overflow";
	  break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
	  msg = "Cannot continue after exception";
	  break;
	case EXCEPTION_STACK_OVERFLOW:
	  msg = "Stack overflow";
	  break;
	default:
	  msg = "Unknown exception";
	}
      printf("%s at %p.\n",
	     msg,
	     exception_pointers->ExceptionRecord->ExceptionAddress);
    }

  ExitThread(1);
}

const BOOL bFalse = FALSE;
const BOOL bTrue = TRUE;

int
main(int argc, char **argv)
{
  WSADATA wd;
  if (WSAStartup(0x0101, &wd))
    {
      win_perror();
      return -1;
    }

  if (argc < 4)
    {
      printf("Usage:\n"
	     "\n"
	     "tcpfw [listenaddress] listenport targethost targetport\n"
	     "\n"
	     "Environment variables for proxy connection:\n"
	     "TCPROUTE_PROXYADDRESS = http proxy host address\n"
	     "TCPROUTE_PROXYPORT    = http proxy port number (def 8080)\n"
	     "TCPROUTE_SOCKSADDRESS = socks proxy host address\n"
	     "TCPROUTE_SOCKSPORT    = socks proxy port number (def 1080)\n"
	     "TCPROUTE_SOCKSVERSION = socks protocol version (def 4)\n"
	     "TCPROUTE_LOCAL_LOOKUP = set to 1 for client side resolving\n");
      return 10;
    }

  SOCKET sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sServer == INVALID_SOCKET)
    {
      h_perror();
      return 1;
    }

  setsockopt(sServer, SOL_SOCKET, SO_REUSEADDR, (LPCSTR) &bTrue, sizeof bTrue);

  sockaddr_in sin = { 0 };

  sin.sin_family = AF_INET;

  if (argc > 4)
    {
      sin.sin_addr.s_addr = inet_addr(argv[1]);
      argc--;
      argv++;
    }

  sin.sin_port = htons((u_short) strtol(argv[1], NULL, 0));
  if (sin.sin_port == 0)
    {
      // Get port name for service
      servent *service = getservbyname(argv[1], "tcp");
      if (service == NULL)
	{
	  h_perror();
	  return 2;
	}

      sin.sin_port = service->s_port;
    }

  if (bind(sServer, (PSOCKADDR) &sin, sizeof sin))
    {
      h_perror();
      return 2;
    }

  if (listen(sServer, 1))
    {
      h_perror();
      return 2;
    }

  int sin_length = sizeof sin;
  if (getsockname(sServer, (PSOCKADDR) &sin, &sin_length))
    {
      h_perror();
      return 2;
    }

  char *http_proxy = getenv("TCPROUTE_PROXYADDRESS");
  char *socks_proxy = getenv("TCPROUTE_SOCKSADDRESS");
  char *c_local_lookup = getenv("TCPROUTE_LOCAL_LOOKUP");
  int local_lookup = 0;

  if (c_local_lookup != NULL)
    if (atoi(c_local_lookup) == 1)
      local_lookup = 1;

  SOCKET (*ConnectRoutine)(LPCSTR, LPCSTR) = ConnectTCP;

  if (http_proxy != NULL)
    {
      printf("Using HTTP proxy at %s.\n", http_proxy);
      if (local_lookup)
	{
	  ConnectRoutine = ConnectTCPHttpProxyWithLookup;
	  puts("Resolving destination host names locally.");
	}
      else
	{
	  ConnectRoutine = ConnectTCPHttpProxy;
	  puts("Resolving destination host names at proxy server.");
	}
    }
  else if (socks_proxy != NULL)
    {
      printf("Using SOCKS proxy at %s.\n", socks_proxy);
      if (local_lookup)
	{
	  ConnectRoutine = ConnectTCPSocksProxyWithLookup;
	  puts("Resolving destination host names locally.");
	}
      else
	{
	  ConnectRoutine = ConnectTCPSocksProxy;
	  puts("Resolving destination host names at proxy server.");
	}
    }

  printf("Listening on address %s port %u...\n",
	 inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));

  SetUnhandledExceptionFilter(ExceptionFilter);

  for (;;)
    {
      sin_length = sizeof sin;
      SOCKET sConnection = accept(sServer, (PSOCKADDR) &sin, &sin_length);
      if (sConnection == INVALID_SOCKET)
	{
	  h_perror();
	  break;
	}

      setsockopt(sConnection, IPPROTO_TCP, TCP_NODELAY, (LPCSTR) &bTrue,
		 sizeof bTrue);

      printf("Got connection from %s:%u, connecting to %s:%s...\n",
	     inet_ntoa(sin.sin_addr), ntohs(sin.sin_port),
	     argv[2], argv[3]);

      SOCKET sFWConn = ConnectRoutine(argv[2], argv[3]);
      if (sFWConn == INVALID_SOCKET)
	{
	  h_perror();
	  closesocket(sConnection);
	  continue;
	}

      printf("Connected to %s:%s.\n", argv[2], argv[3]);

      setsockopt(sFWConn, IPPROTO_TCP, TCP_NODELAY, (LPCSTR) &bTrue,
		 sizeof bTrue);

      DataBlock *block = new DataBlock;
      block->s1 = sConnection;
      block->s2 = sFWConn;

      if (!SafeBeginThread(Thread, 0, block))
	{
	  win_perror();
	  closesocket(sConnection);
	  closesocket(sFWConn);
	  delete block;
	  continue;
	}
    }

  return 0;
}
