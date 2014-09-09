#define WINVER          0x0400
#define _WIN32_WINDOWS  0x0400
#define _WIN32_WINNT    0x0400
#define WIN32_LEAN_AND_MEAN

#include <winstrct.h>
#include <wtime.h>

#include <stdio.h>

#include "ntservice.h"

// Invoked with the /INSTALL switch. Installs LightRepeater Engine as a
// Windows NT Service using Service Control Manager.
EXTERN_C int WINAPI InstallService(int argc, char **argv)
{
   if (argc > 4)
      puts("InstallService(): Too many parameters.");

   LPSTR pcMachineName = NULL;
   if (argc >= 4)
      pcMachineName = argv[argc-1];

   const char cRunServiceSwitch[] = "\" /RUNSERVICE";
   char cImagePath[MAX_PATH+sizeof(cRunServiceSwitch)+2] = "\"";
   if (argv[2])
     strncpy(cImagePath+1, argv[2], MAX_PATH);
   else
     GetModuleFileName(NULL, cImagePath+1, MAX_PATH);
   strcat(cImagePath, cRunServiceSwitch);
   if (argc > 4)
     {
       if (strlen(cImagePath)+strlen(argv[3])+4 > sizeof(cImagePath))
	 {
	   fputs("Too long parameter.\r\n", stderr);
	   return 1;
	 }
       strcat(cImagePath, " \"");
       strcat(cImagePath, argv[3]);
       strcat(cImagePath, "\"");
     }
   
   puts("Opening Service Control Manager...");
   SC_HANDLE hSC = OpenSCManager(pcMachineName, NULL,
				 SC_MANAGER_CREATE_SERVICE);

   if (hSC == NULL)
     {
       if (win_errno == ERROR_CALL_NOT_IMPLEMENTED)
	 fputs("Error: There is no Service Control Manager available in this "
	       "operating system.\r\n", stderr);
       else
	 win_perror();
       return 1;
     }
   
   puts("Creating service...");
   SC_HANDLE hService = CreateService(hSC, LR_SERVICE_NAME,
      LR_SERVICE_DESCRIPTION, 0,
      SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS, SERVICE_AUTO_START,
      SERVICE_ERROR_NORMAL, cImagePath, NULL, NULL, NULL, NULL, NULL);

   if (hService)
   {
      puts("Service install successful.");
      CloseServiceHandle(hService);
      CloseServiceHandle(hSC);
      return 0;
   }
   else
   {
      win_perror();
      CloseServiceHandle(hSC);
      return 1;
   }
}

// Invoked with the /UNINSTALL switch. Uninstalls LightRepeater Engine as a
// Windows NT Service using Service Control Manager.
EXTERN_C int WINAPI UninstallService(int argc, char **argv)
{
   if (argc > 3)
      puts("Too many parameters.");

   puts("Opening Service Control Manager...");
   SC_HANDLE hSC = OpenSCManager(argv[2], NULL,
      SC_MANAGER_CREATE_SERVICE);

   if (hSC == NULL)
     {
       if (win_errno == ERROR_CALL_NOT_IMPLEMENTED)
	 fputs("Error: There is no Service Control Manager available in this "
	       "operating system.\r\n", stderr);
       else
         win_perror();
       return 1;
     }

   puts("Opening service...");
   SC_HANDLE hService = OpenService(hSC, LR_SERVICE_NAME, DELETE);

   if (hService)
   {
      puts("Deleting service...");
      if (DeleteService(hService))
      {
         CloseServiceHandle(hService);
         CloseServiceHandle(hSC);
         puts("Service deleted successfully.");
         return 0;
      }
      else
      {
         win_perror();
         CloseServiceHandle(hService);
         CloseServiceHandle(hSC);
         return 1;
      }
   }
   else
   {
      win_perror();
      CloseServiceHandle(hSC);
      return 1;
   }
}

