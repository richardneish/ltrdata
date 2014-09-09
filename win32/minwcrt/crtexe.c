#include "crtintrn.h"
#include "crtlib.h"
#include "sect_attribs.h"
#include "rterr.h"
#include "crtintrn.h"

#pragma comment(linker, "/nodefaultlib:msvcrt.lib")

#ifdef DEBUG_CRTEXE_C
#ifdef _WINMAIN_
#define dbgmsg(msg) MessageBox(NULL,msg,"",0)
#pragma comment(lib, "user32")
#else
#define dbgmsg(msg) puts(msg)
#endif
#else
#define dbgmsg(msg)
#endif

#ifdef WPRFLAG
#include <windows.h>
#include <shellapi.h>
#define _T(t) L##t
typedef unsigned short _TUCHAR;
typedef signed short _TSCHAR;
#else
#define _T(t) t
typedef unsigned char _TUCHAR;
typedef signed char _TSCHAR;
#endif

#if defined(_WINMAIN_) && defined(WPRFLAG)
int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int);
#endif

#define SPACECHAR   _T(' ')
#define DQUOTECHAR  _T('\"')

#ifdef WPRFLAG
extern wchar_t ** __wargv;  /* pointer to table of wide cmd line args */
extern wchar_t * _wenviron[MAX_WENVIRON_COUNT]; /* pointer to wide environ */
extern wchar_t * _wpgmptr;  /* points to the module (EXE) wide name */
extern wchar_t **__winitenv;
#else
extern char **__initenv;
#endif

extern int _fmode;
extern int _commode;

/*
 * pointers to initialization sections
 */
//extern _CRTALLOC(".CRT$XIA") _PVFV __xi_a[];
//extern _CRTALLOC(".CRT$XIZ") _PVFV __xi_z[];    /* C initializers */
//extern _CRTALLOC(".CRT$XCA") _PVFV __xc_a[];
//extern _CRTALLOC(".CRT$XCZ") _PVFV __xc_z[];    /* C++ initializers */

/*
 * Pointers to beginning and end of the table of function pointers manipulated
 * by _onexit()/atexit().  The atexit/_onexit code is shared for both EXE's and
 * DLL's but different behavior is required.  These values are set to -1 to
 * mark this module as an EXE.
 */

_PVFV *__onexitbegin;
_PVFV *__onexitend;

#define _IMP___FMODE    _fmode_dll
#define _IMP___COMMODE  _commode_dll

/*
 * Declare/define communal that serves as indicator the default matherr
 * routine is being used.
 */
//int __defaultmatherr;

/*
 * routine in DLL to do initialization (in this case, C++ constructors)
 */
extern void __cdecl _initterm(_PVFV *, _PVFV *);

#ifdef _WINMAIN_

