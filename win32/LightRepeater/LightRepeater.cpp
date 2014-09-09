/* LightRepeater Engine
 *
 * LightRepeater v 1.01 by SM6XMK, Olof Lagerkvist
 * Send comments and suggestions to sm6xmk@ssa.se
 *
 * Latest version available at http://www.qsl.net/sm6xmk/apps/LightRepeater/
 *
 * This program controls a repeater through a COM port.
 * Connect COM port CTS line to RX squelch line
 * Connect COM port RTS line to TX PTT line
 * Connect COM port RING line to RX tone call detection line
 *
 * Command line usage:
 * LightRepeater [COMn]
 * where COMn is the COM port that is to be used. Default is COM1.
 * In version 1.01 a new command line switch was introduced:
 * LightRepeater /STOP
 * Use this command to stop all running instances of LightRepeater Engine.
 *
 * Statistics are updated in LightRep_stat.dat (the file is created if it does
 * not exist). Use lightrep_stat.exe utility to view data in statistics file.
 *
 *  SM6XMK LightRepeater, Repeater control software
 *  Copyright (C) 2001  SM6XMK, Olof Lagerkvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#define WINVER          0x0400
#define _WIN32_WINDOWS  0x0400
#define _WIN32_WINNT    0x0400
#define WIN32_LEAN_AND_MEAN

#include <winstrct.h>
#include <mmsystem.h>

#include <wcomm.h>
#include <wfilever.h>

#include <stdio.h>
#include <process.h>

#include "lightrep_log.h"
#include "ntservice.h"

// Comment this line out to get carrier wave opened repeater
#define LR_CHECK_TONE_CALL

// Repeater behaviour constants
#define LR_TAIL_TIMEOUT      10000   // milliseconds open after squelch close
#define LR_ID_PERIOD        600000   // milliseconds between repeater ID sounds
#define LR_MAX_TALK_TIME    300000   // milliseconds max talk time or INFINITE
#define LR_MIN_TONECALL_DUR    200   // minimum milliseconds tone call duration
#define LR_TONECALL_TIMEOUT  10000   // maximum milliseconds tone call duration
#define LR_DEBOUNCE_TIME        80   // milliseconds sleep after wait completed

// If LR_SQL_OPEN_CANCEL_SOUND is defined, squelch open stop all computer
// generated sounds
#define LR_SQL_OPEN_CANCEL_SOUND

// COM escape commands to start/stop TX. Swap SETRTS and CLRRTS if PTT line
// is inverted (RTS low=TX On, RTS high=TX Off). You can also use SETDTR and
// CLRDTR if you plan to use DTR pin instead of RTS.
enum LR_TX_CTL
{
  LR_TX_ON = SETRTS,
  LR_TX_OFF = CLRRTS
};

// Uncomment definitions if any COM port pin is inverted,
// i.e. 1=squelch closed, 0=squelch opened
//#define LR_INVERT_SQUELCH
//#define LR_INVERT_TONE_CALL

// Use control panel settings for repeater sounds
#define LR_SIGNOFF_SOUND         "LR_Signoff"
#define LR_COURTESY_SOUND        "LR_Courtesy"
#define LR_STARTUP_SOUND         "LR_Startup"
#define LR_ID_SOUND              "LR_RepeaterID"
#define LR_LONG_SPEAKER_SOUND    "LR_LongSpeaker"
#define LR_GUMMITUMME_SOUND      "LR_Gummitumme"
#define LR_TIME_ANNOUNCE_SOUND   "LR_TimeAnnounce"

// Global name of quit event. If this event is signalled all instances of
// LightRepeater Engine will exit.
#define LR_QUIT_EVENT_NAME       "LRQuitEvent"
// Path to registry key
#define LR_REG_ROOT_KEY          HKEY_LOCAL_MACHINE
#define LR_REG_KEY_NAME          "Software\\SM6XMK\\LightRepeater"

#pragma comment(lib, "winmm")
#pragma comment(lib, "advapi32")
#pragma comment(lib, "version")

// Global variables
HANDLE hComm = NULL;		// Handle to open COM port
DWORD dwLastIDTime = 0;		// Last ID time (0=Closed, 1=Opened)
DWORD dwOpenTime = 0;		// Last open time
bool bSquelchOpened = false;	// To check if already catched squelch close
bool bTailTimerEnabled = false;	// True if waiting to close
LRStatistics *plrs = NULL;	// Mapping of statistics file
OVERLAPPED oLineWait;		// Async comm wait object
HKEY hkAppKey = NULL;		// Registry key
HANDLE hAnnounceThread = NULL;	// Thread handle to AnnounceEngine()
HANDLE hRepeaterThread = NULL;	// Thread handle to RepeaterEngine()
HANDLE hQuitEvent = NULL;	// Global event, signal using /STOP switch
HANDLE hCtrlEvent = NULL;	// Local event, signal when console closing
SERVICE_STATUS NTServiceStatus;	// Status codes if running as a NT service
SERVICE_STATUS_HANDLE NTServiceStatusHandle = NULL;
DWORD dwSndAliasFlag = 0;	// Set by main() to SND_APPLICATION or
				 // SND_ALIAS depending on OS version.

// Function declarations
int NoServiceMain();
int Initialize();
inline bool PrepareCommPort();

BOOL WINAPI CtrlHandler(DWORD);

void AnnounceEngine(void *);
void PlayAnnouncements();
void RepeaterEngine(void *);

inline DWORD GetRX();
inline void SetTX(LR_TX_CTL);
void StartWaitRXEvent();

// The Windows NT Service Callback interface
void CALLBACK ServiceMain(DWORD, LPTSTR *);
void CALLBACK ServiceHandler(DWORD);

// AnnounceEngine() is the announcement thread. It wakes up ones a minute to
// check if there are any announcements to play.
void
AnnounceEngine(void *)
{
  try
  {
    for (;;)
      {
	Sleep(1000);		// Wait one second to prevent double event

	SYSTEMTIME st;
	GetLocalTime(&st);

	// Sync minutes
	if (st.wSecond)
	  {
	    Sleep((DWORD) (60 - st.wSecond) * 1000);
	    GetLocalTime(&st);
	  }

	// No announcements if repeater is open
	if (bSquelchOpened | bTailTimerEnabled)
	  continue;

	// Check if there is an announcement for the current time
	char cBuf[MAX_PATH + 1];
	char cHourBuf[6];
	wsprintf(cHourBuf, "%02i:%02i", st.wHour, st.wMinute);
	DWORD dwBufSiz = sizeof(cBuf);
	if (RegQueryValueEx
	    (hkAppKey, cHourBuf, NULL, NULL, (LPBYTE) cBuf,
	     &dwBufSiz) != ERROR_SUCCESS)
	  dwBufSiz = 0;

	// If there was an announcement (or blank=default) in the registry
	if (dwBufSiz)
	  {
	    DWORD dwTXStartTime = GetTickCount();
	    SetTX(LR_TX_ON);	// Turn on transmitter

	    PlayAnnouncements();

	    // If repeater is not open, shut down transmitter
	    if (!bSquelchOpened & !bTailTimerEnabled)
	      {
		SetTX(LR_TX_OFF);	// Turn off transmitter

		plrs->dwTotalTXTime +=
		  (GetTickCount() - dwTXStartTime + 500) / 1000;
		plrs->Flush();
	      }
	  }
      }
  }
  catch(bool bError)
  {
    if (bError)
      MessageBox(NULL, "Unexpected error in AnnounceEngine thread.\n\r"
		 "AnnounceEngine is stopping.", "LightRepeater Engine",
		 MB_ICONSTOP | (WinVer_Major <
				4 ? MB_SERVICE_NOTIFICATION_NT3X :
				MB_SERVICE_NOTIFICATION));
  }
  SetTX(LR_TX_OFF);
  puts("AnnounceEngine stopping.");
}

// PlayAnnouncements() checks if there are any announcement to play at the
// current time. Time announcement and repeater ID are cancelled if
// bSquelchOpened or bTailTimerEnabled gets true values during execution.
// The function does not turn TX on or off.
void
PlayAnnouncements()
{
  SYSTEMTIME st;
  GetLocalTime(&st);
  char cBuf[MAX_PATH + 1];	// Buffer for filenames and registry values
  DWORD dwBufSiz;

  // Check if there is an announcement for the current time
  char cHourBuf[6];
  wsprintf(cHourBuf, "%02i:%02i", st.wHour, st.wMinute);
  dwBufSiz = sizeof(cBuf);
  if (RegQueryValueEx
      (hkAppKey, cHourBuf, NULL, NULL, (LPBYTE) cBuf,
       &dwBufSiz) != ERROR_SUCCESS)
    dwBufSiz = 0;

  // If there was an announcement in the registry
  if (dwBufSiz > 1)
    {
      printf("%s registered announcement.\n", cHourBuf);
      PlaySound(cBuf, NULL, SND_NODEFAULT | SND_NOSTOP | SND_SYNC);
      Sleep(LR_DEBOUNCE_TIME);
    }
  else if (dwBufSiz == 1)	// Empty registry value means default announcement
    {
      printf("%s default announcement.\n", cHourBuf);
      PlaySound(LR_TIME_ANNOUNCE_SOUND, NULL, dwSndAliasFlag | SND_NODEFAULT |
		SND_SYNC | SND_NOWAIT);
      Sleep(LR_DEBOUNCE_TIME);
    }

  // Get path to voice files from registry value VoicePath
  dwBufSiz = sizeof(cBuf) - 20;
  if ((win_errno = RegQueryValueEx(hkAppKey, "VoicePath", NULL, NULL,
				   (LPBYTE) cBuf,
				   &dwBufSiz)) != ERROR_SUCCESS)
    dwBufSiz = 0;

  // Make sure a file name can be appended to path
  if (dwBufSiz)
    if ((cBuf[dwBufSiz - 1] == '\\') | (cBuf[dwBufSiz - 1] == ':'))
      --dwBufSiz;
    else
      {
	cBuf[dwBufSiz - 1] = '\\';
	cBuf[dwBufSiz] = NULL;
      }

  // If repeater is opened, abort announcements
  if (bSquelchOpened | bTailTimerEnabled)
    return;

  puts("Announcing time.");

  strcpy(cBuf + dwBufSiz, "time is.wav");
  PlaySound(cBuf, NULL, SND_FILENAME | SND_NODEFAULT | SND_NOSTOP | SND_SYNC);
  wsprintf(cBuf + dwBufSiz, "%i.wav", st.wHour);
  PlaySound(cBuf, NULL, SND_FILENAME | SND_NODEFAULT | SND_NOSTOP | SND_SYNC);
  if (st.wMinute)
    {
      strcpy(cBuf + dwBufSiz, "point.wav");
      PlaySound(cBuf, NULL,
		SND_FILENAME | SND_NODEFAULT | SND_NOSTOP | SND_SYNC);
      wsprintf(cBuf + dwBufSiz, "%i.wav", st.wMinute / 10);
      PlaySound(cBuf, NULL,
		SND_FILENAME | SND_NODEFAULT | SND_NOSTOP | SND_SYNC);
      wsprintf(cBuf + dwBufSiz, "%i.wav", st.wMinute % 10);
      PlaySound(cBuf, NULL,
		SND_FILENAME | SND_NODEFAULT | SND_NOSTOP | SND_SYNC);
      Sleep(LR_DEBOUNCE_TIME);
    }

  Sleep(500);

  // If repeater is opened, abort announcements
  if (bSquelchOpened | bTailTimerEnabled)
    return;

  // Identify
  puts("Repeater ID playing.");
  PlaySound(LR_ID_SOUND, NULL, dwSndAliasFlag | SND_NODEFAULT | SND_SYNC |
	    SND_NOWAIT);
  Sleep(LR_DEBOUNCE_TIME);
}

// CtrlHandler() is the console Ctrl handler routine. It is only used as a
// callback function. It signals the hCtrlEvent to tell main thread to exit.
BOOL WINAPI
CtrlHandler(DWORD dwCtrlType)
{
  if (dwCtrlType == CTRL_LOGOFF_EVENT)
    return false;

  if (NTServiceStatusHandle)
    {
      puts("Console break. Stopping service...");
      NTServiceStatus.dwCurrentState = SERVICE_STOPPED;
      NTServiceStatus.dwWin32ExitCode = NO_ERROR;
      return SetServiceStatus(NTServiceStatusHandle, &NTServiceStatus);
    }
  else
    {
      if (hCtrlEvent == NULL)
	return false;

      puts("Console break. Signalling event.");
      return SetEvent(hCtrlEvent);
    }
}

main()
{
  // We must manually allocate a console window if running as a service
  AllocConsole();

  // Special Win32s handling
  if (WinVer_Win32s)
    {
      MessageBox(NULL, "LightRepeater Engine cannot run under Windows 3.x.",
		 "LightRepeater Engine", MB_ICONSTOP);
      return -1;
    }

  // No application-specific sound associations in NT 3.5x
  if (WinVer_Major < 4)
    dwSndAliasFlag = SND_ALIAS;
  else
    dwSndAliasFlag = SND_APPLICATION;

  if (__argv[1] ? (__argv[1][0] | 0x02) == '/' : false)
    if (stricmp(__argv[1], "/STOP") == 0)
      ;
    else if (stricmp(__argv[1], "/RUNSERVICE") == 0)
      {
	freopen("CONOUT$", "a", stdout);
	freopen("CONOUT$", "a", stderr);
      }
    else if (stricmp(__argv[1], "/INSTALL") == 0)
      ;
    else if (stricmp(__argv[1], "/UNINSTALL") == 0)
      ;
    else
      {
	puts("The LightRepeater Engine, usage syntax:\r\n"
	     "LightRepeater [COMn]\r\n"
	     "Where n is the number of the COM port the repeater control\r\n"
	     "lines are connected to, default is COM1. CTS should be\r\n"
	     "connected to squelch line, RING to tone call line and RTS\r\n"
	     "to transmitter keying.\r\n"
	     "\n"
	     "If you want to stop running instance(s) of LightRepeater Engine:\r\n"
	     "LightRepeater /STOP\r\n"
	     "\n"
	     "If you want to install LightRepeater Engine as a Windows NT Service:\r\n"
	     "LightRepeater /INSTALL [exefile [machine]]\r\n"
	     "LightRepeater /UNINSTALL [machine]\r\n"
	     "\n"
	     "/INSTALL     Installs LightRepeater Engine as a Windows NT service.\n\r"
	     "exefile      Name of LightRepeater.exe with fully qualified path.\n\r"
	     "machine      Installs/uninstalls LightRepeater Engine service on another\r\n"
	     "             computer.\n\r"
	     "/UNINSTALL   Uninstalls LightRepeater Engine as a Windows NT service.\n\r"
	     "\n"
	     "Syntax to start/stop LightRepeater Engine as a Windows NT service:\r\n"
	     "NET START \"LightRepeater Engine\"\n\r"
	     "NET STOP \"LightRepeater Engine\"");

	return 0;
      }

  // About message from file version resource
  char cFileNameBuf[MAX_PATH];
  WFileVerInfo *pfvi = NULL;
  char *pcDescr = NULL;
  char *pcVer = NULL;
  if (GetModuleFileName
      (GetModuleHandle(NULL), cFileNameBuf, sizeof cFileNameBuf))
    {
      // About message from file version resource
      pfvi = new WFileVerInfo(cFileNameBuf);
      pcDescr = (char *)pfvi->QueryValue();
      pcVer =
	(char *)pfvi->QueryValue("\\StringFileInfo\\040904E4\\FileVersion");
    }

  if (pcDescr)
    SetConsoleTitle(pcDescr);

  printf("\n --->                 %s                 <---\n",
	 pcDescr ? pcDescr : "SM6XMK LightRepeater Engine");

  puts
    (" --->     Repeater controlling software by SM6XMK, Olof Lagerkvist.        <---\r\n"
     " --->            Copyright (C) 2001  SM6XMK Olof Lagerkvist                <---\r\n"
     " --->  Project's web site: http://www.qsl.net/sm6xmk/apps/LightRepeater/   <---\r\n"
     " --->  This is free software distributed under the terms in COPYING.TXT.   <---\r\n");

  printf
    (" --->      (compiled %s, send comments to sm6xmk@ssa.se)          <---\n",
     __DATE__);

  if (pfvi)
    delete pfvi;

  // Install console window close event handler
  SetConsoleCtrlHandler(CtrlHandler, true);

  // If running as a service...
  if (__argv[1] ? stricmp(__argv[1], "/RUNSERVICE") == 0 : false)
    {
      // Next parameter may be comm port
      ++__argv;
      --__argc;

      puts("The Windows NT Service interface is loading...");

      SERVICE_TABLE_ENTRY ste[] = {
	{LR_SERVICE_DESCRIPTION, ServiceMain}
	,
	{NULL, NULL}
      };

      if (!StartServiceCtrlDispatcher(ste))
	{
	  WErrMsg errmsg;
	  MessageBox(NULL, errmsg, LR_SERVICE_DESCRIPTION, MB_ICONSTOP |
		     (WinVer_Major < 4 ? MB_SERVICE_NOTIFICATION_NT3X :
		      MB_SERVICE_NOTIFICATION));
	  return 1;
	}
    }
  else if (__argv[1] ? stricmp(__argv[1], "/INSTALL") == 0 : false)
    return InstallService(__argc, __argv);
  else if (__argv[1] ? stricmp(__argv[1], "/UNINSTALL") == 0 : false)
    return UninstallService(__argc, __argv);
  else
    {
      if (NoServiceMain())
	return 1;
    }

  puts("LightRepeater Engine is shutting down...");

  if (hRepeaterThread)
    {
      puts("Stopping RepeaterEngine...");
      SuspendThread(hRepeaterThread);
    }

  if (hAnnounceThread)
    {
      puts("Stopping AnnounceEngine...");
      SuspendThread(hAnnounceThread);
    }

  // Make sure transmitter is off now
  SetTX(LR_TX_OFF);

  delete plrs;

  if (hkAppKey)
    RegCloseKey(hkAppKey);

  CloseHandle(hComm);

  return 0;
}

VOID WINAPI
ServiceMain(DWORD /* dwArgc */ , LPTSTR * /* lpszArgv */ )
{
  NTServiceStatusHandle =
    RegisterServiceCtrlHandler(LR_SERVICE_NAME, ServiceHandler);

  if (NTServiceStatusHandle == NULL)
    return;

  NTServiceStatus.dwCurrentState = SERVICE_RUNNING;

  NTServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS |
    SERVICE_INTERACTIVE_PROCESS | SERVICE_INTERACTIVE_PROCESS;

  NTServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP |
    SERVICE_ACCEPT_SHUTDOWN;

  NTServiceStatus.dwWin32ExitCode = 0;
  NTServiceStatus.dwServiceSpecificExitCode = 0;
  NTServiceStatus.dwCheckPoint = 0;
  NTServiceStatus.dwWaitHint = 0;

  SetServiceStatus(NTServiceStatusHandle, &NTServiceStatus);

  NTServiceStatusHandle =
    RegisterServiceCtrlHandler(LR_SERVICE_NAME, ServiceHandler);

  if (NTServiceStatusHandle == NULL)
    return;

  // If init OK, return
  if (Initialize() == 0)
    return;

  // If init not OK, stop service
  NTServiceStatus.dwCurrentState = SERVICE_STOPPED;
  NTServiceStatus.dwWin32ExitCode = win_errno;

  MessageBox(NULL, "Initialization of LightRepeater Engine failed.",
	     LR_SERVICE_DESCRIPTION, MB_ICONSTOP |
	     (WinVer_Major < 4 ? MB_SERVICE_NOTIFICATION_NT3X :
	      MB_SERVICE_NOTIFICATION));
  SetServiceStatus(NTServiceStatusHandle, &NTServiceStatus);
}

