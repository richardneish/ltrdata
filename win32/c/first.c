/************************************************************
 //
 //	Program: first.c (kapitel 3,exempel 1)
 //
 //	Beskrivning:
 //	 Ett f�rsta, minimalt Windows program. Visar ett
 //  tomt f�nster utan funktionalitet f�rutom generell
 //	 hantering av f�nstret.
 //
 //	Plattformar: Win16, Win32
 //
 //	(c) Studentlitteratur, Hans-Erik Eriksson
 //	fr�n boken "Programutveckling f�r Windows och Windows NT"
 //
 ************************************************************/

// Inkluderingsfiler
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#pragma comment(linker, "/subsystem:windows")
#pragma comment(linker, "/entry:startup")

HBRUSH hBg;

// F�nsterprocedur f�r huvudf�nster
LRESULT CALLBACK
WindowProc(HWND hWnd,		// F�nster handtag
	   UINT msg,		// Meddelande
	   WPARAM wParam,	// Parameter (w)
	   LPARAM lParam)	// Parameter (l)
{
  switch (msg)
    {
    case WM_DESTROY:
      {
	PostQuitMessage(0);
	return 0;
      }
    default:
      return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}

LPSTR errormsg()
{
  LPSTR lpBuf = NULL;
  if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
		    FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0,
		    (LPSTR) &lpBuf, 0, NULL))
    return lpBuf;
  else
    return NULL;
}

// Registrering av f�nsterklass
void
DoRegisterClasses()
{
  WNDCLASS wndclass;
  HINSTANCE hInstance = GetModuleHandle(NULL);
  HBITMAP hBitMap;

  hBitMap = LoadBitmap(hInstance, "IDB_BGBITMAP");
  if (hBitMap == NULL)
    {
      MessageBox(NULL, errormsg(), "LoadBitmap()", MB_ICONSTOP | MB_OK);
      return;
    }

  hBg = CreatePatternBrush(hBitMap);
  if (hBg == NULL)
    {
      MessageBox(NULL, errormsg(), "CreateBrushIndirect()",
		 MB_ICONSTOP | MB_OK);
      return;
    }

  wndclass.lpszClassName = "APPLIKATION";	// Klassens namn
  wndclass.hInstance = hInstance;	// Programinstans
  wndclass.lpfnWndProc = WindowProc;	// F�nsterprocedur
  wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);	// Musmark�r
  wndclass.hIcon = LoadIcon(hInstance, "IDI_FIRSTICON");	// Ikon
  wndclass.lpszMenuName = NULL;	// Meny
  wndclass.hbrBackground = (HBRUSH) hBg;	// Bakgrundsf�rg
  wndclass.style = 0L;		// F�nsterklass stil
  wndclass.cbClsExtra = 0;	// Extra bytes f�r klass
  wndclass.cbWndExtra = 0;	// Extra bytes f�r f�nster

  // Registrera klass
  RegisterClass(&wndclass);
}

// EXE-filens startkod - registrera f�nsterklass, skapa f�nster och
// initiera meddelande loop
int
startup()
{
  HWND hWnd;
  MSG msg;
  STARTUPINFO si;
  GetStartupInfo(&si);

  // Registrera f�nsterklass
  DoRegisterClasses();

  // Skapa f�nster
  hWnd = CreateWindow("APPLIKATION",	// Klassnamn
		      "Programutveckling f�r Windows och Windows NT",	// F�nster titel
		      WS_OVERLAPPEDWINDOW,	// F�nster stil
		      CW_USEDEFAULT,	// X-punkt �vre v�nstra h�rn
		      CW_USEDEFAULT,	// Y-punkt �vre v�nstra h�rn
		      CW_USEDEFAULT,	// cx (bredd)
		      CW_USEDEFAULT,	// cy (h�jd)
		      NULL,	// F�r�lderf�nster
		      NULL,	// Meny
		      GetModuleHandle(NULL),	// Programinstans
		      NULL);	// Data pekare

  if (!hWnd)
    ExitProcess(GetLastError());

  // Visa f�nster
  ShowWindow(hWnd, si.wShowWindow);

  // Meddelandeloop som h�mtar meddelanden
  while (GetMessage(&msg, 0, 0, 0) > 0)
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

  ExitProcess(0);
}
