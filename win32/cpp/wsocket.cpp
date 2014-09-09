#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <winsock.h>
#include <stdlib.h>

#include <wsocket.hpp>

SOCKET
ConnectTCP(LPCWSTR wszServer, u_short usPort)
{
  if (wszServer == NULL)
    return INVALID_SOCKET;

  if (usPort == 0)
    return INVALID_SOCKET;

  LPSTR szServer = WideToByteAlloc(wszServer);

  // Get server address
  u_long haddr = inet_addr(szServer);

  // Wasn't IP? Lookup host.
  if (haddr == INADDR_NONE)
    {
      hostent *hent = gethostbyname(szServer);
      if (hent == NULL)
	{
	  hfree(szServer);
	  return INVALID_SOCKET;
	}

      haddr = *(u_long*)hent->h_addr;
    }

  hfree(szServer);

  return ConnectTCP(haddr, usPort);
}

SOCKET
ConnectTCP(LPCWSTR wszServer, LPCWSTR wszService)
{
  if (wszServer == NULL)
    return INVALID_SOCKET;

  if (wszService == NULL)
    return INVALID_SOCKET;

  u_short usPort = htons((u_short)wcstoul(wszService, NULL, 0));
  if (usPort == 0)
    {
      // Get port name for service
      LPSTR szService = WideToByteAlloc(wszService);
      servent *service = getservbyname(szService, "tcp");
      hfree(szService);
      if (service == NULL)
	return INVALID_SOCKET;

      usPort = service->s_port;
    }

  return ConnectTCP(wszServer, usPort);
}

SOCKET ConnectTCP(LPCSTR szServer, LPCSTR szService)
{
  if (szServer == NULL)
    return INVALID_SOCKET;

  if (szService == NULL)
    return INVALID_SOCKET;

  u_short usPort = htons((u_short)strtoul(szService, NULL, 0));
  if (usPort == 0)
    {
      // Get port name for service
      servent *service = getservbyname(szService, "tcp");
      if (!service)
	return INVALID_SOCKET;

      usPort = service->s_port;
    }

  return ConnectTCP(szServer, usPort);
}

SOCKET ConnectTCP(LPCSTR szServer, u_short usPort)
{
  if (szServer == NULL)
    return INVALID_SOCKET;

  if (usPort == 0)
    return INVALID_SOCKET;

  // Get server address
  u_long haddr = inet_addr(szServer);

  // Wasn't IP? Lookup host.
  if (haddr == INADDR_NONE)
    {
      hostent *hent = gethostbyname(szServer);
      if (!hent)
	return INVALID_SOCKET;

      haddr = *(u_long*)hent->h_addr;
    }

  return ConnectTCP(haddr, usPort);
}

SOCKET ConnectTCP(u_long ulAddress, u_short usPort)
{
  // Open socket
  SOCKET sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sd == INVALID_SOCKET)
    return INVALID_SOCKET;

  sockaddr_in addr = { 0 };
  addr.sin_family = AF_INET;
  addr.sin_port = usPort;
  addr.sin_addr.s_addr = ulAddress;

  if (connect(sd, (sockaddr*)&addr, sizeof addr) == SOCKET_ERROR)
    {
      int __h_errno = WSAGetLastError();
      closesocket(sd);
      WSASetLastError(__h_errno);
      return INVALID_SOCKET;
    }

  return sd;
}
