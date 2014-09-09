#define WIN32_LEAN_AND_MEAN
#ifndef _DLL
#define _DLL
#endif

#include <windows.h>
#include <winstrct.hpp>
#include <minwcrtlib.h>

#ifndef _WIN64
#pragma comment(lib, "minwcrt.lib")
#endif

#pragma comment(lib, "advapi32.lib")

// This function creates the directory specified with the Path parameter and
// also creates any directories necessary above it in the tree. It works very
// much like MakeSureDirectoryPathExists() in the image helper library.
bool
CreateDirectoryPath(LPCSTR Path)
{
  for (;;)
    {
      LPSTR lpSep;

      if (CreateDirectory(Path, NULL))
	return true;

      switch (win_errno)
	{
	case ERROR_PATH_NOT_FOUND:
	  break;

	case ERROR_ALREADY_EXISTS:
	  return true;

	default:
	  return false;
	}

      LPSTR NewPath = strdup(Path);
      if (NewPath == NULL)
	return false;

      Sleep(0);

      for (lpSep = strchr(NewPath, '/');
	   lpSep != NULL;
	   lpSep = strchr(lpSep + 1, '/'))
	*lpSep = '\\';

      for (lpSep = strrchr(NewPath, '\\');
	   lpSep != NULL;
	   lpSep = strrchr(NewPath, '\\'))
	{
	  *lpSep = 0;

	  if (CreateDirectory(NewPath, NULL))
	    break;
	}

      if (lpSep == NULL)
	if (!CreateDirectory(NewPath, NULL))
	  {
	    free(NewPath);
	    return false;
	  }

      free(NewPath);
    }
}

bool
SaveKey(HKEY hKeyRoot, LPCSTR HiveName, LPCSTR DirName, LPCSTR FileName)
{
  HKEY hKey;
  LONG lRegErr;
  WHeapMem FilePath((DWORD) (strlen(DirName) + strlen(FileName) + 1));

  if (FilePath == NULL)
    {
      win_perror("Memory allocation error");
      exit(2);
    }

  strcpy(FilePath, DirName);
  strcat(FilePath, FileName);

  lRegErr = RegOpenKey(hKeyRoot, HiveName, &hKey);
  if (lRegErr != NO_ERROR)
    {
      WErrMsg errmsg(lRegErr);
      oem_printf(stderr, "%1: %2%%n", HiveName, errmsg);
      return false;
    }

  if (!DeleteFile(FilePath))
    if (win_errno != ERROR_FILE_NOT_FOUND)
      {
	WErrMsg errmsg;
	oem_printf(stderr, "Cannot delete existing '%1': %2%%n",
		   FilePath, errmsg);
	return false;
      }

  oem_printf(stdout, "%1%%n", FilePath);

  lRegErr = RegSaveKey(hKey, FilePath, NULL);
  if (lRegErr != NO_ERROR)
    {
      WErrMsg errmsg(lRegErr);
      oem_printf(stderr, "%1: %2%%n", FilePath, errmsg);
      return false;
    }
  
  RegCloseKey(hKey);
  return true;
}

bool
GetUserRegistryFileName(LPCSTR UserKeyName, LPSTR FilePath, DWORD dwFPSize)
{
  char *sidstr = strdup(UserKeyName);
  if (sidstr == NULL)
    {
      perror("Memory allocation error");
      exit(2);
    }

  char *classptr = strstr(sidstr, "_Classes");

  if (classptr != NULL)
    *classptr = 0;

  LPCSTR subkey = NULL;

  if (classptr != NULL)
    subkey = "\\Software\\Microsoft\\Windows\\CurrentVersion\\"
      "Explorer\\Shell Folders";
  else
    subkey = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\";

  WHeapMem key_path((DWORD) (strlen(sidstr) + strlen(subkey) + 1));
  if (key_path == NULL)
    {
      win_perror("Memory allocation error");
      exit(2);
    }

  if (classptr != NULL)
    {
      strcpy(key_path, sidstr);
      strcat(key_path, subkey);
    }
  else
    {
      strcpy(key_path, subkey);
      strcat(key_path, sidstr);
    }

  free(sidstr);

  HKEY hKey;
  LONG lRegErr;

  if (classptr != NULL)
    lRegErr = RegOpenKey(HKEY_USERS, key_path, &hKey);
  else
    lRegErr = RegOpenKey(HKEY_LOCAL_MACHINE, key_path, &hKey);

  if (lRegErr != NO_ERROR)
    {
      WErrMsg errmsg;
      if (classptr != NULL)
	{
	  oem_printf(stderr, "Error opening HKU\\%1: %2%%n", key_path, errmsg);
	  return false;
	}
      else
	{
	  oem_printf(stderr,
		     "Error opening HKLM\\%1: %2%%n", key_path, errmsg);
	  exit(1);
	}
    }

  DWORD dwSize = dwFPSize;
  DWORD dwRegType;
  lRegErr = RegQueryValueEx(hKey, classptr != NULL ?
			    "Local AppData" : "ProfileImagePath", NULL,
			    &dwRegType, (LPBYTE) FilePath, &dwSize);
  if (lRegErr != NO_ERROR)
    {
      WErrMsg errmsg;
      oem_printf
	(stderr,
	 "Error getting path to registry file for user profile HKU\\%1: %2%%n",
	 UserKeyName, errmsg);
      return false;
    }

  switch (dwRegType)
    {
    case REG_EXPAND_SZ:
      {
	char *envstr = strdup(FilePath);
	if (envstr == NULL)
	  {
	    perror("Memory allocation error");
	    exit(2);
	  }

	ExpandEnvironmentStrings(envstr, FilePath, dwFPSize);
	free(envstr);
	
	FilePath[dwFPSize - 1] = 0;
	dwSize = (DWORD) (strlen(FilePath) + 1);
      }

    case REG_SZ:
      if (classptr != NULL)
	strncat(FilePath, "\\Microsoft\\Windows\\", dwFPSize - dwSize);
      else
	strncat(FilePath, "\\", dwFPSize - dwSize);

      FilePath[dwFPSize - 1] = 0;

      memmove(FilePath, FilePath + 3, strlen(FilePath + 3) + 1);
      return true;

    default:
      oem_printf
	(stderr,
	 "Error getting path to registry file for user profile HKU\\%1: "
	 "Wrong registry value data type.%%n", UserKeyName);
      return false;
    }
}

