/************************************************************
 //
 //	Program: first.c (kapitel 3,exempel 1)
 //
 //	Beskrivning:
 //	 Ett första, minimalt Windows program. Visar ett
 //  tomt fönster utan funktionalitet förutom generell
 //	 hantering av fönstret.
 //
 //	Plattformar: Win16, Win32
 //
 //	(c) Studentlitteratur, Hans-Erik Eriksson
 //	från boken "Programutveckling för Windows och Windows NT"
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

// Fönsterprocedur för huvudfönster
LRESULT CALLBACK
WindowProc(HWND hWnd,		// Fönster handtag
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

// Registrering av fönsterklass
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
  wndclass.lpfnWndProc = WindowProc;	// Fönsterprocedur
  wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);	// Musmarkör
  wndclass.hIcon = LoadIcon(hInstance, "IDI_FIRSTICON");	// Ikon
  wndclass.lpszMenuName = NULL;	// Meny
  wndclass.hbrBackground = (HBRUSH) hBg;	// Bakgrundsfärg
  wndclass.style = 0L;		// Fönsterklass stil
  wndclass.cbClsExtra = 0;	// Extra bytes för klass
  wndclass.cbWndExtra = 0;	// Extra bytes för fönster

  // Registrera klass
  RegisterClass(&wndclass);
}

// EXE-filens startkod - registrera fönsterklass, skapa fönster och
// initiera meddelande loop
int
startup()
{
  HWND hWnd;
  MSG msg;
  STARTUPINFO si;
  GetStartupInfo(&si);

  // Registrera fönsterklass
  DoRegisterClasses();

  // Skapa fönster
  hWnd = CreateWindow("APPLIKATION",	// Klassnamn
		      "Programutveckling för Windows och Windows NT",	// Fönster titel
		      WS_OVERLAPPEDWINDOW,	// Fönster stil
		      CW_USEDEFAULT,	// X-punkt övre vänstra hörn
		      CW_USEDEFAULT,	// Y-punkt övre vänstra hörn
		      CW_USEDEFAULT,	// cx (bredd)
		      CW_USEDEFAULT,	// cy (höjd)
		      NULL,	// Förälderfönster
		      NULL,	// Meny
		      GetModuleHandle(NULL),	// Programinstans
		      NULL);	// Data pekare

  if (!hWnd)
    ExitProcess(GetLastError());

  // Visa fönster
  ShowWindow(hWnd, si.wShowWindow);

  // Meddelandeloop som hämtar meddelanden
  while (GetMessage(&msg, 0, 0, 0) > 0)
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

  ExitProcess(0);
}