// Service handler. Dispatches control codes from Service Control Manager.
VOID WINAPI
ServiceHandler(DWORD dwCtrl)
{
  NTServiceStatus.dwWin32ExitCode = 0;

  // Check which control code.
  switch (dwCtrl)
    {
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN:
      NTServiceStatus.dwCurrentState = SERVICE_STOPPED;
      break;
    }

  SetServiceStatus(NTServiceStatusHandle, &NTServiceStatus);
}

// Replacement for ServiceMain() if not running as a Windows NT Service.
// The function wait for either termination of a RepeaterEngine thread or
// signalling of the local or global quit event.
int
NoServiceMain()
{
  ///////// INITIALIZE ENGINE EVENT HANDLING

  // Create event objects. The main thread waits for these events, and when
  // signaled it terminates engine threads and exits the process.
  hQuitEvent = CreateEvent(NULL, true, false, LR_QUIT_EVENT_NAME);
  if (!hQuitEvent)
    {
      win_perror("CreateEvent() failed (hQuitEvent)");
      return 1;
    }

  // If /STOP switch at command line, signal global event to stop all
  // instances of LightRepeater Engine.
  if (__argv[1] ? stricmp(__argv[1], "/STOP") == 0 : false)
    {
      puts("Stop requested. Signalling global quit event.");
      SetEvent(hQuitEvent);
      return 1;
    }

  hCtrlEvent = CreateEvent(NULL, true, false, NULL);
  if (!hCtrlEvent)
    {
      win_perror("CreateEvent() failed (hCtrlEvent)");
      return 1;
    }

  if (Initialize())
    return 1;

  // An exception in RepeaterEngine will terminate that thread. We wait
  // for quit events or hRepeaterThread termination.
  if (hRepeaterThread)
    {
      HANDLE hExitEvents[] = { hRepeaterThread, hCtrlEvent, hQuitEvent };
      WaitForMultipleObjects(sizeof(hExitEvents) / sizeof(HANDLE),
			     hExitEvents, false, INFINITE);
    }
  else
    {
      HANDLE hExitEvents[] = { hCtrlEvent, hQuitEvent };
      WaitForMultipleObjects(sizeof(hExitEvents) / sizeof(HANDLE),
			     hExitEvents, false, INFINITE);
    }
  return 0;
}

