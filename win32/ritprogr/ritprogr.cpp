#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>
#include <commdlg.h>

#include "ritprogr.rc.h"

#include "ritprogr.h"
#include "mikael.h"
#include "maliheh.h"

#pragma comment(lib, "kernel32")
#pragma comment(lib, "user32")
#pragma comment(lib, "gdi32")
#pragma comment(lib, "comdlg32")

#pragma comment(linker, "/subsystem:windows")
#pragma comment(linker, "/entry:startup")

COLORREF drawcolor = 0x007F00FF;
HMENU hMenu = NULL;
COLORREF CustColors[16];
LOGFONT textfont;
Tools tool = TOOL_LINE;
COLORREF crBGColor = 0x00FFFFFF;
HINSTANCE hInstance = NULL;
WORD xPos, yPos, xTarget, yTarget;
bool MouseButtonDown = false;

// Funktion f�r Verktyg-menyn
// Bockar f�r valt alternativ p� menyn och l�gger l�gger r�tt konstant i
// variabeln curtool.
void
ChooseTool(WORD curtool)
{
  CheckMenuItem(hMenu, mnuToolLine, MF_UNCHECKED);
  CheckMenuItem(hMenu, mnuToolCircle, MF_UNCHECKED);
  CheckMenuItem(hMenu, mnuToolRectangle, MF_UNCHECKED);
  CheckMenuItem(hMenu, mnuToolText, MF_UNCHECKED);
  CheckMenuItem(hMenu, mnuToolFreehand, MF_UNCHECKED);
  CheckMenuItem(hMenu, curtool, MF_CHECKED);

  switch (curtool)
    {
    case mnuToolLine:
      tool = TOOL_LINE;
      return;
    case mnuToolCircle:
      tool = TOOL_CIRCLE;
      return;
    case mnuToolRectangle:
      tool = TOOL_RECTANGLE;
      return;
    case mnuToolText:
      tool = TOOL_TEXT;
      return;
    case mnuToolFreehand:
      tool = TOOL_FREEHAND;
      return;
    }
}

