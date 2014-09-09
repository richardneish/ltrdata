#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma comment(lib, "kernel32")

#pragma comment(linker, "/subsystem:console")
#pragma comment(linker, "/entry:main")

#define IO_BUFFER_SIZE 8192

int main()
{
   HANDLE hConOut = GetStdHandle(STD_OUTPUT_HANDLE);

   HANDLE hConPipe = GetStdHandle(STD_INPUT_HANDLE);

   char buffer[IO_BUFFER_SIZE];

   DWORD dwReadLength, i;

   while (ReadFile(hConPipe, buffer, sizeof buffer, &dwReadLength, NULL))
   {
      if (dwReadLength == 0)
         break;

      for (i=0; i < dwReadLength; i++)
      {
         Sleep(0);
         if (buffer[i] == 0x07)
            buffer[i] = 0x00;
      }

      if (!WriteFile(hConOut, buffer, dwReadLength, &dwReadLength, NULL))
         break;
   }

   ExitProcess(0);
}
