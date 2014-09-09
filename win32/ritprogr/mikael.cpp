#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>

#include "ritprogr.h"
#include "mikael.h"

/*---------------------------------------------------------------------------*/
/*                       GLOBALA VARIABLER                                   */
/*---------------------------------------------------------------------------*/

char      szText[256];                           //Text som stämplas ut
int      nSizeX,nSizeY;                           //Skärmens mått
HBITMAP    hBMP;                                    //Handtag till bitmap (BMP)
HDC       hDC_COPY;                               //Handtag till DC
HFONT    hFont = NULL;                           //Handtag till typsnitt (font)
HPEN       hPen = NULL;                          //Handtag till penna (ritfärg)
HBRUSH   hBrush = NULL;                         //Handtag till brush (ritfunk)

/*---------------------------------------------------------------------------*/
/*                       DEFINITION AV FUNKTIONER                            */
/*---------------------------------------------------------------------------*/

bool DoChooseText( HWND hWnd )
{
   //Poppa upp dialogrutan som väljer text
   if( DialogBox(hInstance,"MY_TEXTBOX",hWnd,(DLGPROC)TextboxProc) == -1 )
      return false;   //Det gick åt skogen
   else
      return true;   //Allt gick väl
}

bool DoInitScreen( HWND hWnd )
{
   //Skärmens mått (i pixlar)
   nSizeX = GetSystemMetrics( SM_CXSCREEN );
   nSizeY = GetSystemMetrics( SM_CYSCREEN );

   //Initiera texten
   strcpy(szText,"123");
   szText[sizeof(szText)-1] = 0;

   //--------------Initiera minnes-DC------------------------------------------
   HDC hDC = GetDC(hWnd);                                //Hämta DC för fönster
   hDC_COPY = CreateCompatibleDC(hDC);                 //Skapa kopia av DC
   static HBITMAP hBMP = CreateCompatibleBitmap(hDC,nSizeX,nSizeY);
   //Skapa bitmap från DC
   SelectObject(hDC_COPY,hBMP);                        //Koppla BMP:en -> kopia
   SelectObject(hDC_COPY,CreateSolidBrush(crBGColor)); //Koppla pensel -> kopia
   PatBlt(hDC_COPY,0,0,nSizeX,nSizeY,PATCOPY);        //Måla över ev. skräp
   ReleaseDC(hWnd,hDC);                               //Lämna DC -> fönster

   return true;   //Allt gick väl (?)
}

bool DoLoadScreen( HWND hWnd )
{
   int nResult;                                       //Resultat
   PAINTSTRUCT   psTemp;                                    //Ritstruktur
     HDC hDC = BeginPaint( hWnd, &psTemp );

   //Nu är det dags att rita om fönstret
   //Kopiera (innehållet i) minnes-DC till uppdaterings-DC
   nResult = BitBlt( hDC, 0, 0, nSizeX, nSizeY, hDC_COPY, 0, 0, SRCCOPY );
   EndPaint(hWnd,&psTemp);                            //Vi har ritat färdigt nu
   if (nResult != 0)
      return true;                                    //Allt gick väl
   else
      return false;                                    //Det gick åt skogen
}

bool DoSaveScreen( HWND hWnd )
{
   int result;
   HDC hDC = GetDC( hWnd );
   RECT Rect;
   GetClientRect( hWnd, &Rect );

   //Kopiera (innehållet i) DC till minnes-DC
   result = BitBlt( hDC_COPY, Rect.left, Rect.top, Rect.right, Rect.bottom, hDC, 0, 0, SRCCOPY );

   ReleaseDC( hWnd, hDC );
   if (result != 0)
      return true;
   else
      return false;
}

bool DoTextTool( HWND hWnd, int x, int y )
{
   HDC hDC = GetDC(hWnd);                                //Hämta Device Context
   SelectObject(hDC,hFont);                              //Välj typsnitt
   SetBkColor(hDC,crBGColor);                              //Sätter bg-färg
   TextOut(hDC,x,y,szText,strlen(szText));               //Stämpla text
   ReleaseDC( hWnd, hDC );
   return true;
}

/*---------------------------------------------------------------------------*/
/*                       HANTERING AV MEDDELANDE (TEXTBOX)                   */
/*---------------------------------------------------------------------------*/

long FAR PASCAL TextboxProc(
   HWND hDlg,                                             //Fönsterhandtag
   UINT   msg,                                              //Meddelande
   WPARAM wParam,                                              //Parameter (w)
   LPARAM /*lParam*/)                                           //Parameter (l)
{

   switch(msg)
   {
      case WM_COMMAND:                                       //Något ät tryckt?
         switch(LOWORD(wParam))
         {
            case IDOK:                                      //OK-knapp tryckt?
               //Hämta text (från textboxen i dialogruta) och ändra textstämpel
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

