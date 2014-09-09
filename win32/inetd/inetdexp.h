// Declarations for exports in INETD.EXE module.
// This version of inetdexp.h complies to INETD Service Version 1.10-1.20.
#ifndef _INETDEXP_H
#define _INETDEXP_H

//---- Compile this version for Windows NT 4.00.1398, Windows 98 4.10.1998 ----
#define WINVER          0x0410
#define _WIN32_WINDOWS  0x0410
#define _WIN32_WINNT    0x0400

#define INETD_USE_WINSOCK_VERSION     0x0202
#define INETD_PRIMARY_WINSOCK_DLL     "WS2_32.DLL"

#include <winstrct.h>
#include <wsync.h>

#if WINVER != 0x0410
# error WINVER changed.
#endif
#if _WIN32_WINDOWS != 0x0410
# error WINVER changed.
#endif
#if _WIN32_WINNT != 0x0400
# error WINVER changed.
#endif
//-----------------------------------------------------------------------------

#define INETD_NEW_THREAD_SLEEP_TIME   20
#define INETD_SERVICE_STOP_TIMEOUT    10000

/// External functions invoked from command line with character "!".
typedef unsigned long (WINAPI LibCommandFunc)(LPCSTR lpArg);

/// External functions handling incoming connections.
typedef unsigned long (WINAPI LibHandlerFunc)(HANDLE hConn, LPCSTR lpArg);

/**
   External interfaces can attach a window to receive messages when INETD
   service handlers are created and deleted, global flags are changed, INETD
   is about to shutdown and other events.
   When the external interface creates an attach object to receive messages,
   it creates an INETD Change Notification object using
   CreatexxxChangeNotification() with apropriate window message information.
   When the window object is destroyed, the Change Notification object should
   be deleted using the corresponding DeletexxxChangeNotification() function.
   */

/**
   Function to create a notification object that sends a WM_QUIT
   message to the window thread when INETD is about to terminate.

   @param hWnd Handle to a window of the thread the WM_QUIT message will be
      sent to.
   @param dwExitCode The exit code sent as wParam along with the WM_QUIT
      message.
   */
EXTERN_C EXE_EXTERN HANDLE WINAPI CreateQuitNotification(HWND hWnd,
   DWORD dwExitCode);

/**
   Posts WM_QUIT messages to all window threads attached using the
   CreateQuitNotification() function.
   */
EXTERN_C EXE_EXTERN bool WINAPI PostQuitNotifications();

/**
   Deletes a notification created with the CreateQuitNotification() function so
   that no WM_QUIT message is sent to the window thread.
   */
EXTERN_C EXE_EXTERN void WINAPI DeleteQuitNotification(
   HANDLE hQuitNotification);

/**
   Function to create a notification object that sends a custom window message
   to a window when a service handler is created or deleted.

   The parameters specifies the window message to send.
   */
