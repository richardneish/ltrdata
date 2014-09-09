#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <winioctl.h>

int main(int argc, LPSTR *argv)
{
  DISK_GEOMETRY DiskGeometry;
  DWORD dw;
  HANDLE h;

  SetErrorMode(SEM_FAILCRITICALERRORS);

  h = CreateFile(argv[1], 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		 OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

  if (h == INVALID_HANDLE_VALUE)
    {
      win_perror(NULL);
      return 1;
    }

  if (!DeviceIoControl(h, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0,
		       &DiskGeometry, sizeof DiskGeometry, &dw, NULL))
    {
      win_perror(NULL);
      return 1;
    }

  printf("Cylinders: %I64i\n"
	 "Tracks per cylinder: %u\n"
	 "Sectors per track: %u\n"
	 "Bytes per sector: %u\n",
	 DiskGeometry.Cylinders.QuadPart,
	 DiskGeometry.TracksPerCylinder,
	 DiskGeometry.SectorsPerTrack,
	 DiskGeometry.BytesPerSector);
}
