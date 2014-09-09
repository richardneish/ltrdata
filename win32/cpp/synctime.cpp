#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0350

#include <winstrct.h>
#include <winsock2.h>
#include <wtime.h>

#if defined(_DLL) && !defined(_WIN64)
#include <minwcrtlib.h>
#pragma comment(lib, "minwcrt.lib")
#else
#include <stdlib.h>
#endif

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "wsock32.lib")

#define TCP_WINDOW_SIZE 1024

const DWORD dwStdServers[] =
{
  0x84a38783,
  0xc02bf412,
  0x84a30466,
  0x84a30467,
  0x84a30465,
  0x81060f1d,
  0xcc46830d,
  0x836b010a,
};

bool bLogEvents = false;

void
ReportMsg(DWORD dwErrorCode, LPCSTR lpMsg, ...)
{
  va_list param_list;
  LPSTR lpBuf = NULL;

  va_start(param_list, lpMsg);

  if (!FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		      FORMAT_MESSAGE_FROM_STRING, lpMsg, 0, 0, (LPSTR) &lpBuf,
		      0, &param_list))
    return;

  if (bLogEvents)
    {
      HANDLE hEventLog = RegisterEventSource(NULL, "SyncTime");
      if (hEventLog == NULL)
	{
	  LocalFree(lpBuf);
	  return;
	}
      
      ReportEvent(hEventLog, EVENTLOG_WARNING_TYPE, 0, dwErrorCode, NULL, 1, 0,
		  (LPCSTR *) &lpBuf, NULL);
      
      DeregisterEventSource(hEventLog);
    }
  else
    MessageBox(NULL, lpBuf, "SyncTime", MB_ICONEXCLAMATION);

  LocalFree(lpBuf);
}

