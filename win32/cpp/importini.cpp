#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <shellapi.h>

#pragma comment(lib, "shell32")
#pragma comment(lib, "advapi32")

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

// Limitation in Windows, not Windows NT.
#define MAXINIBUFFER 32767

int main(int argc, char **argv)
{
   if (argc < 3)
   {
     printf("%s\n",
	    "Usage: IMPORTINI [/SYS] inifile registrypath\r\n"
	    "\n"
	    "registrypath is registry key relative to \"HKEY_CURRENT_USER\".\r\n"
	    "\n"
	    "If /SYS is used, registrypath is registry key relative to\r\n"
	    "\"HKEY_LOCAL_MACHINE\\SOFTWARE\".");
      return 1;
   }

   HKEY hKeyRoot;
   HKEY hKey;
   DWORD dwRegErrNo;

   if (stricmp(argv[1], "/SYS") == 0)
   {
      argv++;
      hKeyRoot = HKEY_LOCAL_MACHINE;
      char cBuf[MAX_PATH];
      strcpy(cBuf, "SOFTWARE\\");
      strncat(cBuf, argv[2], sizeof(cBuf)/sizeof(*cBuf)-20);
      cBuf[sizeof(cBuf)/sizeof(*cBuf)-1] = 0;
      dwRegErrNo = RegCreateKey(hKeyRoot, cBuf, &hKey);
   }
   else
   {
      hKeyRoot = HKEY_CURRENT_USER;
      dwRegErrNo = RegCreateKey(hKeyRoot, argv[2], &hKey);
   }

   if (dwRegErrNo)
   {
      win_errno = dwRegErrNo;
      win_perror("Error creating registry key");
      return 3;
   }

   char Sections[MAXINIBUFFER];
   if (GetPrivateProfileSectionNames(Sections,
				     sizeof(Sections)/sizeof(*Sections),
				     argv[1]) < 1)
     fprintf(stderr, "No INI data in %s.\n", argv[1]);

   char Keys[MAXINIBUFFER];
   for (LPTSTR section = Sections; *section; section = strchr(section, 0)+1)
   {
     if (GetPrivateProfileSection(section, Keys, sizeof(Keys)/sizeof(*Keys),
				  argv[1]) < 1)
      {
	fprintf(stderr, "Cannot read section [%s] from %s.\n", section, 
		argv[1]);
	continue;
      }

      printf("Processing section [%s]...\n", section);

      HKEY hKeySection;
      dwRegErrNo = RegCreateKey(hKey, section, &hKeySection);
      if (dwRegErrNo)
      {
         win_errno = dwRegErrNo;
         win_perror("Error creating registry key");
         continue;
      }

      LPTSTR key = Keys;
      while (*key)
      {
         key = strtok(key, "=");
         LPTSTR value = strchr(key, 0)+1;

         dwRegErrNo = RegSetValueEx(hKeySection, key, NULL, REG_SZ,
				    (PUCHAR)value,
				    (DWORD)(strlen(value)+1)*sizeof(*value));
         if (dwRegErrNo)
         {
	   fprintf(stderr, "Error writing value %s to registry: ", key);
	   win_errno = dwRegErrNo;
	   win_perror(NULL);
         }
         else
	   printf("Wrote %s/%s='%s'\n", section, key, value);

         key = strchr(value, 0)+1;
      }

      RegCloseKey(hKeySection);
   }

   RegCloseKey(hKey);

   printf("%s\n", "Creating IniFileMapping...");
   dwRegErrNo = RegOpenKey(HKEY_LOCAL_MACHINE,
			   "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\IniFileMapping",
			   &hKey);
   if (dwRegErrNo)
   {
      win_errno = dwRegErrNo;
      win_perror("Error creating IniFileMapping");
      return 4;
   }

   char cKeyRoot[MAX_PATH];
   if (hKeyRoot == HKEY_CURRENT_USER)
     strcpy(cKeyRoot, "USR:");
   else
     strcpy(cKeyRoot, "SYS:");

   strncat(cKeyRoot, argv[2], sizeof(cKeyRoot)/sizeof(*cKeyRoot)-5);
   cKeyRoot[sizeof(cKeyRoot)/sizeof(*cKeyRoot)-1] = 0;

   dwRegErrNo = RegSetValue(hKey, argv[1], REG_SZ, cKeyRoot,
			    (DWORD)strlen(cKeyRoot)*sizeof(*cKeyRoot));
   if (dwRegErrNo)
   {
      win_errno = dwRegErrNo;
      win_perror("Error creating IniFileMapping");
      return 4;
   }

   printf("%s\n", "IniFileMapping created successfully.");

   RegCloseKey(hKey);

   return 0;
}

