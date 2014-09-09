#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

#include <wntsecur.h>

#pragma comment(lib, "advapi32.lib")

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

void Connection(HANDLE);

int
main(int argc, char **argv)
{
  if (argc > 1)
    {
      puts("This program unsecures the current windowstation and desktop so "
	   "that any user\r\n"
	   "can access them and create processes in them.\r\n"
	   "\n"
	   "Syntax:\r\n"
	   "unsecwinsta");
      return 0;
    }

  WSecurityDescriptor secdesc;
  if (!secdesc.SetDacl(TRUE, NULL, FALSE))
    {
      win_perror("unsecwinsta");
      return 1;
    }

  GetDesktopWindow();
  SECURITY_INFORMATION secinfo = DACL_SECURITY_INFORMATION;
  if (!secdesc.ToUserObject(GetProcessWindowStation(), &secinfo))
    {
      win_perror("unsecwinsta: Cannot unsecure windowstation");
      return 1;
    }

  if (!secdesc.ToUserObject(GetThreadDesktop(GetCurrentThreadId()), &secinfo))
    {
      win_perror("unsecwinsta: Cannot unsecure desktop");
      return 1;
    }

  puts("OK.");
  return 0;
}
