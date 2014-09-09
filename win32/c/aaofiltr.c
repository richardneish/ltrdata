#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdio.h>

#pragma comment(lib, "kernel32")
#pragma comment(lib, "user32")

#pragma comment(linker, "/subsystem:console")
#pragma comment(linker, "/entry:main")

void lputs(HANDLE h, LPCSTR lpStr)
{
  DWORD dw;
  WriteFile(h, lpStr, lstrlenA(lpStr), &dw, NULL);
}

int main()
{
  char buffer[261];		// Buffert fˆr inmatad text
  DWORD count;
  DWORD dw;
  HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  
  for (;;)
    {
      // LÂt anv skriva in text
      if (!ReadFile(hIn, buffer, sizeof(buffer)-1, &count, NULL))
	break;
      if (count == 0)
	break;
      buffer[count] = 0;

      // Vandra genom str‰ngen sÂ l‰nge tecknet vi ‰r pÂ
      // inte ‰r ascii 0.
      for (count=0; buffer[count]; ++count)
	// Kolla tecknet vi ‰r pÂ. Fˆr varje Â‰ˆ≈ƒ÷ skrivs
	// dess motsvarighet, annars tecknet i str‰ngen.
	// H‰nsyn tas till om n‰sta tecken ‰r lowercase. I
	// sÂ fall blir ≈ Aa ist‰llet fˆr AA.
	switch (buffer[count])
	  {
	  case 'Ü':
	    lputs(hOut, "aa");
	    break;
	  case 'Ñ':
	    lputs(hOut, "ae");
	    break;
	  case 'î':
	    lputs(hOut, "oe");
	    break;
	  case 'è':
	    lputs(hOut, IsCharLower(buffer[count+1]) ? "Aa" : "AA");
	    break;
	  case 'é':
	    lputs(hOut, IsCharLower(buffer[count+1]) ? "Ae" : "AE");
	    break;
	  case 'ô':
	    lputs(hOut, IsCharLower(buffer[count+1]) ? "Oe" : "OE");
	    break;
	  default:
	    WriteFile(hOut, buffer+count, 1, &dw, NULL);
	  }
    }

  ExitProcess(0);
}