#ifdef WPRFLAG
int wWinMainCRTStartup(
#else  /* WPRFLAG */
int WinMainCRTStartup(
#endif  /* WPRFLAG */

#else  /* _WINMAIN_ */

#ifdef WPRFLAG
int wmainCRTStartup(
#else  /* WPRFLAG */
int mainCRTStartup(
#endif  /* WPRFLAG */

#endif  /* _WINMAIN_ */
        void
        )
{
        int argc = 0;   /* three standard arguments to main */
        _TSCHAR **argv = NULL;
        _TSCHAR **envp = NULL;

        int argret = 0;

#ifdef _WINMAIN_
        _TUCHAR *lpszCommandLine;
        STARTUPINFO StartupInfo;
#endif  /* _WINMAIN_ */

#ifdef WPRFLAG
	int envp_count = 0;
#endif

        //_startupinfo    startinfo;

        /*
         * Determine if this is a managed application
         */
        //managedapp = check_managed_app();

	/*
	 * Set __app_type properly
	 */
#ifdef _WINMAIN_
	//__set_app_type(_GUI_APP);
#else  /* _WINMAIN_ */
	//__set_app_type(_CONSOLE_APP);
#endif  /* _WINMAIN_ */

#ifdef _WINMAIN_
#ifdef WPRFLAG
	dbgmsg("wWinMainCRTStartup()");
#else
	dbgmsg("WinMainCRTStartup()");
#endif
#else
#ifdef WPRFLAG
	dbgmsg("wmainCRTStartup()");
#else
	dbgmsg("mainCRTStartup()");
#endif
#endif
	/*
	 * Mark this module as an EXE file so that atexit/_onexit
	 * will do the right thing when called, including for C++
	 * d-tors.
	 */
	__onexitbegin = __onexitend = (_PVFV *)(-1);
	    
	dbgmsg("OK1b");

	/*
	 * Propogate the _fmode and _commode variables to the DLL
	 * (crash in crtdll.dll...?)
	 */
	//*_IMP___FMODE = _fmode;
	dbgmsg("OK1c");
	//*_IMP___COMMODE = _commode;
	dbgmsg("OK1d");
	
#ifdef _M_IX86
	/*
	 * Set the local copy of the Pentium FDIV adjustment flag
	 * (not supported by crtdll.dll)
	 */
	
	//_adjust_fdiv = * _imp___adjust_fdiv;
#endif  /* _M_IX86 */
	
	dbgmsg("OK2");

	/*
	 * Run the RTC initialization code for this DLL
	 */
#ifdef _RTC
	_RTC_Initialize();
#endif  /* _RTC */
	
	/*
	 * Call _setargv(), which will trigger a call to __setargv() if
	 * SETARGV.OBJ is linked with the EXE.  If SETARGV.OBJ is not
	 * linked with the EXE, a dummy _setargv() will be called.
	 */
#ifdef WPRFLAG
	//_wsetargv();
#else  /* WPRFLAG */
	//_setargv();
#endif  /* WPRFLAG */

	/*
	 * If the user has supplied a _matherr routine then set
	 * __pusermatherr to point to it.
	 */
	//            if ( !__defaultmatherr )
	//    __setusermatherr(_matherr);
	
#ifdef _M_IX86
	//_setdefaultprecision();
#endif  /* _M_IX86 */

	/*
	 * Do runtime startup initializers.
	 *
	 * Note: the only possible entry we'll be executing here is for
	 * __lconv_init, pulled in from charmax.obj only if the EXE was
	 * compiled with -J.  All other .CRT$XI* initializers are only
	 * run as part of the CRT itself, and so for the CRT DLL model
	 * are not found in the EXE.  For that reason, we call _initterm,
	 * not _initterm_e, because __lconv_init will never return failure,
	 * and _initterm_e is not exported from the CRT DLL.
	 *
	 * Note further that, when using the CRT DLL, executing the
	 * .CRT$XI* initializers is only done for an EXE, not for a DLL
	 * using the CRT DLL.  That is to make sure the -J setting for
	 * the EXE is not overriden by that of any DLL.
	 */
	//_initterm( __xi_a, __xi_z );

#ifdef _RTC
	atexit(_RTC_Terminate);
#endif  /* _RTC */

	/*
	 * Get the arguments for the call to main. Note this must be
	 * done explicitly, rather than as part of the dll's
	 * initialization, to implement optional expansion of wild
	 * card chars in filename args
	 */

	dbgmsg("OK3");

	//startinfo.newmode = 0;

#ifdef WPRFLAG
	_wcmdln = GetCommandLineW();
	argv = (_TSCHAR**)CommandLineToArgvW(_wcmdln, &argc);
	if (argv == NULL)
	  FatalAppExitA(0, "This application requires Windows NT.");

	_wpgmptr = (wchar_t*)argv[0];
	__wargv = (wchar_t**)argv;
	__argc = argc;
	_wenviron[0] = GetEnvironmentStringsW();
	while ((_wenviron[envp_count][0] != 0) &
	       (envp_count < MAX_WENVIRON_COUNT))
	  {
	    _wenviron[envp_count+1] = _wenviron[envp_count]+
	      wcslen(_wenviron[envp_count])+1;
	    envp_count++;
	  }
	
	if ((argv == NULL) | (envp_count >= MAX_WENVIRON_COUNT))
	  argret = -1;

	if (argret < 0)
	  _amsg_exit(_RT_SPACEARG);

	_wenviron[envp_count] = NULL;
	envp = (_TSCHAR**)_wenviron;

#else  /* WPRFLAG */
	argret = __GetMainArgs(&argc, &argv, &envp,
			       0, NULL /*&startinfo*/);

	if (argv == NULL)
	  _amsg_exit(_RT_SPACEARG);

	__argc = argc;
	__argv = argv;
	_environ = envp;
	_pgmptr = argv[0];
#endif
	dbgmsg("OK4");

	/*
	 * do C++ constructors (initializers) specific to this EXE
	 */
	//_initterm( __xc_a, __xc_z );
	
#ifdef _WINMAIN_
	/*
	 * Skip past program name (first token in command line).
	 * Check for and handle quoted program name.
	 */
#ifdef WPRFLAG
	/* OS may not support "W" flavors */

	if (_wcmdln == NULL)
	  FatalAppExitA(0, "This application requires Windows NT.");

	lpszCommandLine = (wchar_t *)_wcmdln;

#else  /* WPRFLAG */
	lpszCommandLine = (unsigned char *)_acmdln;
#endif  /* WPRFLAG */
	
	if ( *lpszCommandLine == DQUOTECHAR ) {
	  /*
	   * Scan, and skip over, subsequent characters until
	   * another double-quote or a null is encountered.
	   */
	  while ( *++lpszCommandLine && (*lpszCommandLine
					 != DQUOTECHAR) );
	  /*
	   * If we stopped on a double-quote (usual case), skip
	   * over it.
	   */
	  if ( *lpszCommandLine == DQUOTECHAR )
	    lpszCommandLine++;
	}

	while (*lpszCommandLine > SPACECHAR)
	  lpszCommandLine++;

	if (*lpszCommandLine >= SPACECHAR)
	  lpszCommandLine++;

	dbgmsg("OK5");

	/*
	 * Skip past any white space preceeding the second token.
	 */
	while (*lpszCommandLine && (*lpszCommandLine <= SPACECHAR)) {
	  lpszCommandLine++;
	}

	StartupInfo.dwFlags = 0;
	GetStartupInfo( &StartupInfo );

#ifdef WPRFLAG
	    __winitenv = (wchar_t**)envp;
	    exit(wWinMain(
#else  /* WPRFLAG */
            __initenv = envp;
	    exit(WinMain(
#endif  /* WPRFLAG */
                       GetModuleHandleA(NULL),
                       NULL,
                       (LPTSTR)lpszCommandLine,
                       StartupInfo.dwFlags & STARTF_USESHOWWINDOW
                        ? StartupInfo.wShowWindow
                        : SW_SHOWDEFAULT
		       ));

#else  /* _WINMAIN_ */

#ifdef WPRFLAG
	    dbgmsg("OK6");
            exit(wmain(argc, (wchar_t**)argv, (wchar_t**)envp));
#else  /* WPRFLAG */
	    dbgmsg("OK6");
            exit(main(argc, argv, envp));
#endif  /* WPRFLAG */

#endif  /* _WINMAIN_ */
}
