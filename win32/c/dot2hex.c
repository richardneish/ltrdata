#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock.h>
#include <stdio.h>

#pragma comment(lib, "wsock32")

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

int main(int argc, char **argv)
{
   IN_ADDR addr;
   addr.s_addr = inet_addr(argv[1]?argv[1]:"0");

   printf("IP#: %s\n"
	  "Hex: %p\n"
	  "Dec: %u\n",
	  inet_ntoa(addr), ntohl(addr.s_addr), ntohl(addr.s_addr));

   return 0;
}
