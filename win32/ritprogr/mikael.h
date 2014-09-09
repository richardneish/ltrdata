/*---------------------------------------------------------------------------*/
/*                       DEKLARATIONER AV FUNKTIONER                         */
/*---------------------------------------------------------------------------*/

//Fönsterproceduren för textbox-dialogen
long FAR PASCAL TextboxProc(
						HWND hTextbox,
                  UINT msg,
                  WPARAM wParam,
                  LPARAM lParam);
//bool DoChooseFont();	//Låter användaren välja typsnitt (font) och stil
bool DoChooseText( HWND hWnd );	//Låter användaren byta text som stämplas
bool DoInitScreen( HWND hWnd );	//Initierar minnes-DC:n m.m.
bool DoLoadScreen( HWND hWnd );	//När uppdatering av skärm behövs, ladda från minnes-DC
bool DoSaveScreen( HWND hWnd );	//När vi ritat färdigt, lagra i minnes-DC
bool DoTextTool( HWND hWnd, int x, int y );	//Stämplar ut text vid aktuella koordinater

/*---------------------------------------------------------------------------*/
/*                       GLOBALA VARIABLER                                   */
/*---------------------------------------------------------------------------*/

extern char		szText[256];							//Text som stämplas ut
extern int		nSizeX,nSizeY;							//Skärmens mått
extern HBITMAP 	hBMP;									//Handtag till bitmap (BMP)
extern HDC 		hDC,hDC_COPY,hDC_UPDATE;			//Handtag till DC
extern HFONT 	hFont;									//Handtag till typsnitt (font)
extern HPEN		hPen;                            //Handtag till penna (ritfärg)
extern HBRUSH	hBrush;                         	//Handtag till brush (ritfunk)

