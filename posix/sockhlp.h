#include <sys/types.h>
#include <netinet/in.h>

int ConnectTCP(const char *szServer, const char *szService);
int ConnectTCP(const char *szServer, u_short usPort);
int ConnectTCP(u_long ulAddress, u_short usPort);

