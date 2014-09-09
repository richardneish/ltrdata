#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma comment(lib, "kernel32")
#pragma comment(lib, "user32")
#pragma comment(lib, "advapi32")

#pragma comment(linker, "/subsystem:windows")
#pragma comment(linker, "/entry:main")

int main()
{
  char buf[260];
  DWORD bufsiz = sizeof(buf);

  if (GetUserName(buf, &bufsiz) == 0)
    return 1;

  MessageBox(NULL, buf, "GetUserName() return data",  MB_ICONINFORMATION);

  ExitProcess(0);
}
