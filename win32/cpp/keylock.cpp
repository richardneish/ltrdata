#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>

#if defined(_DLL) && !defined(_WIN64)
#include <minwcrtlib.h>
#pragma comment(lib, "minwcrt.lib")
#else
#include <stdlib.h>
#endif

int
syntax_help()
{
  MessageBox(NULL,
	     "Syntax:\r\n"
	     "KEYLOCK [CAPS|NUM|SCROLL|INSERT] [ON|OFF]",
	     "KEYLOCK",
	     MB_ICONEXCLAMATION);
  return 0;
}

int
WINAPI
WinMain(HINSTANCE /*hInstance*/,
	HINSTANCE /*hPrevInstance*/,
	LPSTR /*lpCmdLine*/,
	int /*nCmdShow*/)
{
  if (__argc != 3)
    return syntax_help();

  WORD vKey = 0;

  if (strcmpi(__argv[1], "CAPS") == 0)
    vKey = VK_CAPITAL;
  else if (strcmpi(__argv[1], "NUM") == 0)
    vKey = VK_NUMLOCK;
  else if (strcmpi(__argv[1], "SCROLL") == 0)
    vKey = VK_SCROLL;
  else if (strcmpi(__argv[1], "INSERT") == 0)
    vKey = VK_INSERT;
  else
    return syntax_help();

  SHORT wanted_keystate = 0;

  if (strcmpi(__argv[2], "ON") == 0)
    wanted_keystate = 1;
  else if (strcmpi(__argv[2], "OFF") == 0)
    wanted_keystate = 0;
  else
    return syntax_help();

  SHORT keystate = GetKeyState(vKey);
  if ((keystate & 1) == wanted_keystate)
    return 0;

  INPUT keybd = { INPUT_KEYBOARD };
  keybd.ki.wVk = vKey;

  SendInput(1, &keybd, sizeof keybd);

  keybd.ki.dwFlags = KEYEVENTF_KEYUP;

  SendInput(1, &keybd, sizeof keybd);

  return 0;
}
