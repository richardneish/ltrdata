#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <shellapi.h>
#include <winioctl.h>
#include <ntddscsi.h>
#include <ntddvol.h>

#pragma pack(push)
#pragma pack(1)
typedef struct _FAT_BPB
{
  WORD BytesPerSector;
  BYTE SectorsPerCluster;
  WORD ReservedSectors;
  BYTE NumberOfFileAllocationTables;
  WORD NumberOfRootEntries;
  WORD NumberOfSectors;
  BYTE MediaDescriptor;
  WORD SectorsPerFileAllocationTable;
  WORD SectorsPerTrack;
  WORD NumberOfHeads;
  union
  {
    struct
    {
      WORD NumberOfHiddenSectors;
      WORD TotalNumberOfSectors;
    } DOS_320;
    struct
    {
      DWORD NumberOfHiddenSectors;
      DWORD TotalNumberOfSectors;
    } DOS_331;
  };
} FAT_BPB, *PFAT_BPB;

typedef struct _EXT_BPB
{
  BYTE PhysicalDriveNumber;
  BYTE CurrentHead;
  BYTE Signature;
  DWORD Id;
  CHAR VolumeLabel[11];
  CHAR SystemId[8];
} EXT_BPB, *PEXT_BPB;

typedef struct _FAT_VBR
{
  BYTE JumpInstruction[3];
  CHAR OEMName[8];
  FAT_BPB BPB;
  EXT_BPB ExtendedBPB;
  BYTE FillData1[512 -
		 3 -
		 8 -
		 sizeof(FAT_BPB) -
		 sizeof(EXT_BPB) -
		 2];
  BYTE Signature[2];
} FAT_VBR, *PFAT_VBR;
#pragma pack(pop)

LPSTR
CleanNonPrintables(LPSTR str,
		   int size)
{
  int i;
  for (i = 0; i < size; i++)
    if ((str[i] & 0xE0)== 0)
      str[i] = 0x20;

  return str;
}

