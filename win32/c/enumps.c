#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <tlhelp32.h>

#include <stdlib.h>

#ifndef _WIN64
#ifdef _DLL
#pragma comment(lib, "minwcrt.lib")
#endif
#endif

void
__declspec(noreturn) usage()
{
  fputs("Processes, threads and modules enumeration tool.\r\n"
	"For Windows 95, 98, ME, 2000, XP and Server 2003.\r\n"
	"Copyright (C) Olof Lagerkvist, 2005.\r\n"
	"\n"
	"Syntax:\r\n"
	"enumps [-p] [-t] [-a|process ...]\r\n"
	"\n"
	"-p         Enumerate all running processes.\r\n"
	"-t         Enumerate all running threads.\r\n"
	"-a         Used instead of a process list to enumerate all referenced modules\r\n"
	"           in all running processes.\r\n"
	"process    List of ids or image filenames with or without path, with or without\r\n"
	"           extensions. Enumerates all modules referenced in the processes.\r\n"
	"\n"
	"Without parameters enumps enumerates all running processes.\r\n",
	stderr);

  exit(1);
}

void PrintProcesses()
{
  BOOL b;
  PROCESSENTRY32 process;

  HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapShot == INVALID_HANDLE_VALUE)
    {
      win_perror("Failed to create process snapshot");
      return;
    }

  printf("%10s %10s %3s %3s %4s %s\n",
	 "PID", "ParentPID", "Thr", "Pri", "Refs", "Image");

  process.dwSize = sizeof process;
  for (b = Process32First(hSnapShot, &process);
       b != 0;
       b = Process32Next(hSnapShot, &process))
    {
      CharToOem(process.szExeFile, process.szExeFile);
      printf("%10i %10i %3i %3i %4i %s\n",
	     process.th32ProcessID,
	     process.th32ParentProcessID,
	     process.cntThreads,
	     process.pcPriClassBase,
	     process.cntUsage,
	     process.szExeFile);
    }

  if (GetLastError() != ERROR_NO_MORE_FILES)
    win_perror("Failed to enumerate processes");

  CloseHandle(hSnapShot);

  fflush(stdout);
  fflush(stderr);
}

void PrintThreads()
{
  BOOL b;
  THREADENTRY32 thread;

  HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if (hSnapShot == INVALID_HANDLE_VALUE)
    {
      win_perror("Failed to create thread snapshot");
      return;
    }

  printf("%10s %10s %3s %4s %4s\n",
	 "TID", "OwnerPID", "Pri", "DPri", "Refs");

  thread.dwSize = sizeof thread;
  for (b = Thread32First(hSnapShot, &thread);
       b != 0;
       b = Thread32Next(hSnapShot, &thread))
    printf("%10i %10i %3i %4i %4i\n",
	   thread.th32ThreadID,
	   thread.th32OwnerProcessID,
	   thread.tpBasePri,
	   thread.tpDeltaPri,
	   thread.cntUsage);

  if (GetLastError() != ERROR_NO_MORE_FILES)
    win_perror("Failed to enumerate threads");

  CloseHandle(hSnapShot);

  fflush(stdout);
  fflush(stderr);
}

void PrintProcessModules(DWORD dwPid)
{
  BOOL b;
  MODULEENTRY32 module;

  HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
  if (hSnapShot == INVALID_HANDLE_VALUE)
    {
      LPSTR szErrMsg = win_error;
      CharToOem(szErrMsg, szErrMsg);
      fprintf(stderr, "Failed to create module snapshot for process %i: %s\n",
	      dwPid, szErrMsg);
      return;
    }

  module.dwSize = sizeof module;
  for (b = Module32First(hSnapShot, &module);
       b != 0;
       b = Module32Next(hSnapShot, &module))
    {
      CharToOem(module.szExePath, module.szExePath);
      printf("%10i %4i %4i %08X %5.4g %2s %s\n",
	     module.th32ProcessID,
	     (LONG)(SHORT)module.GlblcntUsage,
	     (LONG)(SHORT)module.ProccntUsage,
	     module.modBaseAddr,
	     TO_h(module.modBaseSize), TO_p(module.modBaseSize),
	     module.szExePath);
    }

  if (GetLastError() != ERROR_NO_MORE_FILES)
    win_perror("Failed to enumerate modules");

  CloseHandle(hSnapShot);

  fflush(stdout);
  fflush(stderr);
}

