#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#define APPNAME "CUT32"
#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif
#else
#define APPNAME "CUT"
#endif

void  help( void );

// Den här buffertlängden skulle egentligen kunna vara
// mycket större för 32-bitarsversionen.
#define BUFLEN ((size_t)32767)

int
main( int argc, char *argv[] )
{
  int   margin,
    offset=0;
  char	*buf;
  unsigned  lines=0;

  if( argc - 1 )
    {
      if( (argv[1][0] | 2) == '/' )
	{
	  help();
	  return 0;
	}
      if( argc - 2 )
	offset = atoi( argv[2] );

      margin = atoi( argv[1] );

      if( margin + offset >= BUFLEN - 1 )
	{
	  puts( "F”r stort parameterv„rde." );
	  return 0;
	}
    }
  else
    {
      help();
      return 0;
    }

  if( (buf = (char*)malloc( BUFLEN )) == NULL )
    {
      fputs( "Otillr„ckligt minne.", stderr );
      return 0;
    }

  for (;;)
    {
      buf[margin] = 0;

      if( fgets( buf, BUFLEN - 1, stdin ) == NULL )
	return lines;

      buf[BUFLEN - 2] = '\n';
      buf[BUFLEN - 1] = 0;

      if( offset && buf[margin + offset] )
	{
	  buf[margin + offset] = '\n';
	  buf[margin + offset + 1] = 0;
	}

      if( fputs( buf + margin, stdout ) == EOF )
	return lines;

      ++ lines;
#ifdef __WIN32__
      Sleep(0);	// Thread pause så att andra processer kan köra
#endif
    }
}

void help( void )
{
  puts(
       APPNAME
       ".EXE - Freeware av Olof Lagerkvist 1998.\r\n"
       "http://www.ltr-data.se      olof@ltr-data.se\r\n"
       "L„s "
       APPNAME
       ".TXT f”r information om distribution m m.\r\n"
       "\n"
       "Det h„r programmet kopierar fr†n STDIN till STDOUT men klipper bort texten\r\n"
       "till v„nster om en viss position. Offset anger maximal radl„ngd.\r\n"
       "\n"
       APPNAME
       " marginal [offset]\r\n"
       "\n"
       "Programmet returnerar antal behandlade rader."
       );
}
