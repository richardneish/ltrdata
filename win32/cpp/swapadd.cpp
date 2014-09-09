#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <ntdll.h>
#include <stdio.h>
#include <stdlib.h>
#include <winstrct.h>

#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "shell32.lib")

__declspec(noreturn)
void
syntax_help()
{
  fputs("Swapfile utility.\r\n"
	"\n"
	"Type swapadd without parameters to view information about current swapfiles.\r\n"
	"\n"
	"Syntax to create a new swapfile:\r\n"
	"swapadd filename minsize[K|M|G] maxsize[K|M|G]\r\n",
	stderr);

  exit(-1);
}

BOOL
ReadLargeInteger(PLARGE_INTEGER large_integer,
		 LPWSTR source_string)
{
  WCHAR suffix;

  switch (swscanf(source_string, L"%I64i%wc", large_integer, &suffix))
    {
    case 2:
      switch (suffix)
	{
	case 'G':
	  large_integer->QuadPart <<= 10;
	case 'M':
	  large_integer->QuadPart <<= 10;
	case 'K':
	  large_integer->QuadPart <<= 10;
	  return TRUE;

	default:
	  return FALSE;
	}

    case 1:
      return TRUE;

    default:
      return FALSE;
    }
}

int
wmain(int argc, LPWSTR *argv)
{
  NTSTATUS status;

  if (argc == 1)
    {
      static BYTE buffer[512];
      ULONG bytes;

      status = NtQuerySystemInformation(SystemPageFileInformation,
					buffer,
					sizeof buffer,
					&bytes);

      if (!NT_SUCCESS(status))
	{
	  nt_perror(status, L"Cannot get information about current swapfiles");
	  return RtlNtStatusToDosError(status);
	}

      if (bytes == 0)
	puts("No swapfiles.");
      else
	for (PSYSTEM_PAGE_FILE_INFORMATION pagefile_info =
	       (PSYSTEM_PAGE_FILE_INFORMATION) buffer;
	     ;
	     pagefile_info = (PSYSTEM_PAGE_FILE_INFORMATION)
	       (buffer +
		pagefile_info->NextEntryOffset))
	  {
	    oem_printf(stdout,
		       "Swapfile '%1!.*ws!': "
		       "size: %2!u! MB usage: %3!u! MB (%4!u!%%)%%n",
		       pagefile_info->FileName.Length / sizeof(WCHAR),
		       pagefile_info->FileName.Buffer,
		       (pagefile_info->CurrentSize + 0xFF) >> 8,
		       (pagefile_info->TotalUsed + 0xFF) >> 8,
		       (pagefile_info->TotalUsed / (pagefile_info->CurrentSize
						      / 100)));
	    
	    if (pagefile_info->NextEntryOffset == 0)
	      break;
	  }

      status = NtQuerySystemInformation(SystemRegistryQuotaInformation,
					buffer,
					sizeof buffer,
					&bytes);

      if (!NT_SUCCESS(status))
	{
	  nt_perror(status, L"Cannot get registry quota information");
	  return RtlNtStatusToDosError(status);
	}

      PSYSTEM_REGISTRY_QUOTA_INFORMATION reg_quota_info =
	(PSYSTEM_REGISTRY_QUOTA_INFORMATION) buffer;

      oem_printf(stdout,
		 "%%n"
		 "Registry quota allowed: %1!i! MB%%n"
		 "Registry quota used: %2!i! MB%%n"
		 "Paged pool size: %3!i! MB%%n",
		 (reg_quota_info->RegistryQuotaAllowed + 0xFFFFF) >> 20,
		 (reg_quota_info->RegistryQuotaUsed + 0xFFFFF) >> 20,
		 (reg_quota_info->PagedPoolSize + 0xFFFFF) >> 20);

      return NO_ERROR;
    }

  if (argc != 4)
    syntax_help();

  UNICODE_STRING file_nt_name = { 0 };

  if (!RtlDosPathNameToNtPathName_U(argv[1], &file_nt_name, NULL, NULL))
    {
      oem_printf(stderr, "Invalid path or file name: '%1!ws!'%%n", argv[1]);
      return ERROR_PATH_NOT_FOUND;
    }

  LARGE_INTEGER min_size = { 0 };
  if (!ReadLargeInteger(&min_size, argv[2]))
    syntax_help();

  LARGE_INTEGER max_size = { 0 };
  if (!ReadLargeInteger(&max_size, argv[3]))
    syntax_help();

  BOOLEAN old_setting;
  RtlAdjustPrivilege(SE_CREATE_PAGEFILE_PRIVILEGE, TRUE, FALSE, &old_setting);

  status = NtCreatePagingFile(&file_nt_name,
			      &min_size,
			      &max_size,
			      NULL);

  if (!NT_SUCCESS(status))
    {
      nt_perror(status, L"Page file creation failed");
      return RtlNtStatusToDosError(status);
    }

    puts("OK.");

  return NO_ERROR;
}

#ifdef _DLL
// This is enough startup code for this program if compiled to use the DLL CRT.
extern "C"
int
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
#endif
