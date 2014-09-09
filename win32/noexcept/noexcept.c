#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <ntdll.h>
#include <winstrct.h>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ntdllp.lib")
#pragma comment(lib, "noexcept.lib")

#pragma comment(linker, "/subsystem:windows")

DWORD __declspec(dllimport) WINAPI InstallHandler(LPVOID);
DWORD __declspec(dllimport) WINAPI UninstallHandler(LPVOID);

int
Uninstall(int argc, LPWSTR *argv)
{
  DWORD dwPid = wcstoul(argv[1], NULL, 0);
  DWORD dwTid;
  HANDLE hProcess;
  HANDLE hThread;
  LPVOID lpMemBlock = NULL;
  static WCHAR wszModFile[32768] = L"";
  DWORD dwBaseAddress = 0;
  DWORD dwFunctionOffset =
    (DWORD) UninstallHandler - (DWORD) GetModuleHandle(L"noexcept.dll");
  NTSTATUS status;
  const WCHAR wczString[] = L"noexcept.dll";
  SIZE_T dwStringLength = sizeof wczString;
  SIZE_T dwRegionSize = dwStringLength;

  if (argc < 2)
    {
      MessageBoxA(NULL, "/u requires a process identifier as parameter.",
		  "Syntax error", MB_ICONEXCLAMATION);
      return 2;
    }

  hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |
			 PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION |
			 SYNCHRONIZE, FALSE, dwPid);
  if (hProcess == NULL)
    {
      MessageBoxA(NULL, win_errmsgA(win_errno), "Cannot open process",
		  MB_ICONEXCLAMATION);
      return 1;
    }

  status = NtAllocateVirtualMemory(hProcess, &lpMemBlock, 0, &dwRegionSize,
				   MEM_COMMIT, PAGE_READWRITE);
  if (!NT_SUCCESS(status))
    {
      MessageBoxA(NULL, win_errmsgA(RtlNtStatusToDosError(status)),
		  "Cannot allocate memory in the process",
		  MB_ICONEXCLAMATION);
      return 1;
    }

  if (!WriteProcessMemory(hProcess, lpMemBlock, wczString, dwStringLength,
			  NULL))
    {
      MessageBoxA(NULL, win_errmsgA(win_errno),
		  "Cannot write memory in the process", MB_ICONEXCLAMATION);
      NtFreeVirtualMemory(hProcess, &lpMemBlock, &dwRegionSize, MEM_RELEASE);
      return 1;
    }

  hThread =
    CreateRemoteThread(hProcess, NULL, 0,
		       (LPTHREAD_START_ROUTINE) GetModuleHandleW, lpMemBlock,
		       4096, &dwTid);
  if (hThread == NULL)
    {
      MessageBoxA(NULL, win_errmsgA(win_errno),
		  "Cannot create thread in the process", MB_ICONEXCLAMATION);
      dwRegionSize = 0;
      NtFreeVirtualMemory(hProcess, &lpMemBlock, &dwRegionSize, MEM_RELEASE);
      return 1;
    }

  WaitForSingleObject(hThread, INFINITE);

  dwRegionSize = 0;
  NtFreeVirtualMemory(hProcess, &lpMemBlock, &dwRegionSize, MEM_RELEASE);

  GetExitCodeThread(hThread, &dwBaseAddress);
  CloseHandle(hThread);
  
  if (dwBaseAddress == 0)
    {
      MessageBoxA(NULL,
		  "The noexcept.dll module is not loaded into the specified "
		  "process and can therefore not be uninstalled from it.",
		  "Not installed", MB_ICONEXCLAMATION);
      return 1;
    }

  hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)
			       (dwBaseAddress + dwFunctionOffset),
			       (LPVOID) ((argc > 2) ?
					 wcstoul(argv[2], NULL, 0) : 0), 4096,
			       &dwTid);
  if (hThread == NULL)
    {
      MessageBoxA(NULL, win_errmsgA(win_errno),
		  "Cannot create thread in the process", MB_ICONEXCLAMATION);
      return 1;
    }

  WaitForSingleObject(hThread, INFINITE);

  return 0;
}

