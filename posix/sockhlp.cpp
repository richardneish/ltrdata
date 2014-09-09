#include "sockhlp.h"

/*
 * sockhlp.cpp
 *
 * Socket help functions, *nix edition.
 * Copyright (C) Olof Lagerkvist
 */

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int ConnectTCP(const char *szServer, const char *szService)
{
   if (szServer == NULL)
      return -1;

   if (szService == NULL)
      return -1;

   u_short usPort = htons((u_short)strtoul(szService, NULL, 0));
   if (usPort == 0)
   {
      // Get port name for service
      servent *service = getservbyname(szService, "tcp");
      if (!service)
         return -1;

      usPort = service->s_port;
   }

   return ConnectTCP(szServer, usPort);
}

int ConnectTCP(const char *szServer, u_short usPort)
{
   if (szServer == NULL)
      return -1;

   if (usPort == 0)
      return -1;

   // Get server address
   u_long haddr = inet_addr(szServer);

   // Wasn't IP? Lookup host.
   if (haddr == INADDR_NONE)
   {
      hostent *hent = gethostbyname(szServer);
      if (!hent)
         return -1;

      haddr = *(u_long*)hent->h_addr;
   }

   return ConnectTCP(haddr, usPort);
}

int ConnectTCP(u_long ulAddress, u_short usPort)
{
   // Open socket
   int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (sd == -1)
      return -1;

   sockaddr_in addr;
   addr.sin_family = AF_INET;
   addr.sin_port = usPort;
   addr.sin_addr.s_addr = ulAddress;

   bzero(addr.sin_zero, sizeof(addr.sin_zero));

   if (connect(sd, (sockaddr*)&addr, sizeof addr) == -1)
   {
      close(sd);
      return -1;
   }

   return sd;
}