// Open and init comm port, create threads
int
Initialize()
{
  ///////// OPEN AND INITIALIZE COMM PORT

  // Get COM port from command line parameter
  char *pcCommFile = __argc >= 2 ? __argv[1] : "COM1";

  printf("Opening port %s\n", pcCommFile);
  hComm = CreateFile(pcCommFile, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		     OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

  if (hComm == INVALID_HANDLE_VALUE)
    {
      win_perror(pcCommFile);
      return 1;
    }

  // Initialization code
  if (!PrepareCommPort())
    {
      fputs("Initialization failed. Bailing out.\r\n", stderr);
      return 1;
    }

  ///////// PREPARE STATISTICS, OPEN REGISTRY KEY AND START ENGINE THREADS

  plrs = new LRStatistics;	// Open statistics object

  // Open registry key
  win_errno = RegOpenKeyEx(LR_REG_ROOT_KEY, LR_REG_KEY_NAME, 0, KEY_READ,
			   &hkAppKey);
  if (win_errno)
    {
      win_perror("RegOpenKeyEx() failed");
      hkAppKey = NULL;
    }
  else
    {
      // AnnounceEngine thread creation
      puts("AnnounceEngine is starting up...");
      hAnnounceThread = (HANDLE) _beginthread(AnnounceEngine, 0, NULL);
      if ((LONG) hAnnounceThread == -1)
	hAnnounceThread = NULL;

      if (hAnnounceThread == NULL)
	win_perror("_beginthread() failed.");
    }

  if (hAnnounceThread)
    puts("AnnounceEngine started.");
  else
    fputs("AnnounceEngine not started.\r\n", stderr);

  puts("RepeaterEngine is starting up...");

  hRepeaterThread = (HANDLE) _beginthread(RepeaterEngine, 0, NULL);
  if ((LONG) hRepeaterThread == -1)
    hRepeaterThread = NULL;

  if (hRepeaterThread == NULL)
    win_perror("_beginthread() failed.");

  if (hAnnounceThread)
    puts("RepeaterEngine started.");
  else
    fputs("RepeaterEngine not started.\r\n", stderr);

  return 0;
}

// PrepareCommPort() function. Initialization code for COMM ports.
inline bool
PrepareCommPort()
{
  // Check if really a COMM port
  if (GetFileType(hComm) != FILE_TYPE_CHAR)
    {
      fputs("Specified device is not a COM port.\r\n", stderr);
      return false;
    }
  puts("Port opened OK.");

  // This is a correction. We can't wait for CTS change if XonLim or XoffLim
  // has illegal values.
  WDCB dcb;
  if (!dcb.Get(hComm))
    {
      win_perror("GetCommState() failed (valid COM port?)");
      return false;
    }
  dcb.XonLim = 2048;		// This should be better
  dcb.XoffLim = 512;
  if (!dcb.Set(hComm))
    {
      win_perror("SetCommState() failed (valid COM port?)");
      return false;
    }
  puts("New COM parameters set OK.");

  // We want to wait for CTS or RING change
  if (!SetCommMask(hComm, EV_CTS | EV_RING))
    {
      win_perror("SetCommMask() failed");
      return false;
    }
  puts("Monitoring CTS and RING pins.");

  ZeroMemory(&oLineWait, sizeof(oLineWait));
  oLineWait.hEvent = CreateEvent(NULL, false, false, NULL);
  if (oLineWait.hEvent == NULL)
    {
      win_perror("CreateEvent() failed");
      return false;
    }
  puts("Created event for CTS/RING monitoring.");

  return true;
}

// RepeaterEngine() is the repeater control thread. It handles RX events and
// plays repeater sounds.
void
RepeaterEngine(void *)
{
  try
  {
    SetTX(LR_TX_OFF);		// Begin all with TX off

    // Main repeater loop
    for (;;)
      {
	// Start waiting (async) for CTS/RING
	StartWaitRXEvent();

	// Sync wait for CTS/RING and wait for timeouts
	DWORD dwMsecTimeout;
	if (bTailTimerEnabled)
	  {
	    dwMsecTimeout = LR_TAIL_TIMEOUT;
	    puts("Waiting for squelch open or tail timeout...");
	  }
	else if (bSquelchOpened)
	  {
	    dwMsecTimeout = LR_MAX_TALK_TIME;
	    puts("Waiting for squelch close or max talk time...");
	  }
	else
	  {
	    dwMsecTimeout = INFINITE;
	    puts("Waiting for squelch open...");
	  }
	DWORD dwWaitResult =
	  WaitForSingleObject(oLineWait.hEvent, dwMsecTimeout);

	if (dwWaitResult == WAIT_FAILED)
	  {
	    win_perror("WaitForSingleObject() failed");
	    throw true;
	  }
	puts("OK.");

	// Sleep
	Sleep(LR_DEBOUNCE_TIME);

	// Timeout handle. Shut down transmitter.
	if (dwWaitResult == WAIT_TIMEOUT)
	  {
	    // Stop tail timer
	    bTailTimerEnabled = false;

	    if (bSquelchOpened)	// If squelch opened, max talk time reached
	      {
		++plrs->dwLongSpeakerCount;
		++plrs->dwQSOCount;
		plrs->Flush();

		puts("Timout. Max talktime reached.");

		// Time for ID before shut down?
		if (GetTickCount() - dwLastIDTime > LR_ID_PERIOD)
		  {
		    // Play repeater ID
		    puts("Repeater ID playing.");
		    PlaySound(LR_ID_SOUND, NULL,
			      dwSndAliasFlag | SND_NODEFAULT | SND_SYNC |
			      SND_NOWAIT);
		    Sleep(LR_DEBOUNCE_TIME);
		  }

		puts("Playing long speaker timeout sound.");
		PlaySound(LR_LONG_SPEAKER_SOUND, NULL, dwSndAliasFlag |
			  SND_NODEFAULT | SND_SYNC);
		bSquelchOpened = false;
	      }
	    else if (dwLastIDTime <= 1)	// If opened but no ID played, 'gummitumme'
	      {
		++plrs->dwGummitummeCount;
		plrs->Flush();

		puts("Timout. No transmissions. Playing 'gummitumme' sound.");
		PlaySound(LR_GUMMITUMME_SOUND, NULL, dwSndAliasFlag |
			  SND_NODEFAULT | SND_SYNC);
	      }
	    else		// Normal tail timeout shutdown
	      {
		++plrs->dwQSOCount;
		plrs->Flush();

		puts("Tail timeout, shutting down.");

		// If long session, play announcements with ID
		if (GetTickCount() - dwOpenTime > LR_ID_PERIOD)
		  PlayAnnouncements();

		puts("Playing signoff sound.");
		PlaySound(LR_SIGNOFF_SOUND, NULL, dwSndAliasFlag |
			  SND_NODEFAULT | SND_SYNC);
	      }

	    // Calculate some statistics
	    GetLocalTime(&plrs->stLastClosedTime);
	    DWORD dwTicksOpen = GetTickCount() - dwOpenTime;
	    if (dwTicksOpen > plrs->dwLongestSessionTime)
	      {
		plrs->dwLongestSessionTime = GetTickCount() - dwOpenTime;
		plrs->stLongestSessionOpenTime = plrs->stLastOpenedTime;
		GetLocalTime(&plrs->stLongestSessionCloseTime);
	      }
	    if ((dwTicksOpen < plrs->dwShortestSessionTime) |
		!plrs->dwShortestSessionTime)
	      {
		plrs->dwShortestSessionTime = GetTickCount() - dwOpenTime;
		plrs->stShortestSessionOpenTime = plrs->stLastOpenedTime;
		GetLocalTime(&plrs->stShortestSessionCloseTime);
	      }
	    plrs->dwTotalTXTime += (dwTicksOpen + 500) / 1000;
	    plrs->Flush();

	    // Turn off transmitter
	    SetTX(LR_TX_OFF);

	    // Reset last ID time
	    dwLastIDTime = 0;
	    continue;
	  }

	// Comm event signalled, check if squelch opened or closed
	DWORD dwLineStatus = GetRX();

	if (dwLineStatus & MS_CTS_ON)	// If squelch opened
	  {
	    // Do nothing if we are double-catching a squelch open event
	    if (bSquelchOpened)
	      {
		puts("Squelch open ignored.");
		continue;
	      }

	    // Stop tail timer
	    bTailTimerEnabled = false;

	    puts("Squelch opened.");

	    // If repeater is not open, check for tone call
	    if (dwLastIDTime == 0)
	      {
#ifdef LR_CHECK_TONE_CALL
		// We may call a wait function without timeout, make
		// sure transmitter is off.
		SetTX(LR_TX_OFF);

		dwLineStatus = GetRX();	// Update RX status variable

		puts("Checking for tone call.");
		if (~dwLineStatus & MS_RING_ON)	// If Ring Indicator off
		  {
		    // Start waiting (async) for RING
		    StartWaitRXEvent();

		    // Sync wait for CTS and wait for tail timeouts
		    fputs("Waiting for tone call...", stdout);
		    dwWaitResult =
		      WaitForSingleObject(oLineWait.hEvent, INFINITE);

		    if (dwWaitResult == WAIT_FAILED)
		      {
			win_perror("\n\rWaitForSingleObject() failed");
			throw true;
		      }
		    puts(" Trigged.");

		    // Sleep
		    Sleep(LR_DEBOUNCE_TIME);
		  }

		dwLineStatus = GetRX();	// Refresh pin states

		// if squelch closed or no tone call, cancel
		if (~dwLineStatus & (MS_CTS_ON | MS_RING_ON))
		  {
		    puts("Invalid tone call.");
		    continue;
		  }

		// Check time to calculate tone call time
		DWORD dwToneCallBeginTime = GetTickCount();

		// Start waiting (async) for RING
		StartWaitRXEvent();

		// Sync wait for CTS and wait for tail timeouts
		puts("Waiting for tone call end...");
		dwWaitResult = WaitForSingleObject(oLineWait.hEvent,
						   LR_TONECALL_TIMEOUT);

		switch (dwWaitResult)
		  {
		  case WAIT_FAILED:
		    win_perror("\n\rWaitForSingleObject() failed");
		    throw true;
		  case WAIT_TIMEOUT:
		    puts(" Timeout.");
		    continue;
		  }

		if (GetTickCount() - dwToneCallBeginTime >
		    LR_MIN_TONECALL_DUR)
		  puts(" Tone call detected.");
		else
		  {
		    puts(" Too short tone call.");
		    continue;
		  }
#else // No tone call check
		dwLastIDTime = 1;	// No gummitumme checks
#endif // LR_CHECK_TONE_CALL

		// Mark some statistics
		GetLocalTime(&plrs->stLastOpenedTime);
		dwOpenTime = GetTickCount();
		plrs->Flush();

		// Sleep
		Sleep(LR_DEBOUNCE_TIME);
	      }

	    bSquelchOpened = true;

	    SetTX(LR_TX_ON);	// Turn on transmitter

#ifdef LR_SQL_OPEN_CANCEL_SOUND
	    // Stop any playing sounds
	    PlaySound(NULL, NULL, 0);
	    puts("Wave output cancelled.");
#endif // LR_SQL_OPEN_CANCEL_SOUND
	  }
	else			// If squelch closed
	  {
	    // No courtesy sound or ID if already catched squelch close
	    if (!bSquelchOpened)
	      {
		puts("Squelch close ignored.");
		continue;
	      }

	    // Enable tail timer
	    bTailTimerEnabled = true;

	    bSquelchOpened = false;
	    puts("Squelch closed.");

	    // Make sure transmitter is on
	    SetTX(LR_TX_ON);

	    // Check if time to identify
	    if (dwLastIDTime == 0)
	      {
		// Play start tune
		dwLastIDTime = 1;
		PlaySound(LR_STARTUP_SOUND, NULL,
			  dwSndAliasFlag | SND_NODEFAULT | SND_ASYNC |
			  SND_NOWAIT);
		puts("Repeater start sound playing.");
	      }
	    else if (GetTickCount() - dwLastIDTime > LR_ID_PERIOD)
	      {
		// Play repeater ID
		dwLastIDTime = GetTickCount();
		PlaySound(LR_ID_SOUND, NULL,
			  dwSndAliasFlag | SND_NODEFAULT | SND_ASYNC |
			  SND_NOWAIT);
		puts("Repeater ID playing.");
	      }
	    else
	      {
		// Play courtesy sound
		PlaySound(LR_COURTESY_SOUND, NULL,
			  dwSndAliasFlag | SND_NODEFAULT | SND_ASYNC |
			  SND_NOWAIT);
		puts("Courtesy sound playing.");
	      }
	  }
      }
  }
  catch(bool bError)
  {
    if (bError)
      MessageBox(NULL, "Unexpected error in RepeaterEngine thread.\n\r"
		 "RepeaterEngine is stopping.", "LightRepeater Engine",
		 MB_ICONSTOP |
		 (WinVer_Major < 4 ? MB_SERVICE_NOTIFICATION_NT3X :
		  MB_SERVICE_NOTIFICATION));
  }
  puts("RepeaterEngine stopping.");
  SetTX(LR_TX_OFF);
}

// GetRX() function. Reads status of CTS and RING pins on the COMM port.
// If compiled with inverted logic, this function inverts pins to get a
// true=highlevel, false=lowlevel logic.
inline DWORD
GetRX()
{
  DWORD dwLineStatus;
  if (!GetCommModemStatus(hComm, &dwLineStatus))
    {
      win_perror("GetCommModemStatus() failed");
      throw true;
    }
#ifdef LR_INVERT_SQUELCH
  dwLineStatus ^= MS_CTS_ON;
#endif
#ifdef LR_INVERT_TONE_CALL
  dwLineStatus ^= MS_RING_ON;
#endif
  return dwLineStatus;
}

// SetTX() function. Changes state of outdata pins on the COMM port. Used with
// the parameters LR_TX_ON or LR_TX_OFF to set transmitter on or off.
inline void
SetTX(LR_TX_CTL TXCtl)
{
  if (TXCtl == LR_TX_OFF)
    Sleep(LR_DEBOUNCE_TIME);

  if (!EscapeCommFunction(hComm, TXCtl))
    {
      win_perror("EscapeCommFunction() failed");
      throw true;
    }
  fputs("Transmitter is ", stdout);
  if (TXCtl == LR_TX_ON)
    puts("on.");
  else
    puts("off.");
}

// StartWaitRXEvent() starts asynchronous wait for indata pin state changes.
// Used to enable wait for squelch or tone call. The caller can then use a
// wait function on the hEvent object of the OVERLAPPED class oLineWait object.
void
StartWaitRXEvent()
{
  // The result variable must survive after function returns, therefore static
  static DWORD dwLineEvent;
  if (WaitCommEvent(hComm, &dwLineEvent, &oLineWait))
    {
      puts("WaitCommEvent() completed.");
      return;
    }

  switch (win_errno)
    {
    case ERROR_IO_PENDING:
      puts("Started WaitCommEvent() async.");
      return;
    case ERROR_INVALID_PARAMETER:
      puts("WaitCommEvent() already started.");
      return;
    }

  win_perror("WaitCommEvent() failed");
  throw true;
}
