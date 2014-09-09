#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <winstrct.h>
#include <objbase.h>
#include <ws2spi.h>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "ws2_32.lib")

LPCWSTR GetGUIDString(LPGUID pguid)
{
  static WCHAR buf[80];
  if (StringFromGUID2(pguid, (LPOLESTR)buf, sizeof(buf)/sizeof(*buf)) == 0)
    return NULL;
  else
    return buf;
}

int
wmain(int argc, LPWSTR *argv)
{
  BOOL bRemoveMode = FALSE;
  static WSAPROTOCOL_INFOW protocol_info[256];
  DWORD dwSize = sizeof protocol_info;
  INT iErrNo;
  int iSize;

  if (argc > 1)
    if (wcsicmp(argv[1], L"/REMOVE") == 0)
      {
	if (argc < 3)
	  {
	    fputs("Need GUID list as parameter with /REMOVE switch.\r\n",
		  stderr);
	    return 2;
	  }
	bRemoveMode = TRUE;
	argv++;
	argc--;
      }
    else if (argv[1][0] != L'{')
      {
	fputs("Layered Service Providers and Protocols enumeration and remove tool.\r\n"
	      "Version 0.0.1. Copyright (C) 2005, Olof Lagerkvist.\r\n"
	      "This program is open source freeware.\r\n"
	      "http://www.ltr-data.se     olof@ltr-data.se\r\n"
	      "\n"
	      "Syntax:\r\n"
	      "enumproto [/remove] [GUID ...]\r\n"
	      "\n"
	      "Without parameters all protocols are enumerated and the GUID of the provider\r\n"
	      "providing the protocol is displayed.\r\n", stderr);
	return 2;
      }

  iSize = WSCEnumProtocols(NULL, protocol_info, &dwSize, &iErrNo);
  if (iSize == SOCKET_ERROR)
    {
      SetLastError(iErrNo);
      win_perror("Error, WSCEnumProtocols");
      return 1;
    }

  while (iSize--)
    {
      LPCWSTR *cargv = argv;
      LPCWSTR strGUID = GetGUIDString(&protocol_info[iSize].ProviderId);
      if (argc > 1)
	{
	  while (*++cargv != NULL)
	    if (wcscmp(*cargv, strGUID) == 0)
	      {
		oem_printf(stdout,
			   "GUID: %1!ws! Name: '%2!ws!'.\n",
			   strGUID, protocol_info[iSize].szProtocol);
		if (bRemoveMode)
		  if (WSCDeinstallProvider(&protocol_info[iSize].ProviderId,
					   &iErrNo) == SOCKET_ERROR)
		    {
		      oem_printf(stderr,
				 "Error removing provider %1!ws!:\n",
				 strGUID);
		      SetLastError(iErrNo);
		      win_perror(NULL);
		    }
		  else
		    printf("Successfully removed provider: %ws\n", strGUID);
	      }
	}
      else
	oem_printf(stdout,
		   "GUID: %1!ws! Name: '%2!ws!'.%%n",
		   strGUID, protocol_info[iSize].szProtocol);
    }
}
