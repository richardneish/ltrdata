#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>

#include "ritprogr.h"
#include "mikael.h"

/*---------------------------------------------------------------------------*/
/*                       GLOBALA VARIABLER                                   */
/*---------------------------------------------------------------------------*/

char      szText[256];                           //Text som st�mplas ut
int      nSizeX,nSizeY;                           //Sk�rmens m�tt
HBITMAP    hBMP;                                    //Handtag till bitmap (BMP)
HDC       hDC_COPY;                               //Handtag till DC
HFONT    hFont = NULL;                           //Handtag till typsnitt (font)
HPEN       hPen = NULL;                          //Handtag till penna (ritf�rg)
HBRUSH   hBrush = NULL;                         //Handtag till brush (ritfunk)

/*---------------------------------------------------------------------------*/
/*                       DEFINITION AV FUNKTIONER                            */
/*---------------------------------------------------------------------------*/

bool DoChooseText( HWND hWnd )
{
   //Poppa upp dialogrutan som v�ljer text
   if( DialogBox(hInstance,"MY_TEXTBOX",hWnd,(DLGPROC)TextboxProc) == -1 )
      return false;   //Det gick �t skogen
   else
      return true;   //Allt gick v�l
}

bool DoInitScreen( HWND hWnd )
{
   //Sk�rmens m�tt (i pixlar)
   nSizeX = GetSystemMetrics( SM_CXSCREEN );
   nSizeY = GetSystemMetrics( SM_CYSCREEN );

   //Initiera texten
   strcpy(szText,"123");
   szText[sizeof(szText)-1] = 0;

   //--------------Initiera minnes-DC------------------------------------------
   HDC hDC = GetDC(hWnd);                                //H�mta DC f�r f�nster
   hDC_COPY = CreateCompatibleDC(hDC);                 //Skapa kopia av DC
   static HBITMAP hBMP = CreateCompatibleBitmap(hDC,nSizeX,nSizeY);
   //Skapa bitmap fr�n DC
   SelectObject(hDC_COPY,hBMP);                        //Koppla BMP:en -> kopia
   SelectObject(hDC_COPY,CreateSolidBrush(crBGColor)); //Koppla pensel -> kopia
   PatBlt(hDC_COPY,0,0,nSizeX,nSizeY,PATCOPY);        //M�la �ver ev. skr�p
   ReleaseDC(hWnd,hDC);                               //L�mna DC -> f�nster

   return true;   //Allt gick v�l (?)
}

bool DoLoadScreen( HWND hWnd )
{
   int nResult;                                       //Resultat
   PAINTSTRUCT   psTemp;                                    //Ritstruktur
     HDC hDC = BeginPaint( hWnd, &psTemp );

   //Nu �r det dags att rita om f�nstret
   //Kopiera (inneh�llet i) minnes-DC till uppdaterings-DC
   nResult = BitBlt( hDC, 0, 0, nSizeX, nSizeY, hDC_COPY, 0, 0, SRCCOPY );
   EndPaint(hWnd,&psTemp);                            //Vi har ritat f�rdigt nu
   if (nResult != 0)
      return true;                                    //Allt gick v�l
   else
      return false;                                    //Det gick �t skogen
}

bool DoSaveScreen( HWND hWnd )
{
   int result;
   HDC hDC = GetDC( hWnd );
   RECT Rect;
   GetClientRect( hWnd, &Rect );

   //Kopiera (inneh�llet i) DC till minnes-DC
   result = BitBlt( hDC_COPY, Rect.left, Rect.top, Rect.right, Rect.bottom, hDC, 0, 0, SRCCOPY );

   ReleaseDC( hWnd, hDC );
   if (result != 0)
      return true;
   else
      return false;
}

bool DoTextTool( HWND hWnd, int x, int y )
{
   HDC hDC = GetDC(hWnd);                                //H�mta Device Context
   SelectObject(hDC,hFont);                              //V�lj typsnitt
   SetBkColor(hDC,crBGColor);                              //S�tter bg-f�rg
   TextOut(hDC,x,y,szText,strlen(szText));               //St�mpla text
   ReleaseDC( hWnd, hDC );
   return true;
}

/*---------------------------------------------------------------------------*/
/*                       HANTERING AV MEDDELANDE (TEXTBOX)                   */
/*---------------------------------------------------------------------------*/

long FAR PASCAL TextboxProc(
   HWND hDlg,                                             //F�nsterhandtag
   UINT   msg,                                              //Meddelande
   WPARAM wParam,                                              //Parameter (w)
   LPARAM /*lParam*/)                                           //Parameter (l)
{

   switch(msg)
   {
      case WM_COMMAND:                                       //N�got �t tryckt?
         switch(LOWORD(wParam))
         {
            case IDOK:                                      //OK-knapp tryckt?
               //H�mta text (fr�n textboxen i dialogruta) och �ndra textst�mpel
               GetDlgItemText(hDlg,101,szText,256);
               EndDialog(hDlg,wParam);
               break;
            case IDCANCEL:                               //CANCEL-knapp tryckt?
               EndDialog(hDlg,wParam);
               break;
         }
         break;
      case WM_INITDIALOG:                       //Vid initiering av dialogruta
         //Fyll textbox med tidigare text
         SetDlgItemText( hDlg, 101, szText);
         break;
      default:
         return false;                          //Meddelande har ej bearbetats
   }
   return true;                                 //Meddelande har bearbetats
}

