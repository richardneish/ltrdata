#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>

#include "ritprogr.h"
#include "mikael.h"
#include "maliheh.h"

//Funktionen rita rektangel
void DoDrawRectangle(HWND hWnd,int x,int y,int xPos,int yPos)
{
   HDC hDC = GetDC( hWnd );
   SelectObject( hDC, hPen );
   SelectObject( hDC, hBrush );
   Rectangle(hDC,x,y,xPos,yPos);
   ReleaseDC( hWnd, hDC );
}
//Funktionen rita linjer
void DoDrawLines(HWND hWnd, int x, int y, int xPos, int yPos)
{
   HDC hDC = GetDC( hWnd );
   SelectObject( hDC, hPen );
   MoveToEx( hDC, x, y, NULL );
   LineTo( hDC, xPos, yPos);
   ReleaseDC( hWnd, hDC );
}
//Funktionen rita cirkel
void DoDrawEllipse(HWND hWnd,int x,int y,int xPos,int yPos)
{
   HDC hDC = GetDC( hWnd );
   SelectObject( hDC, hPen );
   SelectObject( hDC, hBrush );
   Ellipse( hDC, x, y, xPos, yPos );
   ReleaseDC( hWnd, hDC );
}

