#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

int
main(int argc, char **argv)
{
  STARTUPINFO sif = { sizeof(sif) };
  PROCESS_INFORMATION pif;
  BOOL uniproc = FALSE;
  BOOL noexcept = FALSE;

  if (argv[1] == NULL)
    {
      puts("Creates a process and returns process id.\r\n"
	   "Syntax:\r\n"
	   "\n"
	   "cpretpid [/u] [/i] commandline\r\n"
	   "\n"
	   "/u    Run on first processor only.\r\n"
	   "/i    Auto-ignore exceptions in process (not implemented).");
      return 0;
    }

  for (;;)
    if (strcmpi(argv[1], "/U") == 0)
      {
	argv++;
	uniproc = TRUE;
      }
    else if (strcmpi(argv[1], "/I") == 0)
      {
	argv++;
	noexcept = TRUE;
      }
    else
      break;

  if (!CreateProcess(NULL, strstr(GetCommandLine(), argv[1]), NULL, NULL,
		     FALSE, CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP |
		     CREATE_SUSPENDED | GetPriorityClass(GetCurrentProcess()),
		     NULL, NULL, &sif, &pif))
    {
      win_perror("cpretpid");
      return 0;
    }

  if (uniproc)
    if (!SetProcessAffinityMask(pif.hProcess, 1))
      win_perror("cpretpid /U");

  if (noexcept)
    ;

  ResumeThread(pif.hThread);
  return pif.dwProcessId;
}