// F�nsterprocedur f�r huvudf�nster
LRESULT CALLBACK
WindowProc(HWND hWnd,		// hWnd
	   UINT msg,		// Meddelande
	   WPARAM wParam,	// Parameter (w)
	   LPARAM lParam)	// Parameter (l)
{
  switch (msg)
    {
    case WM_PAINT:
      DoLoadScreen(hWnd);

      if (MouseButtonDown)
	switch (tool)
	  {
	  case TOOL_LINE:
	    DoDrawLines(hWnd, xPos, yPos, xTarget, yTarget);
	    break;

	  case TOOL_CIRCLE:
	    DoDrawEllipse(hWnd, xPos, yPos, xTarget, yTarget);
	    break;

	  case TOOL_RECTANGLE:
	    DoDrawRectangle(hWnd, xPos, yPos, xTarget, yTarget);
	    break;

	  case TOOL_TEXT:
	    DoTextTool(hWnd, xTarget, yTarget);
	    break;

	  case TOOL_FREEHAND:
	    {
	      HDC hDC = GetDC(hWnd);
	      SelectObject(hDC, hPen);
	      MoveToEx(hDC, xPos, yPos, NULL);
	      LineTo(hDC, xTarget, yTarget);
	      SetPixel(hDC, xTarget, yTarget, drawcolor);
	      ReleaseDC(hWnd, hDC);

	      xPos = xTarget;
	      yPos = yTarget;

	      DoSaveScreen(hWnd);
	      break;
	    }
	  }
      return 0;

    case WM_SIZE:
      RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
      return 0;

    case WM_CLOSE:
      if (MessageBox
	  (hWnd, "Vill du verkligen avsluta?", "Ritprogram",
	   MB_OKCANCEL | MB_ICONINFORMATION | MB_SETFOREGROUND) == IDOK)
	DestroyWindow(hWnd);
      return 0;

    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;

    case WM_LBUTTONDOWN:
      xTarget = xPos = LOWORD(lParam);	// horizontal position of cursor
      yTarget = yPos = HIWORD(lParam);	// vertical position of cursor
      MouseButtonDown = true;
      RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
      return 0;

    case WM_MOUSEMOVE:
      {
	if (!MouseButtonDown)
	  return 0;

	MouseButtonDown = wParam & MK_LBUTTON;
	if (!MouseButtonDown)
	  return 0;

	xTarget = LOWORD(lParam);
	yTarget = HIWORD(lParam);

	RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
	return 0;
      }

    case WM_LBUTTONUP:
      {
	MouseButtonDown = false;
	DoSaveScreen(hWnd);
	RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
	return 0;
      }

    case WM_COMMAND:
      {
	switch (LOWORD(wParam))
	  {
	    // Arkiv-menyn
	  case mnuFileNew:
	    CHOOSECOLOR StrChooseColor;

	    StrChooseColor.lStructSize = sizeof(CHOOSECOLOR);
	    StrChooseColor.hwndOwner = hWnd;
	    StrChooseColor.hInstance = NULL;
	    StrChooseColor.rgbResult = crBGColor;
	    StrChooseColor.lpCustColors = CustColors;
	    StrChooseColor.Flags = CC_FULLOPEN | CC_RGBINIT;
	    StrChooseColor.lCustData = NULL;
	    StrChooseColor.lpfnHook = NULL;
	    StrChooseColor.lpTemplateName = NULL;

	    if (!ChooseColor(&StrChooseColor))
	      return 0;

	    crBGColor = StrChooseColor.rgbResult;
	    SelectObject(hDC_COPY, CreateSolidBrush(crBGColor));	//Koppla pensel -> kopia
	    PatBlt(hDC_COPY, 0, 0, nSizeX, nSizeY, PATCOPY);	//M�la �ver ev. skr�p
	    RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
	    return 0;
	  case mnuFileExit:
	    SendMessage(hWnd, WM_CLOSE, 0, 0);
	    return 0;

	    // F�rg-menyn
	  case mnuColorRed:
	    drawcolor = 0x000000FF;
	    CheckMenuItem(hMenu, mnuColorRed, MF_CHECKED);
	    CheckMenuItem(hMenu, mnuColorGreen, MF_UNCHECKED);
	    CheckMenuItem(hMenu, mnuColorBlue, MF_UNCHECKED);
	    CheckMenuItem(hMenu, mnuColorOther, MF_UNCHECKED);
	    ReloadColor();
	    return 0;
	  case mnuColorGreen:
	    drawcolor = 0x0000FF00;
	    CheckMenuItem(hMenu, mnuColorRed, MF_UNCHECKED);
	    CheckMenuItem(hMenu, mnuColorGreen, MF_CHECKED);
	    CheckMenuItem(hMenu, mnuColorBlue, MF_UNCHECKED);
	    CheckMenuItem(hMenu, mnuColorOther, MF_UNCHECKED);
	    ReloadColor();
	    return 0;
	  case mnuColorBlue:
	    drawcolor = 0x00FF0000;
	    CheckMenuItem(hMenu, mnuColorRed, MF_UNCHECKED);
	    CheckMenuItem(hMenu, mnuColorGreen, MF_UNCHECKED);
	    CheckMenuItem(hMenu, mnuColorBlue, MF_CHECKED);
	    CheckMenuItem(hMenu, mnuColorOther, MF_UNCHECKED);
	    ReloadColor();
	    return 0;
	  case mnuColorOther:
	    StrChooseColor.lStructSize = sizeof(CHOOSECOLOR);
	    StrChooseColor.hwndOwner = hWnd;
	    StrChooseColor.hInstance = NULL;
	    StrChooseColor.rgbResult = drawcolor;
	    StrChooseColor.lpCustColors = CustColors;
	    StrChooseColor.Flags = CC_FULLOPEN | CC_RGBINIT;
	    StrChooseColor.lCustData = NULL;
	    StrChooseColor.lpfnHook = NULL;
	    StrChooseColor.lpTemplateName = NULL;

	    if (!ChooseColor(&StrChooseColor))
	      return 0;

	    CheckMenuItem(hMenu, mnuColorRed, MF_UNCHECKED);
	    CheckMenuItem(hMenu, mnuColorGreen, MF_UNCHECKED);
	    CheckMenuItem(hMenu, mnuColorBlue, MF_UNCHECKED);
	    CheckMenuItem(hMenu, mnuColorOther, MF_UNCHECKED);

	    switch (drawcolor = StrChooseColor.rgbResult)
	      {
	      case 0x000000FF:
		CheckMenuItem(hMenu, mnuColorRed, MF_CHECKED);
		break;
	      case 0x0000FF00:
		CheckMenuItem(hMenu, mnuColorGreen, MF_CHECKED);
		break;
	      case 0x00FF0000:
		CheckMenuItem(hMenu, mnuColorBlue, MF_CHECKED);
		break;
	      default:
		CheckMenuItem(hMenu, mnuColorOther, MF_CHECKED);
	      }

	    ReloadColor();
	    return 0;

	    // Verktyg-menyn
	  case mnuToolLine:
	  case mnuToolCircle:
	  case mnuToolRectangle:
	  case mnuToolText:
	  case mnuToolFreehand:
	    ChooseTool(LOWORD(wParam));
	    return 0;

	    // Text-menyn
	  case mnuTextChooseText:
	    DoChooseText(hWnd);
	    return 0;

	  case mnuTextChooseFont:
	    CHOOSEFONT StrChooseFont;
	    StrChooseFont.lStructSize = sizeof(CHOOSEFONT);
	    StrChooseFont.hwndOwner = hWnd;
	    StrChooseFont.hDC = NULL;
	    StrChooseFont.lpLogFont = &textfont;
	    StrChooseFont.iPointSize = 0;
	    StrChooseFont.Flags =
	      CF_INITTOLOGFONTSTRUCT | CF_FORCEFONTEXIST | CF_SCREENFONTS;
	    StrChooseFont.rgbColors = NULL;
	    StrChooseFont.lCustData = NULL;
	    StrChooseFont.lpfnHook = NULL;
	    StrChooseFont.lpTemplateName = NULL;
	    StrChooseFont.hInstance = NULL;
	    StrChooseFont.lpszStyle = NULL;
	    StrChooseFont.nFontType = NULL;
	    StrChooseFont.nSizeMin = 0;
	    StrChooseFont.nSizeMax = 128;

	    if (!ChooseFont(&StrChooseFont))
	      return 0;

	    textfont = *StrChooseFont.lpLogFont;

	    // �ppna typsnittet och st�ng det gamla
	    ReloadFont();
	    return 0;
	  }
      }
    default:
      return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}

// Funktion som raderar gammalt typsnitt och skapar en ny handle som
// textfunktionen kan anv�nda
void
ReloadFont()
{
  DeleteObject(hFont);
  hFont = CreateFontIndirect(&textfont);
}

// Funktion som raderar gammal penna och skapar en ny handle som
// ritfunktionerna kan anv�nda
void
ReloadColor()
{
  DeleteObject(hPen);
  hPen = CreatePen(PS_SOLID, 0, drawcolor);
}

// EXE-filens startkod - registrera f�nsterklass, skapa f�nster och
// initiera meddelande loop
void __declspec(noreturn) startup()
{
  hInstance = GetModuleHandle(NULL);

  STARTUPINFO si;
  GetStartupInfo(&si);

  // Initiera textfont-strukturen
  textfont.lfHeight = 16;
  textfont.lfWidth = 0;
  textfont.lfEscapement = 0;
  textfont.lfOrientation = 0;
  textfont.lfWeight = FW_DONTCARE;
  textfont.lfItalic = false;
  textfont.lfUnderline = false;
  textfont.lfStrikeOut = false;
  textfont.lfCharSet = DEFAULT_CHARSET;
  textfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
  textfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  textfont.lfQuality = DEFAULT_QUALITY;
  textfont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
  strcpy(textfont.lfFaceName, "Arial");
  ReloadFont();

  // Initiera bakgrundsf�rg
  ReloadColor();

  // Initiera hollow brush
  LOGBRUSH logBrush;
  logBrush.lbStyle = BS_HOLLOW;
  hBrush = CreateBrushIndirect(&logBrush);

  // Registrera f�nsterklass
  WNDCLASS wndclass;
  wndclass.lpszClassName = "RITPROGR";	// Klassens namn
  wndclass.hInstance = hInstance;	// Programinstans
  wndclass.lpfnWndProc = WindowProc;	// F�nsterprocedur
  wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);	// Musmark�r
  wndclass.hIcon = LoadIcon(hInstance, "IDI_FIRSTICON");	// Ikon
  wndclass.lpszMenuName = "IDM_MENU";	// Meny
  wndclass.hbrBackground = (HBRUSH) COLOR_WINDOW;	// Bakgrundsf�rg
  wndclass.style = 0L;	// F�nsterklass stil
  wndclass.cbClsExtra = 0;	// Extra bytes f�r klass
  wndclass.cbWndExtra = 0;	// Extra bytes f�r f�nster

  // Registrera klass
  RegisterClass(&wndclass);

  // Skapa f�nster
  HWND hWnd = CreateWindow(wndclass.lpszClassName,	// Klassnamn
			   "Ritprogram",	// F�nstertitel
			   WS_OVERLAPPEDWINDOW,	// F�nsterstil
			   CW_USEDEFAULT,	// X-punkt �vre v�nstra h�rn
			   CW_USEDEFAULT,	// Y-punkt �vre v�nstra h�rn
			   CW_USEDEFAULT,	// cx (bredd)
			   CW_USEDEFAULT,	// cy (h�jd)
			   NULL,	// F�r�lderf�nster
			   NULL,	// Meny
			   hInstance,	// Programinstans
			   NULL);	// Data pekare

  // Kolla att f�nstret verkligen skapades
  if (!hWnd)
    ExitProcess((DWORD)-1);

  // H�mta handle till menyn
  hMenu = GetMenu(hWnd);

  // Visa f�nster
  ShowWindow(hWnd, si.wShowWindow);

  // Initiera funktionaliteten f�r sk�rmuppdatering
  DoInitScreen(hWnd);

  // Meddelandeloop som h�mtar meddelanden
  MSG msg;
  while (GetMessage(&msg, 0, 0, 0) > 0)
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

  ExitProcess(0);
}
