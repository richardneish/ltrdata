#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <ntdll.h>

#include <winstrct.h>

#include <stdlib.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "crtdll.lib")
#endif
#pragma comment(lib, "ntdll.lib")

BOOL
CreateHardLinkToOpenFile(HANDLE hFile, LPCWSTR lpTarget, BOOL bReplaceOk)
{
  UNICODE_STRING Target;
  PFILE_LINK_INFORMATION FileLinkData;
  NTSTATUS Status;
  IO_STATUS_BLOCK IoStatusBlock;

  if (!RtlDosPathNameToNtPathName_U(lpTarget, &Target, NULL, NULL))
    {
      SetLastError(ERROR_PATH_NOT_FOUND);
      return FALSE;
    }

  FileLinkData = (PFILE_LINK_INFORMATION)
    halloc(Target.Length + sizeof(FILE_LINK_INFORMATION) -
	   sizeof(FileLinkData->FileName));
  if (FileLinkData == NULL)
    return FALSE;

  FileLinkData->ReplaceIfExists = (BOOLEAN) bReplaceOk;
  FileLinkData->RootDirectory = NULL;
  FileLinkData->FileNameLength = Target.Length;
  memcpy(FileLinkData->FileName, Target.Buffer, Target.Length);

  Status = NtSetInformationFile(hFile, &IoStatusBlock, FileLinkData,
				Target.Length +
				sizeof(FILE_LINK_INFORMATION) -
				sizeof(FileLinkData->FileName),
				FileLinkInformation);

  hfree(FileLinkData);

  if (NT_SUCCESS(Status))
    {
      OBJECT_ATTRIBUTES ObjectAttributes;
      HANDLE Handle;
      
      InitializeObjectAttributes(&ObjectAttributes,
				 &Target,
				 OBJ_CASE_INSENSITIVE,
				 NULL,
				 NULL);

      Status = NtOpenFile(&Handle,
			  FILE_READ_ATTRIBUTES,
			  &ObjectAttributes,
			  &IoStatusBlock,
			  FILE_SHARE_READ |
			  FILE_SHARE_WRITE |
			  FILE_SHARE_DELETE,
			  FILE_OPEN_FOR_BACKUP_INTENT);
      if (NT_SUCCESS(Status))
	NtClose(Handle);

      RtlFreeUnicodeString(&Target);
      return TRUE;
    }
  else
    {
      RtlFreeUnicodeString(&Target);
      SetLastError(RtlNtStatusToDosError(Status));
      return FALSE;
    }
}

int
wmain(int argc, LPWSTR *argv)
{
  BOOL bReplaceOk = FALSE;
  if (argc > 3)
    if (wcsicmp(argv[1], L"-f") == 0)
      {
	bReplaceOk = TRUE;
	argc--;
	argv++;
      }

  if (argc < 3)
    {
      printf("Usage: %ws [-f] existing new1 [new2 ...]\n", argv[0]);
      return 2;
    }

  EnableBackupPrivileges();

  HANDLE hFile =
    CreateFile(argv[1], 0,
	       FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
	       NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    {
      win_perrorW(argv[1]);
      return 1;
    }

  while (argc-- > 2)
    {
      Sleep(0);

      if (!CreateHardLinkToOpenFile(hFile, (++argv)[1], bReplaceOk))
	win_perrorW(argv[1]);
    }

  return 0;
}

// This is enough startup code for this program if compiled to use the DLL CRT.
extern "C" int
wmainCRTStartup()
{
  int argc = 0;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
  if (argv == NULL)
    {
      MessageBoxA(NULL, "This program requires Windows NT.",
		  "Incompatible Windows version", MB_ICONEXCLAMATION);
      ExitProcess((UINT) -1);
    }
  else
    exit(wmain(argc, argv));
}