EXTERN_C EXE_EXTERN HANDLE WINAPI CreateServiceChangeNotification(HWND hWnd,
   UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
   Function to notify windows attached using the
   CreateServiceChangeNotification() function that a service handler is
   created or deleted.

   This function is called automatically when a service is created or deleted.
   */
EXTERN_C EXE_EXTERN bool WINAPI PostServiceChangeNotifications();

/**
   Deletes a notification object created by the
   CreateServiceChangeNotification() function. The window will no longer be
   notified when a service handler is created or deleted.
   */
EXTERN_C EXE_EXTERN void WINAPI DeleteServiceChangeNotification(
   HANDLE hServiceChangeNotification);

/**
   Function to create a notification object that sends a custom window message
   to a window when the value of a global INETD flag is changed. Flags are used
   to specify socket options for created or connected sockets. The SetSockOpts()
   function can be used to set options on a socket according to the global
   flags.

   The parameters specifies the window message to send.
   */
EXTERN_C EXE_EXTERN HANDLE WINAPI CreateFlagChangeNotification(HWND hWnd,
   UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
   Function to notify windows attached using the
   CreateFlagChangeNotification() function that the value of a global flag has
   changed.

   The function must be manually called after flag values have been changed.
   */
EXTERN_C EXE_EXTERN bool WINAPI PostFlagChangeNotifications();

/**
   Deletes a notification object created by the
   CreateFlagChangeNotification() function. The window will no longer be
   notified when a global flag has been changed.
   */
EXTERN_C EXE_EXTERN void WINAPI DeleteFlagChangeNotification(
   HANDLE hFlagChangeNotification);

/**
   ServicePort is the base class for service objects.
   External protocol drivers should derive a class called xxxServicePort, where
   xxx is the name of the service. This name is used by the CREATE command in
   INETD. The library must then export CreateSocketAndListen() and
   ServiceThread() methods in the derived class. Note that the derived classes
   must never contain data members, constructors or destructors, because the
   object actually created by INETD is of the size of the base class
   ServicePort.
   */
/// Base class for service objects.
class ServicePort
{
   /// First loaded ServicePort object, or NULL
   static ServicePort *first;
   /// Next loaded ServicePort object, or NULL
   ServicePort *next;

   /// Handle to listening thread.
   HANDLE hServiceThread;

   /// Adds or deletes from linked list.
   void UpdateChain(bool bAddFlag);

   /**
      Function to workaround the problem that C++ doesn't accept starting a
      member function as a thread function
      */
   /// Creates the listening service thread.
   HANDLE CreateServiceThread(DWORD dwMaxConn);

   /// Dispatcher objects initializes using a ServicePort object.
   friend class DispatchByCmd;
   /// Dispatcher objects initializes using a ServicePort object.
   friend class DispatchByFunc;

   protected:
#ifdef __DLL__
   struct   // Bug in Borland compiler.
   {        // Data members static in imported classes if not inside a struct
#endif
   /// Handle to library with ServicePort object
   HINSTANCE hProtocolDriver;

   /**
      If object is constructed with dwMaxConn > 1 a semaphore is created to
      keep count of concurrent connections to the service handler. If a
      semaphore is created, the dispatcher creates new threads for each call to
      HandleConnection(). If no semaphore is created, the connection handler
      will run in the same thread as the listening service, which makes it
      handle only one connection at a time. The hSemaphore member is NULL if
      no semaphore has been created.
      */
   /// Handle to connection counting semaphore.
   HANDLE hSemaphore;

   /// User defined tag. Ignored by INETD.
   void *Tag;

   /**
      Socket (or other handle) listening for incoming calls.
      Should be opened and initialized by the CreateSocketAndListen() method.
      The handle is closed using CloseHandle() by the destructor. A protocol
      driver can set this member to INVALID_HANDLE_VALUE to prevent INETD from
      closing it using a call to CloseHandle().
      */
   /// Listening handle.
   union
   {
      HANDLE hServer;
      SOCKET sServer;
   };

   /// Name of service, i.e. pop3 or telnet.
   LPSTR szService;
   /// Protocol of service, i.e. tcp or udp.
   LPSTR szProtocol;

   /// External program file (with parameters) to run on incoming connections.
   LPSTR szDispatchCommand;
   /// Handle to library (DLL) with handler function.
   HINSTANCE hHandlerLib;
   /// Address of handler function in loaded DLL (hHandlerLib).
   LibHandlerFunc *pDispatchFunc;
   /// Null terminated arguments passed to pDispatchFunc.
   LPSTR szFuncParam;

   /**
      Event object set if service is stopping. This is to notify service thread
      that the service should stop listening for incoming connections.
      */
   /// Event object set if service is stopping.
   HANDLE hStopEvent;
#ifdef __DLL__
   };
#endif

   /**
      Run by service handler to dispatch an incoming connection. If successful,
      the function returns a thread handle, if an error occures the return value
      is INVALID_HANDLE_VALUE.

      @param hConn Handle to the accepted socket or other connection.
     */
   /// Function called by service thread when a connection is accepted.
   void HandleConnection(HANDLE hConn) const;

   /**
      This function provides a multithread safe and CRT memory management safe
      way to change strings in a ServicePort object. It creates storage for the
      new string, assigns a pointer to the new storage to the string variable,
      and destroys the old string's storage, if it existed.

      @param pszStringMember Points out a string member of a ServicePort object.
      @param szNewString New string to copy to the string member.
      */
   /// Function to replace a string member of a ServicePort object.
   static bool ReplaceString(LPSTR *pszStringMember, LPCSTR szNewString);

   public:
   /**
      Returns address to the first ServicePort object in the linked list.
      Returns NULL if no ServicePort objects are installed.
      */
   static ServicePort *GetFirst();

   /**
      Returns address to the ServicePort object following this object in the
      linked list. Returns NULL if this is the last object in chain.
      */
   ServicePort *GetNext() const
   {
      return next;
   }

   /**
      Checks if ServicePort object is installed in chain. Returns the pointer
      (servicehandle) if it was found or NULL if it was not found.
      */
   /// Checks if ServicePort object is installed in chain.
   ServicePort *Installed() const;

   /**
      Checks if the Service thread associated with this ServicePort object is
      running or has terminated. If the thread has terminated, the object is
      deleted and false is returned. If the thread is running, nothing is done
      and the function returns true.
      */
   /// Verifies that the service thread is running.
   bool Verify();

   /**
      Function to destroy all ServicePort objects. Used to stop all service
      handlers running in INETD.
      */
   /// Delete all INETD services.
   static void DeleteAll();

   /**
      Finds ServicePort object by protcol and service names. NULL parameter
      finds any object, which makes Find(NULL, NULL) equivalent to GetFirst().
      */
   /// Finds ServicePort object.
   static ServicePort *Find(LPCSTR szProt, LPCSTR szServ)
   {
      return GetFirst()->FindNext(szProt, szServ);
   }

   /**
      Like Find() method, but starts search at this object in the chain.
      */
   /// Finds more ServicePort objects.
   ServicePort *FindNext(LPCSTR szProt, LPCSTR szServ);

   /**
      Creates a character string array with information lines about this
      ServicePort object. Each string is terminated by a NULL character, and the
      last string is terminated by two NULL characters. Use FreeStrings() method
      to free the returned string buffer when it is no longer needed.
      */
   /// Gets information about this ServicePort object.
   LPSTR * MakeInfoStrings() const;

   /**
      Creates a character string array object with a list of currently existing
      ServicePort objects. Each string is terminated by a NULL character, and
      the last string is terminated by two NULL characters. Use FreeStrings()
      method to free the returned string buffer when it is no longer needed.
      */
   /// Gets list of all ServicePort objects.
   static LPSTR * MakeListStrings();

   /**
      Used to free string arrays allocated by MakeInfoStrings() and
      MakeListStrings() functions.
      */
   /// Free buffers allocated by MakeInfoStrings() and MakeListStrings().
   static void FreeStrings(LPSTR *pBuf);

   /**
      Suspends service thread. Called by NT service Handler() function in
      response to service pause request.
      */
   /// Suspends service thread.
   DWORD Suspend() const
   {
      if (hServiceThread != INVALID_HANDLE_VALUE)
         return SuspendThread(hServiceThread);
      else
         return -1;
   }

   /**
      Resume service thread. Called by NT service Handler() function in
      response to service continue request.
      */
   /// Resumes suspended service thread.
   DWORD Resume() const
   {
      if (hServiceThread != INVALID_HANDLE_VALUE)
         return ResumeThread(hServiceThread);
      else
         return -1;
   }

   /**
      Constructor for services where an external program file is executed in
      response to a connection.

      @param szServ Name of service, i.e. pop3 or telnet.
      @param szProt Name of protocol. May be on the form <driverfile>protocol
         if the driver for the protocol is to be loaded. If no driverfile is
         specified, the protocol driver is assumed to be statically linked in
         INETD.EXE.
      @param szCom Command line to execute (calling CreateProcess() function)
         in response to a accepted connection.
      @param dwMaxConn Max number of connections to be handled concurrently. If
         this value is greater than 1, a semaphore is created and the service
         thread should wait for the semaphore before accepting a connection.
         The semaphore is released each time a connection is closed.
      */
   /// Constructor for command line handled services.
   ServicePort(LPCSTR szServ, LPCSTR szProt, LPCSTR szCom, DWORD dwMaxConn);

   /**
      Constructor for services where a function in a DLL file is called in
      response to a connection.

      @param szServ Name of service, i.e. pop3 or telnet.
      @param szProt Name of protocol. May be on the form <driverfile>protocol
         if the driver for the protocol is to be loaded. If no driverfile is
         specified, the protocol driver is assumed to be statically linked in
         INETD.EXE.
      @param szMod Name and optionally path to the DLL file where the handler
         function is exported.
      @param szFunc Name of the function exported from the DLL file.
      @param szArg Argument string to pass to the DLL function as second
         parameter each time the function is called.
      @param dwMaxConn Max number of connections to be handled concurrently. If
         this value is greater than 1, a semaphore is created and the service
         thread should wait for the semaphore before accepting a connection.
         The semaphore is released each time a connection is closed.
      */
   /// Constructor for DLL function handled services.
   ServicePort(LPCSTR szServ, LPCSTR szProt, LPCSTR szMod, LPCSTR szFunc,
      LPSTR szArg, DWORD dwMaxConn);

   /**
      General destructor for all ServicePort derived objects. The object is
      deleted from the linked list of ServicePort objects.
      */
   ~ServicePort();

   /*
      To make the new and delete operators work across different modules and
      different runtime libraries, objects storage are allocated in a call
      to GlobalAlloc(). No exception is thrown if the allocation fails. This
      case is instead handled by the constructor.
      */
   void *operator new(size_t size)
   {
      return GlobalAlloc(GPTR, size);
   }

   /*
      To make the new and delete operators work across different modules and
      different runtime libraries, objects storage are deallocated in a call
      to GlobalFree(). A NULL pointer check is made before calling GlobalFree().
      */
   void operator delete(void *__this)
   {
      if (__this)
         GlobalFree(__this);
   }
};

/// Sets socket options on specified socket using global socket option flags.
EXTERN_C EXE_EXTERN void WINAPI SetSockOpts(SOCKET s);

/**
   Function to interprete switches when INETD is started from command line or as
   a Windows NT service. The function sets the global flags indicated by the
   command line switches passed to this function.

   @param pargc Pointer to the argument count parameter.
   @param pargv Pointer to the argument vector parameter.
   */
/// Sets global flags according to command line switches.
EXTERN_C EXE_EXTERN void WINAPI ProcessSwitches(int *pargc, char ***pargv);

/**
   Processes an INETD command line. This function is used by the console command
   line interface, and for processing script files. It may also be used by
   external remote control functions to run a command as if done at the INETD
   command prompt.

   Any return messages are stored in the buffer when the function returns.
   The second parameter must specify max number of character that can be
   copied into the buffer.

   @param szCmdLine The command line to execute.
   @param dwSize Max size of the buffer pointed to by pcCmdLine. This buffer is
      used to store output data from the command.
   */
/// Processes a command in the INETD command line user interface.
EXTERN_C EXE_EXTERN bool WINAPI ProcessCommand(LPSTR szCmdLine, DWORD dwSize);

/**
   Function to start the command line interface.

   @param lpRunAutoexecFlag Set this parameter to NULL to avoid running the
      autoexec.inetd or inetd.conf files.
   */
/// Starts the INETD command line user interface.
EXTERN_C EXE_EXTERN void _USERENTRY RunCLI(HANDLE lpRunAutoexecFlag);

/// Installs INETD as a Windows NT service.
EXTERN_C EXE_EXTERN int WINAPI InstallService(int argc, char **argv);
/// Uninstalls INETD as a Windows NT service.
EXTERN_C EXE_EXTERN int WINAPI UninstallService(int argc, char **argv);

/// true if the SO_DONTLINGER option should be set on sockets.
extern EXE_EXTERN bool CDECL bSockOptDontLinger;
/// true if the SO_KEEPALIVE option should be set on sockets.
extern EXE_EXTERN bool CDECL bSockOptKeepAlive;
/// true if the SO_REUSEADDR option should be set on sockets.
extern EXE_EXTERN bool CDECL bSockOptReuseAddr;
/// true if the TCP_NODELAY option should be set on sockets.
extern EXE_EXTERN bool CDECL bSockOptTCPNoDelay;

/// Handle to console window, or NULL if startup code could not find a window.
extern EXE_EXTERN HWND hwndConsole;

/// Makes sure only one command line interface is started.
extern EXE_EXTERN HANDLE CDECL hCLIMutex;

/// SERVICE_STATUS structure object for running service.
extern EXE_EXTERN SERVICE_STATUS NTServiceStatus;
/// SERVICE_STATUS_HANDLE referring to running service.
extern EXE_EXTERN SERVICE_STATUS_HANDLE CDECL NTServiceStatusHandle;

#endif /*_INETDEXP_H*/

