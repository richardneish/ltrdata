extern COLORREF drawcolor;
extern HMENU hMenu;
extern COLORREF CustColors[16];
extern LOGFONT textfont;
extern COLORREF crBGColor;
extern HINSTANCE hInstance;
extern WORD xPos;
extern WORD yPos;

extern enum Tools
{
   TOOL_LINE,
   TOOL_CIRCLE,
   TOOL_RECTANGLE,
   TOOL_TEXT,
   TOOL_FREEHAND
} tool;

void ReloadFont();
void ReloadColor();