int PASCAL
WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  WSADATA wd;
  if (WSAStartup(0x0002, &wd))
    return -1;

  if (__argc > 1)
    if (stricmp(__argv[1], "/LOG") == 0)
      {
	bLogEvents = true;
	__argc--;
	__argv++;
      }

  if (bLogEvents)
    {
      HKEY regkey;
      if (RegCreateKey(HKEY_LOCAL_MACHINE,
		       "SYSTEM\\CurrentControlSet\\Services\\"
		       "EventLog\\Application\\SyncTime",
		       &regkey) == NO_ERROR)
	{
	  const char szDLLFile[] = "%SystemRoot%\\System32\\KERNEL32.dll";
	  RegSetValueEx(regkey, "EventMessageFile", 0, REG_EXPAND_SZ,
			(LPBYTE)szDLLFile, sizeof szDLLFile);
	  DWORD dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE |
            EVENTLOG_INFORMATION_TYPE;
	  RegSetValueEx(regkey, "TypesSupported", 0, REG_DWORD,
			(LPBYTE)&dwData, sizeof dwData);
	  RegCloseKey(regkey);
	}
    }

  char *hname = NULL;
  if (__argc > 1)
    hname = __argv[1];

  // Get port name for service
  u_short port;
  if (__argc > 2)
    {
      port = (u_short)strtoul(__argv[2], NULL, 0);
      if (port == 0)
	{
	  servent *service = getservbyname(__argv[2], "tcp");
	  if (service)
            port = service->s_port;
	  else
            port = htons(13);
	}
    }
  else
    port = htons(13);
  
  // Open socket
  SOCKET sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sd == INVALID_SOCKET)
    {
      ReportMsg(h_errno, "Fatal WinSock error, cannot create socket:%n%1",
		h_error);
      return -1;
    }
  
  if (hname)
    {
      // Get server address
      DWORD haddr = inet_addr(hname);
      
      if (haddr == INADDR_NONE)
	{
	  hostent *hent = gethostbyname(hname);
	  if (!hent)
	    {
	      ReportMsg(h_errno,
			"Host name lookup failed for '%1':%n"
			"%2"
			"Time is not updated",
			hname, h_error);
	      return 1;
	    }
	  
	  haddr = *(DWORD*)hent->h_addr;
	}

      sockaddr_in addr = { 0 };
      addr.sin_family = AF_INET;
      addr.sin_port = port;
      addr.sin_addr.s_addr = haddr;

      if (connect(sd, (sockaddr*)&addr, sizeof addr) == SOCKET_ERROR)
	{
	  ReportMsg(h_errno, "Cannot connect to '%1':%n"
		    "%2"
		    "Time is not updated.", hname, h_error);
	  return 1;
	}
    }
  else
    {
      DWORD dwLGS = 0;
      HKEY hKey = NULL;
      char szRegBuffer[15];
      if (RegCreateKey(HKEY_CLASSES_ROOT,
		       "SOFTWARE\\Olof_Lagerkvist\\SyncTime",
		       &hKey) == ERROR_SUCCESS)
	{
	  LONG lSize = sizeof szRegBuffer;
	  if (RegQueryValue(hKey, "LastGoodServer", szRegBuffer, &lSize) ==
	      ERROR_SUCCESS)
	    dwLGS = strtoul(szRegBuffer, NULL, 0);
	  else
            dwLGS = 0;
	}

      if (dwLGS >= sizeof(dwStdServers) / sizeof(DWORD))
	dwLGS = 0;

      DWORD dwIndex = dwLGS;
      for(;;)
	{
	  sockaddr_in addr;
	  addr.sin_family = AF_INET;
	  addr.sin_port = port;
	  addr.sin_addr.s_addr = htonl(dwStdServers[dwIndex]);
	  ZeroMemory(addr.sin_zero, sizeof addr.sin_zero);

	  if (connect(sd, (sockaddr*)&addr, sizeof addr) == SOCKET_ERROR)
	    {
	      if (++dwIndex >= sizeof(dwStdServers) / sizeof(DWORD))
		dwIndex = 0;

	      if (dwIndex == dwLGS)
		break;

	      continue;
	    }

	  _ultoa(dwIndex, szRegBuffer, 10);
	  RegSetValue(hKey, "LastGoodServer", REG_SZ,
		      szRegBuffer, (DWORD) strlen(szRegBuffer));

	  hname = inet_ntoa(addr.sin_addr);
	  break;
	}
      
      if (hname == NULL)
	{
	  ReportMsg(WSAECONNREFUSED,
		    "None of the default time servers are "
		    "available. Time is not updated.");
	  return 1;
	}
    }

  char packet[TCP_WINDOW_SIZE];

  int iLen = 0;
  for (char *ptr = packet; iLen < sizeof(packet) - 2; )
    {
      int iReadLen = recv(sd, ptr, sizeof(packet)-iLen-2, 0);
      if (iReadLen == SOCKET_ERROR)
	{
	  ReportMsg(h_errno,
		    "Error receiving data from '%1':%n"
		    "%2"
		    "Time is not updated.",
		    hname, h_error);
	  return 1;
	}
      
      if (iReadLen == 0)
	break;

      iLen += iReadLen;
      ptr += iReadLen;
    }

  packet[iLen] = '\x00';

#ifdef _DEBUG
  MessageBox(NULL, packet, 0, 0);
#endif

  WSystemTime st;
  DWORD dwDummy;
  sscanf(packet, "%u %u-%u-%u %u:%u:%u %u %u %u %u", &dwDummy, &st.wYear,
	 &st.wMonth, &st.wDay, &st.wHour, &st.wMinute, &st.wSecond, &dwDummy,
	 &dwDummy, &dwDummy, &st.wMilliseconds);
  
  st.wYear += (WORD) 2000;

  if (st.SetUTC())
    {
#ifdef _DEBUG
      stringstream sout;
      sout << st.wYear << '-' << st.wMonth << '-' << st.wDay << ", " <<
	st.wHour << ':' << st.wMinute << ':' << st.wSecond << '.' <<
	st.wMilliseconds << ends;
      MessageBox(NULL, sout.str().c_str(), "", MB_ICONINFORMATION);
#endif
      if (!bLogEvents)
	{
	  LPSTR lpBuf = NULL;
	  if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			     FORMAT_MESSAGE_FROM_STRING |
			     FORMAT_MESSAGE_ARGUMENT_ARRAY,
			     "Time updated successfully from server %1.", 0, 0,
			     (LPSTR) &lpBuf, 0, (va_list *) &hname))
	    {
	      MessageBox(NULL, lpBuf, "SyncTime", MB_ICONINFORMATION);
	      LocalFree(lpBuf);
	    }
	}

      return 0;
    }

  ReportMsg(win_errno,
	    "The format of the time string received from the server '%1' is "
	    "not compatible with this program:%n"
	    "%2"
	    "Time is not updated.",
	    hname, win_error);
  
  return 2;
}
