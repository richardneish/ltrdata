#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

enum
{
  NOERR,
  NOFILES,
  INFERR,
  OUTFERR,
  MEMERR,
  INFTOOSMALL,
  FRAGTOOSMALL,
  OUTFWRERR
};

void GetHelp(void);

int
main(int argc, char **argv)
{
  char *InFile, OutFile[MAX_PATH], *nrptr;
  int infh, outfh;
  unsigned bytesread, byteswritten, written = 0, outfsiz;
  unsigned long filecou = 0;
  char *buffer;

  if (argc - 1)
    if ((argv[1][0] | 2) == '/')
      {
	GetHelp();
	return NOERR;
      }

  if (argc - 1)
    InFile = argv[1];
  else
    {
      GetHelp();
      return NOERR;
    }

  printf("File split tool by Olof Lagerkvist.\n"
	 "http://www.ltr-data.se    olof@ltr-data.se\n"
	 "\n");

  printf("Input file: %s\n", InFile);

  if (InFile[0] == 0)
    return NOFILES;

  if (argc > 2)
    strncpy(OutFile, argv[2], sizeof OutFile);
  else
    strncpy(OutFile, argv[1], sizeof OutFile);

  OutFile[sizeof(OutFile) - 1] = 0;
  
  printf("Output file name template: %s\n", OutFile);

  if (OutFile[0] == 0)
    return NOFILES;

  if (argc > 3)
    {
      outfsiz = strtoul(argv[3], NULL, 10);

      if (outfsiz == 0)
	{
	  fprintf(stderr, "Invalid fragment size: %s\n", argv[3]);
	  return FRAGTOOSMALL;
	}
    }
  else
    outfsiz = 1457664;

  printf("Fragment size: %u\n", outfsiz);

  if ((buffer = (char *)malloc(outfsiz)) == NULL)
    {
      fputs("Insufficient memory for fragment size.\r\n", stderr);
      return MEMERR;
    }

  if ((infh = open(InFile, O_BINARY | O_RDONLY | O_SEQUENTIAL)) == -1)
    {
      perror(InFile);
      fputs("Error opening input file.\r\n", stderr);
      return INFERR;
    }

  nrptr = OutFile + strlen(OutFile);
  *(nrptr++) = '.';

  for (;;)
    {
      printf("Reading %s...\n", InFile);
      if ((bytesread = read(infh, buffer, outfsiz)) == -1)
	{
	  perror(InFile);
	  fprintf(stderr, "Error reading file %s.", InFile);
	  close(infh);
	  return INFERR;
	}

      if (bytesread == 0)
	break;

      ultoa(++filecou, nrptr, 10);
      printf("Writing %s...\n", OutFile);

      if ((outfh =
	   open(OutFile, O_BINARY | O_CREAT | O_SEQUENTIAL | O_WRONLY,
		S_IREAD | S_IWRITE)) == -1)
	{
	  perror(OutFile);
	  fprintf(stderr, "Error creating file %s.", OutFile);
	  close(infh);
	  return OUTFERR;
	}

      if ((byteswritten = write(outfh, buffer, bytesread)) == -1)
	{
	  perror(OutFile);
	  fputs("Error writing output file.\r\n", stderr);
	}

      close(outfh);
      written += byteswritten;

      if (byteswritten != bytesread)
	{
	  printf("\nDeleting %s...\n", OutFile);
	  _unlink(OutFile);
	  close(infh);
	  return OUTFERR;
	}
    }

  if (written != (unsigned)filelength(infh))
    fprintf(stderr, "Warning, %u byte not written.\n",
	    filelength(infh) - written);

  close(infh);
  fprintf(stderr, "File split into %s pieces.\n", nrptr);
}

void
GetHelp(void)
{
  puts("File split program by Olof Lagerkvist.\n"
       "http://www.ltr-data.se          olof@ltr-data.se\n"
       "This program splits large files into smaller pieces.\r\n"
       "\r\n"
       "  Syntax:\r\n"
       "fsplit32 bigfile [partfile [partsize]]\r\n"
       "\n"
       "bigfile      Input file to split into smaller pieces.\r\n"
       "\n"
       "partfile     Name template for part files. Actual files will get this name,\r\n"
       "             suffixed by .nnn, where nnn is number of file. If this parameter\r\n"
       "             is not specified, it is built automatically from input file name.\r\n"
       "\n"
       "partsize     Size of each part file.\r\n"
       "\n"
       "To reassemble the original file, you can for example use the COPY command with\r\n"
       "the /B switch.\r\n"
       "Example: COPY /B partfiles.* bigfile");
}