int
main(int argc, char **argv)
{
  SetOemPrintFLineLength(GetStdHandle(STD_ERROR_HANDLE));

  if (argc > 1)
    {
      if ((argv[1][0] == '-') | (argv[1][0] == '/'))
	{
	  puts("Registry dump tool.\r\n"
	       "Copyright (c) Olof Lagerkvist, 2005-2006\r\n"
	       "http://www.ltr-data.se     olof@ltr-data.se\r\n"
	       "\n"
	       "Usage:\r\n"
	       "regdump [targetdirectory]\r\n"
	       "\n"
	       "This tool creates a directory tree in the current directory or the directory\r\n"
	       "specified by targetdirectory parameter. Currently loaded registry database\r\n"
	       "files are dumped into this directory tree.\r\n");
	  return 1;
	}

      if (!SetCurrentDirectory(argv[1]))
	{
	  WErrMsg errmsg;
	  oem_printf(stderr, "Error changing directory to '%1': %2%%n",
		     argv[1], errmsg);
	  return 2;
	}
    }

  EnableBackupPrivileges();

  char dir[MAX_PATH];
  if (!GetSystemDirectory(dir, sizeof dir))
    {
      win_perror("Error getting Windows system directory path");
      return 2;
    }

  const char config_subdir[] = "\\config\\";
  WHeapMem config_dir((DWORD) (strlen(dir + 3) + sizeof config_subdir));
  if (config_dir == NULL)
    {
      win_perror("Memory allocation error");
      return 2;
    }

  strcpy(config_dir, (dir + 3));
  strcat(config_dir, config_subdir);

  if (!CreateDirectoryPath(config_dir))
    {
      WErrMsg errmsg;
      oem_printf(stderr, "Error creating directory tree '%1': %2%%n",
		 config_dir, errmsg);
      return 2;
    }

  SaveKey(HKEY_LOCAL_MACHINE, "SAM", config_dir, "SAM");
  SaveKey(HKEY_LOCAL_MACHINE, "SECURITY", config_dir, "security");
  SaveKey(HKEY_LOCAL_MACHINE, "SOFTWARE", config_dir, "software");
  SaveKey(HKEY_LOCAL_MACHINE, "SYSTEM", config_dir, "system");
  SaveKey(HKEY_USERS, ".Default", config_dir, "default");

  LONG lRegErr;
  
  DWORD dwIdx = 0;
  char subkey[MAX_PATH];
  while ((lRegErr = RegEnumKey(HKEY_USERS, dwIdx++, subkey, sizeof subkey)) ==
	 NO_ERROR)
    {
      if (strcmpi(subkey, ".Default") == 0)
	continue;

      if (!GetUserRegistryFileName(subkey, dir, sizeof dir))
	continue;

      if (!CreateDirectoryPath(dir))
	{
	  WErrMsg errmsg;
	  oem_printf(stderr, "Error creating '%1': %2%%n", dir, errmsg);
	  continue;
	}
      
      SaveKey(HKEY_USERS,
	      subkey,
	      dir,
	      strstr(subkey, "_Classes") != NULL ?
	      "UsrClass.dat" : "NTUSER.DAT");
    }

  if (lRegErr != ERROR_NO_MORE_ITEMS)
    win_perror("Error enumerating HKEY_USERS");

  return 0;
}
