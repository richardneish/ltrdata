#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#ifndef _DLL
#define _DLL
#endif

#include <winsock2.h>
#include <mswsock.h>
#include <shellapi.h>
#include <winstrct.h>

#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "msvcrt.lib")

int
wmain(int argc, LPWSTR *argv)
{
  HANDLE hFile;
  WSADATA wsadata;
  TRANSMIT_FILE_BUFFERS txbuf;
  PTRANSMIT_FILE_BUFFERS ptxbuf = NULL;
  static char http_header[1024];
  SOCKET hSocket = (SOCKET) GetStdHandle(STD_OUTPUT_HANDLE);

  // Nice argument parse loop :)
  while (argc > 1 ? argv[1][0] ? (argv[1][0]|0x02) == '/' : FALSE : FALSE)
    {
      while ((++argv[1])[0])
	switch (argv[1][0]|0x20)
	  {
	  case 'h':
	    ptxbuf = &txbuf;
	    break;
	  default:
	    fprintf(stderr, "Unknown flag: %c\n", argv[1][0]);
	    return 1;
	  }

      --argc;
      ++argv;
    }

  WSAStartup(0x0002, &wsadata);

  if (argc < 2)
    hFile = GetStdHandle(STD_INPUT_HANDLE);
  else
    hFile = CreateFile(argv[1],
		       GENERIC_READ,
		       FILE_SHARE_READ,
		       NULL,
		       OPEN_EXISTING,
		       FILE_FLAG_SEQUENTIAL_SCAN |
		       FILE_FLAG_NO_BUFFERING,
		       NULL);

  if ((hFile == NULL) | (hFile == INVALID_HANDLE_VALUE))
    {
      if (ptxbuf != NULL)
	win_perror(L"Content-type: text/html\r\n"
		   L"Robots: noindex\r\n"
		   L"Pragma: no-cache\r\n"
		   L"\r\n"
		   L"<html><head><title>Error 500</title></head>\r\n"
		   L"<body>\r\n"
		   L"<h2>HTTP Error 500</h2>\r\n"
		   L"<p><strong>500 Internal Server Error</strong></p>\r\n"
		   L"Cannot open file for sequential reading");
      else
	win_perror(argc > 1 ? argv[1] : L"stdin");
      return 1;
    }

  if (ptxbuf != NULL)
    {
      ULARGE_INTEGER FileSize;
      LPWSTR ext = wcsrchr(argv[1], L'.');
      static char buffer[1024] = "application/octet-stream";
      DWORD dwRecv = sizeof buffer;

      FileSize.LowPart = GetFileSize(hFile, &FileSize.HighPart);
      if (FileSize.LowPart == INVALID_FILE_SIZE)
	if (win_errno != NO_ERROR)
	  FileSize.QuadPart = 0;

      if (ext != NULL)
	{
	  HKEY hKey;
	  if (RegOpenKey(HKEY_CLASSES_ROOT, ext, &hKey) == NO_ERROR)
	    {
	      if (RegQueryValueExA(hKey, "Content Type", NULL, NULL,
				   (LPBYTE) buffer, &dwRecv) == NO_ERROR)
		buffer[dwRecv-1] = '\0';
	      else
		strcpy(buffer, "application/octet-stream");

	      RegCloseKey(hKey);
	    }
	}

      if (FileSize.QuadPart > 0)
	_snprintf(http_header, sizeof http_header,
		  "Content-type: %s\r\n"
		  "Content-length: %.I64u\r\n"
		  "\r\n",
		  buffer, FileSize.QuadPart);
      else
	_snprintf(http_header, sizeof http_header,
		  "Content-type: %s\r\n"
		  "\r\n",
		  buffer);

      http_header[sizeof(http_header)-1] = '\0';

      txbuf.Head = http_header;
      txbuf.HeadLength = (DWORD) strlen(http_header);
      txbuf.Tail = "";
      txbuf.TailLength = 0;
    }

  if (TransmitFile(hSocket, hFile, 0, 0, NULL, ptxbuf, 0))
    {
      if (ptxbuf == NULL)
	fputs("Transmit successful.\r\n", stderr);
      return 0;
    }
  else
    {
      if (ptxbuf != NULL)
	win_perror(L"Robots: noindex\r\n"
		   L"Pragma: no-cache\r\n"
		   L"Content-type: text/html\r\n"
		   L"\r\n"
		   L"<html><head><title>Error 500</title></head>\r\n"
		   L"<body>\r\n"
		   L"<h2>HTTP Error 500</h2>\r\n"
		   L"<p><strong>500 Internal Server Error</strong></p>\r\n"
		   L"File transmission failed");
      else
	win_perror(L"File not transmitted");
      return 1;
    }
}

__declspec(noreturn)
void
wmainCRTStartup()
{
  int argc;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
  if (argv == NULL)
    {  
      MessageBoxA(NULL, "This program requires Windows NT.",
		  "Incompatible Windows version",
		  MB_ICONSTOP);
      ExitProcess((UINT) -1);
    }
  else
    exit(wmain(argc, argv));
}
