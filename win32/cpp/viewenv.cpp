#include <iostream.h>
#include <stdlib.h>
#include <strstrea.h>

int main(int, char **argv, char **envp)
{
  cout <<
    "HTTP/1.1 200 OK\r\n"
    "Connetion: Close\r\n"
    "Content-type: text/plain\r\n";

  cout << "\r\nEnvironment variables passed:\r\n";

  for (int c = 0; envp[c]; c++)
    cout << envp[c] << "\r\n";

  cout << "\r\nSTDIN buffer contents:\r\n";
  for (;;)
    {
      cin.get(*cout.rdbuf());
      if (cin.eof())
	break;
      cout << (char)cin.get();
    }

  cout << "\r\nArguments passed:\r\n";
  for (int c = 0; argv[c]; c++)
    cout << "parameter " << c << ": '" << argv[c] << "'\r\n";

  cout << flush;
}

