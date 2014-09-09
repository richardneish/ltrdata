#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <shellapi.h>
#include <psapi.h>

#include <stdlib.h>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "psapi.lib")

int iFilterArgc = 0;
LPCWSTR *wczFilterArgv = NULL;

void
PrintModules(DWORD processID)
{
  static DWORD dwModsSize = 2048;
  static HMODULE *hMods = NULL;
  static WCHAR wczModName[MAX_PATH];
  static MODULEINFO moduleinfo;
  static char szPrintBuffer[MAX_PATH + 80];
  HANDLE hProcess = NULL;
  DWORD cbNeeded = 0;
  DWORD dwModules;
  UINT i;

  if (hMods == NULL)
    hMods = halloc_seh(dwModsSize);

  // Get a list of all the modules in this process.
  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
			 PROCESS_VM_READ,
			 FALSE,
			 processID);
  if (NULL == hProcess)
    return;

  if (iFilterArgc > 0)
    {
      int argc;
      LPCWSTR wczModFile;

      if (!GetModuleFileNameEx(hProcess,
			       NULL,
			       wczModName,
			       sizeof(wczModName) / sizeof(*wczModName)))
	{
	  CloseHandle(hProcess);
	  return;
	}

      wcslwr(wczModName);

      wczModFile = wcsrchr(wczModName, L'\\');
      if (wczModFile == NULL)
	wczModFile = wczModName;
      else
	wczModFile++;

      for (argc = 0; argc < iFilterArgc; argc++)
	{
	  LPWSTR wczModFileCopy;
	  LPWSTR wczModExt;

	  if (wcstoul(wczFilterArgv[argc], NULL, 0) == processID)
	    break;

	  if (wcscmp(wczModFile, wczFilterArgv[argc]) == 0)
	    break;

	  if (wczModFile != wczModName)
	    if (wcscmp(wczModName, wczFilterArgv[argc]) == 0)
	      break;

	  wczModFileCopy = wcsdup(wczModFile);
	  wczModExt = wcsrchr(wczModFileCopy, L'.');
	  if (wczModExt != NULL)
	    {
	      *wczModExt = 0;
	      
	      if (wcscmp(wczModFileCopy, wczFilterArgv[argc]) == 0)
		{
		  free(wczModFileCopy);
		  break;
		}
	    }
	  free(wczModFileCopy);
	}

      if (argc == iFilterArgc)
	{
	  CloseHandle(hProcess);
	  return;
	}
    }

  for (;;)
    {
      if (!EnumProcessModules(hProcess, hMods, dwModsSize, &cbNeeded))
	{
	  CloseHandle(hProcess);
	  return;
	}

      if (cbNeeded < dwModsSize)
	break;

      dwModsSize <<= 1;
      hMods = hrealloc_seh(hMods, dwModsSize);
    }

  dwModules = cbNeeded / sizeof(HMODULE);
  for (i = 0; i < dwModules; i++)
    {
      LPCWSTR wczModPtr = wczModName;
      // Get the full path to the module's file.
      if (GetModuleFileNameEx(hProcess,
			      hMods[i],
			      wczModName,
			      sizeof(wczModName) / sizeof(*wczModName)))
	{
	  if (wczModName[0] == L'\\')
	    if ((wcsncmp(wczModName, L"\\??\\", 4) == 0) |
		(wcsncmp(wczModName, L"\\\\?\\", 4) == 0) |
		(wcsncmp(wczModName, L"\\\\.\\", 4) == 0))
	      if (wcsnicmp(wczModName + 4, L"UNC\\", 4) == 0)
		{
		  wczModName[6] = L'\\';
		  wczModPtr = wczModName + 6;
		}
	      else if (wcsncmp(wczModName + 5, L":\\", 2) == 0)
		wczModPtr = wczModName + 4;
	}
      else
	wczModPtr = L"{unknown}";

      if (!GetModuleInformation(hProcess,
				hMods[i],
				&moduleinfo,
				sizeof moduleinfo))
	ZeroMemory(&moduleinfo, sizeof moduleinfo);

      _snprintf(szPrintBuffer, sizeof szPrintBuffer,
		"%10i %08X %08X %5.4g %2s %ws",
		processID,
		moduleinfo.lpBaseOfDll,
		moduleinfo.EntryPoint,
		TO_h(moduleinfo.SizeOfImage), TO_p(moduleinfo.SizeOfImage),
		wczModPtr);

      szPrintBuffer[sizeof(szPrintBuffer) - 1] = 0;

      CharToOemA(szPrintBuffer, szPrintBuffer);
      puts(szPrintBuffer);
    }

  CloseHandle(hProcess);
}

int
wmain(int argc, LPWSTR *argv)
{
  // Get the list of process identifiers.
  DWORD dwProcessesSize = 2048;
  DWORD *aProcesses = halloc_seh(dwProcessesSize);
  DWORD cbNeeded;
  DWORD cProcesses;
  UINT i;

  if (argc > 1)
    {
      if ((argv[1][0] | 0x02) == L'/')
      // Break if this is a negative PID
	if (!iswdigit(argv[1][1]))
	  {
	    fputs("Processes and modules enumeration tool.\r\n"
		  "For Windows NT 4.0, 2000, XP and Server 2003.\r\n"
		  "Copyright (C) Olof Lagerkvist, 2005.\r\n"
		  "\n"
		  "Syntax:\r\n"
		  "psmod [process ...]\r\n"
		  "\n"
		  "process    List of ids or image filenames with or without path, with or without\r\n"
		  "           extensions. Enumerates all modules referenced in the processes.\r\n"
		  "\n"
		  "Without parameters psmod enumerates all referenced modules in all running\r\n"
		  "processes.\r\n",
		  stderr);
	    return 1;
	  }

      iFilterArgc = argc - 1;

      while (--argc)
	wcslwr(argv[argc]);

      wczFilterArgv = argv + 1;
    }

  for (;;)
    {
      if (!EnumProcesses(aProcesses, dwProcessesSize, &cbNeeded))
	{
	  win_perrorA(NULL);
	  return 1;
	}

      if (cbNeeded < dwProcessesSize)
	break;

      dwProcessesSize <<= 1;
      aProcesses = hrealloc_seh(aProcesses, dwProcessesSize);
    }

  // Calculate how many process identifiers were returned.
  cProcesses = cbNeeded / sizeof(DWORD);

  // Print the name of the modules for each process.
  printf("%10s %8s %8s %8s %s\n",
	 "PID", "BaseAddr", "Entry", "Size", "Image");

  for (i = 0; i < cProcesses; i++)
    PrintModules(aProcesses[i]);

  return 0;
}

__declspec(noreturn)
void
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
