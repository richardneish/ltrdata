#define WIN32_LEAN_AND_MEAN
#ifndef _DLL
#define _DLL
#endif
#include <winstrct.h>
#include <minwcrtlib.h>

#ifndef _WIN64
#pragma comment(lib, "minwcrt.lib")
#endif

int
main(int argc, char **argv)
{
  HANDLE *hProc = NULL;
  BOOL bWaitAll = FALSE;
  DWORD dwTimeout = INFINITE;
  DWORD dwSignalledObjectId = 0;
  HANDLE *hThisProc = NULL;

  if (argc > 1 ? strcmp(argv[1], "-a") == 0 : FALSE)
    {
      bWaitAll = TRUE;
      argc--;
      argv++;
    }

  if (argc < 2)
    {
      fputs("Usage:\r\n"
	    "waitpid [-a] pid1 [pid2 ...]\r\n", stderr);

      return STILL_ACTIVE;
    }

  if (argc >= MAXIMUM_WAIT_OBJECTS)
    {
      fputs("Too many process identifiers.\r\n", stderr);
      return STILL_ACTIVE;
    }

  hProc = halloc_seh(sizeof(HANDLE) * (argc - 1));

  for (hThisProc = hProc; (hThisProc - hProc) < argc - 1; hThisProc++)
    {
      *hThisProc = OpenProcess(PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE,
			       strtoul((++argv)[0], NULL, 0));

      if (*hThisProc == NULL)
	{
	  win_perror(argv[0]);
	  return STILL_ACTIVE;
	}
    }

  dwSignalledObjectId =
    WaitForMultipleObjects(argc - 1, hProc, bWaitAll, dwTimeout);

  if (dwSignalledObjectId == WAIT_TIMEOUT)
    {
      fputs("Timeout.\r\n", stderr);
      return STILL_ACTIVE;
    }

  if ((dwSignalledObjectId >= WAIT_OBJECT_0) &
      (dwSignalledObjectId <= (WAIT_OBJECT_0 + argc - 2)))
    dwSignalledObjectId = dwSignalledObjectId - WAIT_OBJECT_0;
  else if ((dwSignalledObjectId >= WAIT_ABANDONED_0) &
	   (dwSignalledObjectId <= (WAIT_ABANDONED_0 + argc - 2)))
    dwSignalledObjectId = dwSignalledObjectId - WAIT_ABANDONED_0;
  else
    {
      win_perror("Wait failed");
      return STILL_ACTIVE;
    }

  if (!GetExitCodeProcess(hProc[dwSignalledObjectId], &dwSignalledObjectId))
    {
      win_perror("Cannot get exit code");
      return 0;
    }

  return dwSignalledObjectId;
}
