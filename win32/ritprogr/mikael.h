/*---------------------------------------------------------------------------*/
/*                       DEKLARATIONER AV FUNKTIONER                         */
/*---------------------------------------------------------------------------*/

//F�nsterproceduren f�r textbox-dialogen
long FAR PASCAL TextboxProc(
						HWND hTextbox,
                  UINT msg,
                  WPARAM wParam,
                  LPARAM lParam);
//bool DoChooseFont();	//L�ter anv�ndaren v�lja typsnitt (font) och stil
bool DoChooseText( HWND hWnd );	//L�ter anv�ndaren byta text som st�mplas
bool DoInitScreen( HWND hWnd );	//Initierar minnes-DC:n m.m.
bool DoLoadScreen( HWND hWnd );	//N�r uppdatering av sk�rm beh�vs, ladda fr�n minnes-DC
bool DoSaveScreen( HWND hWnd );	//N�r vi ritat f�rdigt, lagra i minnes-DC
bool DoTextTool( HWND hWnd, int x, int y );	//St�mplar ut text vid aktuella koordinater

/*---------------------------------------------------------------------------*/
/*                       GLOBALA VARIABLER                                   */
/*---------------------------------------------------------------------------*/

extern char		szText[256];							//Text som st�mplas ut
extern int		nSizeX,nSizeY;							//Sk�rmens m�tt
extern HBITMAP 	hBMP;									//Handtag till bitmap (BMP)
extern HDC 		hDC,hDC_COPY,hDC_UPDATE;			//Handtag till DC
extern HFONT 	hFont;									//Handtag till typsnitt (font)
extern HPEN		hPen;                            //Handtag till penna (ritf�rg)
extern HBRUSH	hBrush;                         	//Handtag till brush (ritfunk)

