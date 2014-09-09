#define UNICODE
#define _UNICODE
#ifndef _DLL
#define _DLL
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <ntdll.h>
#include <winstrct.h>
#include <stdlib.h>

#pragma comment(lib, "msvcrt.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "ntdll.lib")

int
wmain(int argc, LPWSTR *argv)
{
  HANDLE hToken;
  HANDLE hFile;
  IO_STATUS_BLOCK IoStatusBlock;
  NTSTATUS NtStatus;
  OBJECT_ATTRIBUTES ObjectAttributes;
  UNICODE_STRING NtPath;
  UNICODE_STRING NewName;
  PFILE_NAME_INFORMATION pFileNameInformation;
  ULONG BufferSize;

  SetOemPrintFLineLength(GetStdHandle(STD_ERROR_HANDLE));

  if (argc < 3)
    {
      fprintf(stderr,
	      "Usage:\r\n"
	      "%ws filename newshortfilename\r\n", argv[0]);

      return 2;
    }

  if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
      TOKEN_PRIVILEGES TP;
      TP.PrivilegeCount = 1;
      LookupPrivilegeValue(L"", SE_RESTORE_NAME, &(TP.Privileges[0].Luid));
      TP.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

      AdjustTokenPrivileges(hToken, FALSE, &TP, 0, NULL, NULL);
    }

  if (!RtlDosPathNameToNtPathName_U(argv[1], &NtPath, NULL, NULL))
    {
      fputs("Invalid path.\r\n", stderr);
      return 1;
    }

  InitializeObjectAttributes(&ObjectAttributes, &NtPath, OBJ_CASE_INSENSITIVE,
			     NULL, NULL);

  NtStatus = NtOpenFile(&hFile,
			SYNCHRONIZE | DELETE | FILE_WRITE_DATA,
			&ObjectAttributes,
			&IoStatusBlock,
			FILE_SHARE_WRITE |
			FILE_SHARE_READ |
			FILE_SHARE_DELETE,
			FILE_OPEN_FOR_BACKUP_INTENT |
			FILE_SYNCHRONOUS_IO_NONALERT);

  if (NT_ERROR(NtStatus))
    {
      char *errmsg = win_errmsgA(RtlNtStatusToDosError(NtStatus));
      oem_printf(stderr,
		 "Cannot open file '%1': %2.%%n"
		 "The file must exist and you must have delete permissions to "
		 "change the short name.%%n", argv[1], errmsg);
      return NtStatus;
    }

  RtlInitUnicodeString(&NewName, argv[2]);
  BufferSize = sizeof(FILE_NAME_INFORMATION) + NewName.Length;
  pFileNameInformation = (PFILE_NAME_INFORMATION) halloc(BufferSize);

  if (pFileNameInformation == NULL)
    {
      win_perror(NULL);
      return -1;
    }

  pFileNameInformation->FileNameLength = NewName.Length;
  memcpy(pFileNameInformation->FileName, NewName.Buffer,
	 pFileNameInformation->FileNameLength);

  NtStatus = NtSetInformationFile(hFile, &IoStatusBlock,
				  pFileNameInformation, BufferSize,
				  FileShortNameInformation);
  if (NT_SUCCESS(NtStatus))
    {
      puts("File short name successfully set.");
      return 0;
    }
  else
    {
      char *errmsg = nt_errmsgA(NtStatus);
      oem_printf(stderr,
		 "Cannot set new short filename: %1%%n"
		 "Short filenames can only be changed on NTFS volumes on "
		 "Windows XP or Server 2003 or later.%%n",
		 errmsg);
      return NtStatus;
    }
}

int
wmainCRTStartup()
{
  int argc = 0;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
  if (argv == NULL)
    {
      MessageBoxA(NULL, "This program requires Windows XP or Server 2003.",
		  "Incompatible Windows version", MB_ICONEXCLAMATION);
      ExitProcess((UINT) -1);
    }
  else
    exit(wmain(argc, argv));
}
