#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <shellapi.h>
#include <winioctl.h>

#pragma comment(lib, "shell32")

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

int main(int argc, char **argv)
{
  HANDLE hDev = CreateFile(argv[1], GENERIC_READ|GENERIC_WRITE,
			   FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
			   OPEN_EXISTING, 0, NULL);
  DWORD dwZ;

  if (hDev == INVALID_HANDLE_VALUE)
    {
      win_perror("Cannot access volume");
      return 2;
    }

  if (!DeviceIoControl(hDev, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &dwZ, 
		       NULL))
    {
      win_perror("Volume dismount failed");
      return 1;
    }
  else
    {
      puts("Volume dismounted successfully.");
      return 0;
    }
}