void PrintModules(LPCSTR *szFilterArray)
{
  BOOL b;
  PROCESSENTRY32 process;

  HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapShot == INVALID_HANDLE_VALUE)
    {
      win_perror("Failed to create process snapshot");
      return;
    }

  process.dwSize = sizeof process;
  for (b = Process32First(hSnapShot, &process);
       b != 0;
       b = Process32Next(hSnapShot, &process))
    if (process.th32ProcessID == 0)
      continue;
    else if (szFilterArray == NULL)
      PrintProcessModules(process.th32ProcessID);
    else
      {
	LPCSTR *szFilter = szFilterArray;
	LPCSTR szModFile = strrchr(process.szExeFile, '\\');
	if (szModFile == NULL)
	  szModFile = process.szExeFile;
	else
	  szModFile++;

	strlwr(process.szExeFile);

	do
	  {
	    LPSTR szModFileCopy;
	    LPSTR szModExt;

	    if (strtoul(*szFilter, NULL, 0) == process.th32ProcessID)
	      break;

	    if (strcmp(szModFile, *szFilter) == 0)
	      break;

	    if (szModFile != process.szExeFile)
	      if (strcmp(process.szExeFile, *szFilter) == 0)
		break;

	    szModFileCopy = strdup(szModFile);
	    szModExt = strrchr(szModFileCopy, '.');
	    if (szModExt != NULL)
	      {
		*szModExt = 0;
	      
		if (strcmp(szModFileCopy, *szFilter) == 0)
		  {
		    free(szModFileCopy);
		    break;
		  }
	      }
	    free(szModFileCopy);
	  }
	while (*(++szFilter) != NULL);

	if (*szFilter != NULL)
	  PrintProcessModules(process.th32ProcessID);
      }

  if (GetLastError() != ERROR_NO_MORE_FILES)
    win_perror("Failed to enumerate processes");

  CloseHandle(hSnapShot);

  fflush(stdout);
  fflush(stderr);
}

int main(int argc, char **argv)
{
  BOOL bPrintModulesOfAllProcesses = FALSE;
  BOOL bPrintProcesses = FALSE;
  BOOL bPrintThreads = FALSE;
  LPCSTR *szFilter = NULL;

  if (argc == 1)
    {
      PrintProcesses();
      return 0;
    }

  // Nice argument parse loop :)
  while (argv[1] ? argv[1][0] ? (argv[1][0] | 0x02) == '/' : FALSE : FALSE)
    {
      // Break if this is a negative PID
      if (isdigit(argv[1][1]))
	break;

      while ((++argv[1])[0])
	switch (argv[1][0] | 0x20)
	  {
	  case 'a':
	    bPrintModulesOfAllProcesses = TRUE;
	    break;
	  case 'p':
	    bPrintProcesses = TRUE;
	    break;
	  case 't':
	    bPrintThreads = TRUE;
	    break;
	  default:
	    usage();
	  }

      --argc;
      ++argv;
    }

  if (bPrintProcesses)
    PrintProcesses();

  if (bPrintThreads)
    PrintThreads();

  if ((argc == 1) & !bPrintModulesOfAllProcesses)
    return 0;

  printf("%10s %4s %4s %8s %8s %s\n",
	 "PID", "RefG", "RefP", "BaseAddr", "Size", "Image");

  while (argc-- > 1)
    {
      LPSTR *largv = argv;

      while (*(++largv) != NULL)
	strlwr(*largv);

      szFilter = argv+1;
    }

  PrintModules(szFilter);
  return 0;
}