int
main(int argc, char **argv)
{
  DWORD dwIoControlCode = 0;
  BOOL displayhelp = FALSE;
  HANDLE hDev;
  char cBuf[MAX_PATH] = "\\\\.\\";
  DWORD dw;

  if (argc < 3)
    displayhelp = TRUE;
  else if (!stricmp(argv[1], "DISMOUNT"))
    dwIoControlCode = FSCTL_LOCK_VOLUME;
  else if (!stricmp(argv[1], "FORCEDISMOUNT"))
    dwIoControlCode = FSCTL_DISMOUNT_VOLUME;
  else if (!stricmp(argv[1], "FLUSH"))
    dwIoControlCode = 0;
  else if (!stricmp(argv[1], "LOAD"))
    dwIoControlCode = IOCTL_STORAGE_LOAD_MEDIA;
  else if (!stricmp(argv[1], "EJECT"))
    dwIoControlCode = IOCTL_STORAGE_EJECT_MEDIA;
  else if (!stricmp(argv[1], "GEOMETRY"))
    dwIoControlCode = IOCTL_DISK_GET_DRIVE_GEOMETRY;
  else if (!stricmp(argv[1], "SCSIADDRESS"))
    dwIoControlCode = IOCTL_SCSI_GET_ADDRESS;
  else if (!stricmp(argv[1], "EXTEND"))
    dwIoControlCode = FSCTL_EXTEND_VOLUME;
  else if (!stricmp(argv[1], "GROW"))
    dwIoControlCode = IOCTL_DISK_GROW_PARTITION;
  else if (!stricmp(argv[1], "UPDATE"))
    dwIoControlCode = IOCTL_DISK_UPDATE_PROPERTIES;
  else if (!stricmp(argv[1], "OFFLINE"))
    dwIoControlCode = IOCTL_VOLUME_OFFLINE;
  else if (!stricmp(argv[1], "ONLINE"))
    dwIoControlCode = IOCTL_VOLUME_ONLINE;
  else
    displayhelp = TRUE;

  if (displayhelp)
    {
      puts("Command line interface to Win32 API function\r\n"
	   "DeviceIoControl.\r\n"
	   "\n"
	   "Usage:\r\n"
	   "DEVIOCTL DISMOUNT|FORCEDISMOUNT|FLUSH|LOAD|EJECT|\r\n"
	   "              GEOMETRY|SCSIADDRESS|UPDATE|OFFLINE|ONLINE d:\r\n"
	   "\n"
	   "DEVIOCTL EXTEND d: totalnew\r\n"
	   "\"totalnew\" is total new size of filesystem in sectors.\r\n"
	   "\n"
	   "Where d: is a storage device, e.g. A:.\r\n"
	   "\n"
	   "DEVIOCTL GROW PhysicalDriveN partno growbytes\r\n"
	   "\"partno\" is the partition number.\r\n"
	   "\"growbytes\" is the number of bytes to grow.");
      return -1;
    }

  SetErrorMode(SEM_FAILCRITICALERRORS);
  SetProcessShutdownParameters(0x3FF, SHUTDOWN_NORETRY);

  while ((++argv)[1] != NULL)
    {
      if (*argv[1] == 0)
	break;

      strncpy(cBuf+4, argv[1], sizeof(cBuf) - 4);
      cBuf[sizeof(cBuf) - 1] = 0;

      hDev = INVALID_HANDLE_VALUE;
      switch (dwIoControlCode)
	{
	case IOCTL_DISK_GET_DRIVE_GEOMETRY:
	  {
	    FAT_VBR fat_vbr;
	    DISK_GEOMETRY disk_geometry = { 0 };
	    PARTITION_INFORMATION partition_information = { 0 };
	    GET_LENGTH_INFORMATION length_information = { 0 };
	    DWORDLONG total_size;
	    STORAGE_DEVICE_NUMBER storage_device_number;

	    hDev = CreateFile(cBuf, GENERIC_READ,
			      FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			      OPEN_EXISTING, 0, NULL);
	    if (hDev == INVALID_HANDLE_VALUE)
	      break;

	    DeviceIoControl(hDev,
			    IOCTL_DISK_UPDATE_PROPERTIES,
			    NULL,
			    0,
			    NULL,
			    0,
			    &dw,
			    NULL);

	    if (DeviceIoControl(hDev,
				IOCTL_DISK_GET_DRIVE_GEOMETRY,
				NULL,
				0,
				&disk_geometry,
				sizeof disk_geometry,
				&dw,
				NULL))
	      {
		total_size = disk_geometry.Cylinders.QuadPart *
		  disk_geometry.TracksPerCylinder *
		  disk_geometry.SectorsPerTrack *
		  disk_geometry.BytesPerSector;

		printf("Disk CHS geometry:\n"
		       "Media type    : 0x%.2X\n"
		       "Cylinders     : %I64u\n"
		       "Tracks per cyl: %u\n"
		       "Sectors per tr: %u\n"
		       "Bytes per sect: %u\n"
		       "Total CHS size: %I64u (%.4g %s)\n",
		       disk_geometry.MediaType,
		       disk_geometry.Cylinders,
		       disk_geometry.TracksPerCylinder,
		       disk_geometry.SectorsPerTrack,
		       disk_geometry.BytesPerSector,
		       total_size, TO_h(total_size), TO_p(total_size));
	      }
	    else
	      win_perror("IOCTL_DISK_GET_DRIVE_GEOMETRY");

	    if (DeviceIoControl(hDev,
				IOCTL_DISK_GET_LENGTH_INFO,
				NULL,
				0,
				&length_information,
				sizeof length_information,
				&dw,
				NULL))
	      printf("\n"
		     "LBA length    : %I64u (%.4g %s)\n",
		     length_information.Length,
		     TO_h(length_information.Length.QuadPart),
		     TO_p(length_information.Length.QuadPart));

	    if (DeviceIoControl(hDev,
				IOCTL_DISK_GET_PARTITION_INFO,
				NULL,
				0,
				&partition_information,
				sizeof partition_information,
				&dw,
				NULL))
	      printf("\n"
		     "Partition information:\n"
		     "Start offset  : %I64u\n"
		     "Partition size: %I64u (%.4g %s)\n"
		     "Hidden sectors: %u\n"
		     "Partition no  : %u\n"
		     "Partition type: 0x%.2X\n"
		     "Boot indicator: %u\n"
		     "Recognized par: %u\n",
		     partition_information.StartingOffset,
		     partition_information.PartitionLength,
		     TO_h(partition_information.PartitionLength.QuadPart),
		     TO_p(partition_information.PartitionLength.QuadPart),
		     partition_information.HiddenSectors,
		     partition_information.PartitionNumber,
		     (DWORD) partition_information.PartitionType,
		     partition_information.BootIndicator,
		     partition_information.RecognizedPartition);

	    if (DeviceIoControl(hDev,
				IOCTL_STORAGE_GET_DEVICE_NUMBER,
				NULL,
				0,
				&storage_device_number,
				sizeof storage_device_number,
				&dw,
				NULL))
	      printf("\n"
		     "Device type: 0x%.2X\n"
		     "Device number: %i\n"
		     "Partition number: %i\n",
		     storage_device_number.DeviceType,
		     storage_device_number.DeviceNumber,
		     storage_device_number.PartitionNumber);

	    if (ReadFile(hDev, &fat_vbr, sizeof(fat_vbr), &dw, NULL))
	      printf("\n"
		     "Volume boot record values:\n"
		     "OEM name      : %.*s\n"
		     "Bytes per sect: %u\n"
		     "Sectors per cl: %u\n"
		     "Reserved sect : %u\n"
		     "FAT count     : %u\n"
		     "FAT root entr : %u\n"
		     "Total sectors : %u\n"
		     "Media descript: 0x%.2X\n"
		     "Sectors pr FAT: %u\n"
		     "Sect per track: %u\n"
		     "Number of head: %u\n"
		     "Physicl drv no: 0x%.2X\n"
		     "Signature     : 0x%.2X\n"
		     "Id            : 0x%.8X\n"
		     "Volume label  : %.*s\n"
		     "System Id     : %.*s\n",
		     sizeof(fat_vbr.OEMName),
		     CleanNonPrintables(fat_vbr.OEMName,
					sizeof(fat_vbr.OEMName)),
		     (UINT)fat_vbr.BPB.BytesPerSector,
		     (UINT)fat_vbr.BPB.SectorsPerCluster,
		     (UINT)fat_vbr.BPB.ReservedSectors,
		     (UINT)fat_vbr.BPB.NumberOfFileAllocationTables,
		     (UINT)fat_vbr.BPB.NumberOfRootEntries,
		     (UINT)fat_vbr.BPB.NumberOfSectors,
		     (INT)fat_vbr.BPB.MediaDescriptor,
		     (UINT)fat_vbr.BPB.SectorsPerFileAllocationTable,
		     (UINT)fat_vbr.BPB.SectorsPerTrack,
		     (UINT)fat_vbr.BPB.NumberOfHeads,
		     (UINT)fat_vbr.ExtendedBPB.PhysicalDriveNumber,
		     (UINT)fat_vbr.ExtendedBPB.Signature,
		     (UINT)fat_vbr.ExtendedBPB.Id,
		     sizeof(fat_vbr.ExtendedBPB.VolumeLabel),
		     CleanNonPrintables
		     (fat_vbr.ExtendedBPB.VolumeLabel,
		      sizeof(fat_vbr.ExtendedBPB.VolumeLabel)),
		     sizeof(fat_vbr.ExtendedBPB.SystemId),
		     CleanNonPrintables(fat_vbr.ExtendedBPB.SystemId,
					sizeof(fat_vbr.ExtendedBPB.SystemId)));
	    else
	      win_perror("Error reading first sector");

	    SetLastError(NO_ERROR);
	    break;
	  }

	case IOCTL_SCSI_GET_ADDRESS:
	  {
	    SCSI_ADDRESS scsi_address = { 0 };
	    scsi_address.Length = sizeof(scsi_address);

	    hDev = CreateFile(cBuf, GENERIC_READ,
			      FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			      OPEN_EXISTING, 0, NULL);
	    if (hDev == INVALID_HANDLE_VALUE)
	      break;

	    DeviceIoControl(hDev,
			    IOCTL_DISK_UPDATE_PROPERTIES,
			    NULL,
			    0,
			    NULL,
			    0,
			    &dw,
			    NULL);

	    if (!DeviceIoControl(hDev,
				 IOCTL_SCSI_GET_ADDRESS,
				 NULL,
				 0,
				 &scsi_address,
				 sizeof scsi_address,
				 &dw,
				 NULL))
	      break;

	    printf("Disk SCSI address:\n"
		   "Port number   : %i\n"
		   "Path id       : %i\n"
		   "Target id     : %i\n"
		   "Logical unit n: %i\n",
		   (int)scsi_address.PortNumber,
		   (int)scsi_address.PathId,
		   (int)scsi_address.TargetId,
		   (int)scsi_address.Lun);


	    SetLastError(NO_ERROR);
	    break;
	  }

	case IOCTL_VOLUME_OFFLINE:
	case IOCTL_VOLUME_ONLINE:
	case FSCTL_LOCK_VOLUME:
	case IOCTL_STORAGE_EJECT_MEDIA:
	    hDev = CreateFile(cBuf, GENERIC_READ | GENERIC_WRITE,
			      FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			      OPEN_EXISTING, 0, NULL);

	    if (hDev == INVALID_HANDLE_VALUE)
	      hDev = CreateFile(cBuf, FILE_READ_ATTRIBUTES | SYNCHRONIZE,
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
				OPEN_EXISTING, 0, NULL);

	    if (hDev == INVALID_HANDLE_VALUE)
	      break;

	  FlushFileBuffers(hDev);

	  if (!DeviceIoControl(hDev, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &dw,
			       NULL))
	    break;

      	case FSCTL_DISMOUNT_VOLUME:
	  if (hDev == INVALID_HANDLE_VALUE)
	    hDev = CreateFile(cBuf, GENERIC_READ | GENERIC_WRITE,
			      FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			      OPEN_EXISTING, 0, NULL);
	  if (hDev == INVALID_HANDLE_VALUE)
	    hDev = CreateFile(cBuf, FILE_READ_ATTRIBUTES | SYNCHRONIZE,
			      FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			      OPEN_EXISTING, 0, NULL);
	  if (hDev == INVALID_HANDLE_VALUE)
	    break;
	  
	  if (!DeviceIoControl(hDev, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0,
			       &dw, NULL))
	    break;

	  switch (dwIoControlCode)
	    {
	    case IOCTL_STORAGE_EJECT_MEDIA:
	    case IOCTL_VOLUME_OFFLINE:
	    case IOCTL_VOLUME_ONLINE:
	      {
	      if (DeviceIoControl(hDev, dwIoControlCode, NULL, 0,
				  NULL, 0, &dw, NULL))
		SetLastError(NO_ERROR);

	      break;
	      }

	    default:
	      SetLastError(NO_ERROR);
	    }

	  break;

	case IOCTL_DISK_UPDATE_PROPERTIES:
	case IOCTL_STORAGE_LOAD_MEDIA:
	  hDev = CreateFile(cBuf, FILE_READ_ATTRIBUTES | SYNCHRONIZE,
			    FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			    OPEN_EXISTING, 0, NULL);
	  if (hDev == INVALID_HANDLE_VALUE)
	    break;

	  if (DeviceIoControl(hDev, dwIoControlCode, NULL, 0, NULL, 0,
			      &dw, NULL))
	    SetLastError(NO_ERROR);

	  break;

	case FSCTL_EXTEND_VOLUME:
	  {
	    LONGLONG newsize;
	    CHAR suffix;

	    if (argv[2] == NULL)
	      {
		fputs("Function requires an argument.\n", stderr);
		return 1;
	      }

	    switch (sscanf(argv[2], "%I64i%c",
			   &newsize, &suffix))
	      {
	      case 2:
		switch (suffix)
		  {
		  case 'G':
		    newsize <<= 10;
		  case 'M':
		    newsize <<= 10;
		  case 'K':
		    newsize <<= 10;
		    break;

		  default:
		    fprintf(stderr, "Invalid size suffix: %c\n", suffix);
		    return 1;
		  }

	      case 1:
		break;

	      default:
		fprintf(stderr, "Invalid grow size: %s\n", argv[3]);
		return 1;
	      }

	    hDev = CreateFile(cBuf, GENERIC_READ | GENERIC_WRITE,
			      FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			      OPEN_EXISTING, 0, NULL);
	    if (hDev == INVALID_HANDLE_VALUE)
	      break;

	    if (DeviceIoControl(hDev, FSCTL_EXTEND_VOLUME, &newsize,
				sizeof newsize, NULL, 0, &dw, NULL))
	      printf("%s - OK.\n", argv[1]);
	    else
	      win_perror(argv[1]);
	    
	    argv++;
	    continue;
	  }

	case IOCTL_DISK_GROW_PARTITION:
	  {
	    DISK_GROW_PARTITION disk_grow_partition = { 0 };
	    CHAR suffix;

	    if (argv[2] == NULL ? TRUE : argv[3] == NULL)
	      {
		fputs("Function requires two arguments.\n", stderr);
		return 1;
	      }

	    if (sscanf(argv[2], "%i", &disk_grow_partition.PartitionNumber) <
		1)
	      {
		fprintf(stderr, "Invalid partition number: %s\n", argv[2]);
		return 1;
	      }

	    switch (sscanf(argv[3], "%I64i%c",
			   &disk_grow_partition.BytesToGrow, &suffix))
	      {
	      case 2:
		switch (suffix)
		  {
		  case 'G':
		    disk_grow_partition.BytesToGrow.QuadPart <<= 10;
		  case 'M':
		    disk_grow_partition.BytesToGrow.QuadPart <<= 10;
		  case 'K':
		    disk_grow_partition.BytesToGrow.QuadPart <<= 10;
		    break;

		  default:
		    fprintf(stderr, "Invalid size suffix: %c\n", suffix);
		    return 1;
		  }

	      case 1:
		break;

	      default:
		fprintf(stderr, "Invalid grow size: %s\n", argv[3]);
		return 1;
	      }

	    hDev = CreateFile(cBuf, GENERIC_READ | GENERIC_WRITE,
			      FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			      OPEN_EXISTING, 0, NULL);
	    if (hDev == INVALID_HANDLE_VALUE)
	      break;

	    if (DeviceIoControl(hDev, IOCTL_DISK_GROW_PARTITION,
				&disk_grow_partition,
				sizeof disk_grow_partition, NULL, 0, &dw,
				NULL))
	      printf("%s - OK.\n", argv[1]);
	    else
	      win_perror(argv[1]);

	    argv += 2;
	    continue;
	  }

	default:
	  hDev = CreateFile(cBuf, GENERIC_READ | GENERIC_WRITE,
			    FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			    OPEN_EXISTING, 0, NULL);
	  if (hDev == INVALID_HANDLE_VALUE)
	    break;

	  FlushFileBuffers(hDev);
	}

      fputs("\r\n", stderr);
      win_perror(argv[1]);
    }

  return 0;
}