int
wmain(int argc, LPWSTR *argv)
{
  PROCESS_INFORMATION process_information = { 0 };
  HANDLE hThread;
  DWORD dwTid;
  LPVOID lpMemBlock = NULL;
  SIZE_T dwStringLength;
  SIZE_T dwRegionSize;
  static WCHAR wszModFile[32768] = L"";
  DWORD dwBaseAddress = 0;
  DWORD dwFunctionOffset =
    (DWORD) InstallHandler - (DWORD) GetModuleHandle(L"noexcept.dll");
  NTSTATUS status;
  DWORD dwMemSize = GetModuleFileName(GetModuleHandle(L"noexcept.dll"),
				      wszModFile, sizeof(wszModFile) /
				      sizeof(*wszModFile));

  if (dwMemSize == 0)
    {
      MessageBoxA(NULL, win_errmsgA(win_errno), "Cannot attach noexcept.dll",
		  MB_ICONSTOP);
      return 1;
    }

  if (argc < 2)
    {
      MessageBoxA(NULL,
		  "Syntax:\r\n"
		  "noexcept <commandline>\r\n"
		  "Starts a new process and attaches noexcept.dll to it with "
		  "special exception handling.\r\n"
		  "\n"
		  "noexcept <pid>\r\n"
		  "Attaches noexcept.dll with special exception handling to a "
		  "running process.\r\n"
		  "\n"
		  "noexcept /u <pid>\r\n"
		  "Detaches noexcept.dll from a running process and restores "
		  "default exception handling in the process.",
		  "noexcept",
		  MB_ICONINFORMATION);
      return 2;
    }

  if (wcsicmp(argv[1], L"/u") == 0)
    return Uninstall(argc - 1, argv + 1);
  
  if (wcstoul(argv[1], NULL, 0) != 0)
    {
      process_information.hProcess =
	OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |
		    PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION |
		    SYNCHRONIZE, FALSE, wcstoul(argv[1], NULL, 0));
      if (process_information.hProcess == NULL)
	{
	  MessageBoxA(NULL, win_errmsgA(win_errno), "Cannot open process",
		      MB_ICONEXCLAMATION);
	  return 1;
	}
    }
  else
    {
      LPWSTR lpCmdLine = GetCommandLine();
      DWORD_PTR dwOffset = argv[1] - argv[0];
      STARTUPINFO startupinfo = { 0 };
      startupinfo.cb = sizeof startupinfo;

      if (dwOffset > 1)
	if (lpCmdLine[dwOffset] == L'"')
	  if (lpCmdLine[dwOffset+1] == L' ')
	    lpCmdLine += dwOffset + 2;
	  else
	    lpCmdLine += dwOffset;
	else
	  lpCmdLine += dwOffset;
      else
	lpCmdLine += dwOffset;

      while (lpCmdLine[0] == ' ')
	++lpCmdLine;

      if (!CreateProcess(NULL, lpCmdLine, NULL, NULL, FALSE, CREATE_SUSPENDED,
			 NULL, NULL, &startupinfo, &process_information))
	{
	  MessageBoxA(NULL,
		      mprintfA("Cannot execute command line:%n%1>%2!ws!<",
			       win_errmsgA(win_errno), lpCmdLine),
		      "Cannot create process", MB_ICONEXCLAMATION);
	  return 1;
	}
    }

  dwStringLength = (wcslen(wszModFile) + 1) * sizeof *wszModFile;
  dwRegionSize = dwStringLength;
  status = NtAllocateVirtualMemory(process_information.hProcess, &lpMemBlock,
				   0, &dwRegionSize, MEM_COMMIT,
				   PAGE_READWRITE);
  if (!NT_SUCCESS(status))
    {
      MessageBoxA(NULL, win_errmsgA(RtlNtStatusToDosError(status)),
		  "Cannot allocate memory in the process",
		  MB_ICONEXCLAMATION);
      if (process_information.hThread != NULL)
	TerminateProcess(process_information.hProcess, -1);
      return 1;
    }

  if (!WriteProcessMemory(process_information.hProcess, lpMemBlock, wszModFile,
			  dwStringLength, NULL))
    {
      MessageBoxA(NULL, win_errmsgA(win_errno),
		  "Cannot write memory in the process", MB_ICONEXCLAMATION);
      NtFreeVirtualMemory(process_information.hProcess, &lpMemBlock,
			  &dwRegionSize, MEM_RELEASE);
      if (process_information.hThread != NULL)
	TerminateProcess(process_information.hProcess, -1);
      return 1;
    }

  hThread =
    CreateRemoteThread(process_information.hProcess, NULL, 0,
		       (LPTHREAD_START_ROUTINE) LoadLibraryW, lpMemBlock, 4096,
		       &dwTid);
  if (hThread == NULL)
    {
      MessageBoxA(NULL, win_errmsgA(win_errno),
		  "Cannot create thread in the process", MB_ICONEXCLAMATION);
      dwRegionSize = 0;
      NtFreeVirtualMemory(process_information.hProcess, &lpMemBlock,
			  &dwRegionSize, MEM_RELEASE);
      if (process_information.hThread != NULL)
	TerminateProcess(process_information.hProcess, -1);
      return 1;
    }

  WaitForSingleObject(hThread, INFINITE);
  GetExitCodeThread(hThread, &dwBaseAddress);
  CloseHandle(hThread);

  dwRegionSize = 0;
  NtFreeVirtualMemory(process_information.hProcess, &lpMemBlock, &dwRegionSize,
		      MEM_RELEASE);

  if (dwBaseAddress == 0)
    {
      MessageBoxA(NULL, "Cannot load noexcept.dll into the process.",
		  "LoadLibrary() failed in the process",
		  MB_ICONEXCLAMATION);
      if (process_information.hThread != NULL)
	TerminateProcess(process_information.hProcess, -1);
      return 1;
    }

  hThread =
    CreateRemoteThread(process_information.hProcess, NULL, 0,
		       (LPTHREAD_START_ROUTINE)
		       (dwBaseAddress + dwFunctionOffset),
		       (LPVOID) (((process_information.hThread == NULL)
				  & (argc > 2)) ?
				 wcstoul(argv[2], NULL, 0) : 0), 4096, &dwTid);
  if (hThread == NULL)
    {
      MessageBoxA(NULL, win_errmsgA(win_errno),
		  "Cannot create thread in the process", MB_ICONEXCLAMATION);
      if (process_information.hThread != NULL)
	TerminateProcess(process_information.hProcess, -1);
      return 1;
    }

  WaitForSingleObject(hThread, INFINITE);

  if (process_information.hThread != NULL)
    ResumeThread(process_information.hThread);

  return 0;
}

int
WinMainCRTStartup()
{
  LPWSTR wczCmdLine = GetCommandLine();
  int argc = 0;
  LPWSTR *argv = CommandLineToArgvW(wczCmdLine, &argc);
  ExitProcess(wmain(argc, argv));
}
