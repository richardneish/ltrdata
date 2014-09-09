#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <io.h>
#pragma comment(lib, "winstrct")
#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif
#endif

#ifdef _WIN32
#define fstat _fstat
#define stat _stat
#else
#define setmode(fd,m)
#endif

#ifndef O_SEQUENTIAL
#define O_SEQUENTIAL 0
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef FALSE
typedef int BOOL;
#define FALSE 0
#define TRUE 1
#endif

#define MMPSIZE 524288
#define IOBSIZE 16384

BOOL wrstdout(char *buffer, int length)
{
  return write(1, buffer, length) == length;
}

int fd;
int rdmapfilepos = 0;
int wrmapfilepos = 0;
char *wrmap = NULL;
char *wrmapptr = NULL;
struct stat sb;
BOOL wrfmap(char *buffer, int length)
{
  static struct stat sb2;

  if (wrmapptr == NULL)
    {
      sb2 = sb;

      if (sb2.st_size > MMPSIZE)
	{
	  wrmap = (char*)mmap(NULL, MMPSIZE, PROT_WRITE, MAP_SHARED, fd,
			      wrmapfilepos);
	  sb2.st_size -= MMPSIZE;
	}
      else
	wrmap = (char*)mmap(NULL, sb2.st_size, PROT_WRITE, MAP_SHARED, fd,
			    wrmapfilepos);

      if (wrmap == MAP_FAILED)
	return FALSE;

      wrmapptr = wrmap;
    }

  while (wrmapptr-wrmap+length > MMPSIZE)
    {
      memmove(wrmapptr, buffer, wrmap+MMPSIZE-wrmapptr);
      length -= (int) (wrmap+MMPSIZE-wrmapptr);

      munmap(wrmap, MMPSIZE);
      wrmapfilepos += MMPSIZE;
      if (sb2.st_size > MMPSIZE)
	{
	  wrmap = (char*)mmap(NULL, MMPSIZE, PROT_WRITE, MAP_SHARED, fd,
			      wrmapfilepos);
	  sb2.st_size -= MMPSIZE;
	}
      else
	wrmap = (char*)mmap(NULL, sb2.st_size, PROT_WRITE, MAP_SHARED, fd,
			    wrmapfilepos);
      if (wrmap == MAP_FAILED)
	return FALSE;

      wrmapptr = wrmap;
    }

  memmove(wrmapptr, buffer, length);
  wrmapptr += length;

  return TRUE;
}

BOOL convert(BOOL (*sendfnc)(char *buf, int len), char *buffer, int length)
{
  char *lastbeginpos = NULL;
  char *lastendpos = buffer;
  char *ptr;

  for (ptr = buffer; ptr-buffer < length; ptr++)
    if (*ptr == '\r')
      {
	if (lastendpos != NULL)
	  {
	    if (!sendfnc(lastendpos, (int) (ptr-lastendpos)))
	      return FALSE;
	    lastbeginpos = ptr;
	    lastendpos = NULL;
	  }
	else if (lastbeginpos == NULL)
	  lastbeginpos = ptr;
      }
    else
      if ((lastbeginpos != NULL) & (lastendpos == NULL))
	lastendpos = ptr;

  if (lastendpos != NULL)
    if (!sendfnc(lastendpos, (int) (ptr-lastendpos)))
      return FALSE;

  return TRUE;
}

int main(int argc, char **argv)
{
  static char buffer[IOBSIZE];
  int length;
  char *rdmap;
  
  if (argc == 1)
    {
      setmode(1, O_BINARY);

      for (;;)
	{
	  usleep(1000);
	  length = read(0, buffer, sizeof buffer);
	  if (length == -1)
	    {
	      perror("read");
	      return 1;
	    }

	  if (length == 0)
	    return 0;

	  if (!convert(wrstdout, buffer, length))
	    {
	      perror("write");
	      return 1;
	    }
	}
    }

  while ((++argv)[0] != NULL)
    {
      fd = open(argv[0], O_RDWR|O_SEQUENTIAL|O_BINARY);
      if (fd == -1)
	{
	  perror("open");
	  fprintf(stderr, "%s failed.\n", argv[0]);
	  continue;
	}

      if (fstat(fd, &sb) == -1)
	{
	  perror("fstat");
	  fprintf(stderr, "%s failed.\n", argv[0]);
	  close(fd);
	  continue;
	}

      rdmapfilepos = 0;
      wrmapfilepos = 0;
      wrmap = NULL;
      wrmapptr = NULL;

      while (sb.st_size > MMPSIZE)
	{
	  usleep(1000);
	  rdmap = (char*)mmap(NULL, MMPSIZE, PROT_READ, MAP_SHARED, fd,
			      rdmapfilepos);
	  if (rdmap == MAP_FAILED)
	    break;

	  if (!convert(wrfmap, rdmap, MMPSIZE))
	    break;

	  munmap(rdmap, MMPSIZE);
	  rdmapfilepos += MMPSIZE;
	  sb.st_size -= MMPSIZE;
	}

      if (sb.st_size > MMPSIZE)
	{
	  perror("mmap");
	  fprintf(stderr, "%s failed.\n", argv[0]);

	  if ((wrmap != NULL) & (wrmap != MAP_FAILED))
	    munmap(wrmap, MMPSIZE);

	  close(fd);
	  continue;
	}

      rdmap = (char*)mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd,
			  rdmapfilepos);
      if (rdmap == MAP_FAILED)
	{
	  perror("mmap");
	  fprintf(stderr, "%s failed.\n", argv[0]);
	  close(fd);
	  continue;
	}

      if (!convert(wrfmap, rdmap, sb.st_size))
	{
	  perror("write end map");
	  fprintf(stderr, "%s failed.\n", argv[0]);

	  if ((rdmap != NULL) & (rdmap != MAP_FAILED))
	    munmap(rdmap, MMPSIZE);

	  if ((wrmap != NULL) & (wrmap != MAP_FAILED))
	    munmap(wrmap, MMPSIZE);

	  close(fd);
	  continue;
	}

      if ((rdmap != NULL) & (rdmap != MAP_FAILED))
	munmap(rdmap, MMPSIZE);
      
      if ((wrmap != NULL) & (wrmap != MAP_FAILED))
	{
	  munmap(wrmap, MMPSIZE);

	  if (ftruncate(fd, (int) (wrmapfilepos + (wrmapptr - wrmap))) == -1)
	    {
	      perror("ftruncate");
	      fprintf(stderr, "%s failed.\n", argv[0]);
	    }
	}

      close(fd);
    }
    
  return 0;
}
