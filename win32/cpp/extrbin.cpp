#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <winioctl.h>
#include <stdio.h>
#include <stdlib.h>

volatile bool ctrlbreak = false;

BOOL CALLBACK
CtrlHandler(DWORD)
{
  ctrlbreak = true;
  return TRUE;
}

int
main(int argc, char **argv)
{
  // search_data for VOB files = 0xBA010000 at 0x00000000, 0x0800 byte blocks.

  DWORD search_data = 0xBA010000;
  static const DWORD search_data_len = sizeof search_data;
  DWORD offset = 0;
  LPBYTE block = NULL;
  DWORD block_size = 0x0800;
  DWORD sector_size = 0;
  char *search_file = NULL;
  char *output_file = NULL;

  while (--argc > 0)
    if (strncmp(argv[argc], "if=", 3) == 0)
      search_file = (argv[argc])+3;
    else if (strncmp(argv[argc], "of=", 3) == 0)
      output_file = (argv[argc])+3;
    else if (strncmp(argv[argc], "data=", 5) == 0)
      search_data = strtoul((argv[argc])+5, NULL, 0);
    else if (strncmp(argv[argc], "offset=", 7) == 0)
      offset = strtoul((argv[argc])+7, NULL, 0);
    else if (strncmp(argv[argc], "bs=", 3) == 0)
      {
	char *suffix = NULL;
	block_size = strtoul((argv[argc])+3, &suffix, 0);
	switch (*suffix)
	  {
	  case 0:
	    break;

	  case 'G':
	    block_size <<= 10;
	  case 'M':
	    block_size <<= 10;
	  case 'K':
	    block_size <<= 10;
	    break;

	  case 'g':
	    block_size *= 1000;
	  case 'm':
	    block_size *= 1000;
	  case 'k':
	    block_size *= 1000;
	    break;

	  default:
	    fprintf(stderr, "Unknown size suffix: '%c'\n", *suffix);
	    return 8;
	  }
      }
    else if (strncmp(argv[argc], "ss=", 3) == 0)
      {
	char *suffix = NULL;
	sector_size = strtoul((argv[argc])+3, &suffix, 0);
	switch (*suffix)
	  {
	  case 0:
	    break;

	  case 'G':
	    sector_size <<= 10;
	  case 'M':
	    sector_size <<= 10;
	  case 'K':
	    sector_size <<= 10;
	    break;

	  case 'g':
	    sector_size *= 1000;
	  case 'm':
	    sector_size *= 1000;
	  case 'k':
	    sector_size *= 1000;
	    break;

	  default:
	    fprintf(stderr, "Unknown size suffix: '%c'\n", *suffix);
	    return 8;
	  }
      }
    else
      {
	fprintf(stderr,
		"Unknown parameter: '%s'\n"
		"\n"
		"Syntax:\n"
		"extrbin [if=<device>] [of=<outputfile>] [ss=<sectorsize>] [bs=<blocksize>]\r\n"
		"    [data=<32bit-data>] [offset=<search-data-offset-in-blocks>]\r\n", argv[argc]);
	return 8;
      }

  if (block_size == 0)
    {
      fprintf(stderr, "Block size cannot be zero.\n");
      return 8;
    }

  block = (LPBYTE)malloc(block_size);
  if (block == NULL)
    {
      perror(NULL);
      return 7;
    }

  CharToOem(search_file, search_file);
  CharToOem(output_file, output_file);
  SetFileApisToOEM();
  SetConsoleCtrlHandler(CtrlHandler, TRUE);

  HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
  if (search_file == NULL)
    fprintf(stderr, "Reading from 'stdin'\n");
  else
    {
      fprintf(stderr, "Opening input device '%s'\n", search_file);
      hIn = CreateFile(search_file,
		       GENERIC_READ,
		       FILE_SHARE_READ|FILE_SHARE_WRITE,
		       NULL,
		       OPEN_EXISTING,
		       FILE_FLAG_SEQUENTIAL_SCAN,
		       NULL);
    }

  if (hIn == INVALID_HANDLE_VALUE)
    {
      win_perror(search_file);
      return 1;
    }

  DWORD rep;
  if (sector_size == 0)
    {
      static DISK_GEOMETRY disk_geometry = { 0 };
      if (DeviceIoControl(hIn, IOCTL_DISK_GET_DRIVE_GEOMETRY,
			  NULL, 0,
			  &disk_geometry, sizeof disk_geometry,
			  &rep, NULL))
	{
	  sector_size = disk_geometry.BytesPerSector;
	  fprintf(stderr, "Aligning search to sector size %u bytes.\n",
		  sector_size);
	}
      else
	{
	  sector_size = 512;
	  fprintf(stderr,
		  "Device sector size unknown, aligning search at %u bytes.\n",
		  sector_size);
	}
    }

  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (output_file == NULL)
    fprintf(stderr, "Writing output to 'stdout'\n");
  else
    {
      fprintf(stderr, "Opening output file '%s'\n", output_file);
      hOut = CreateFile(output_file,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
    }

  if (hOut == INVALID_HANDLE_VALUE)
    {
      win_perror(output_file);
      return 2;
    }

  fprintf(stderr, "Searching for %u byte blocks.\n"
	  "Finding blocks having value 0x%p at offset 0x%p...\n",
	  block_size, search_data, offset);

  DWORD counter = 0;
  DWORDLONG byte_counter = 0;
  while (!ctrlbreak)
    {
      if (!ReadFile(hIn, block, sector_size, &rep, NULL))
	{
	  win_perror(search_file);
	  break;
	}

      if (rep != sector_size)
	break;

      if (memcmp(&search_data, block+offset, search_data_len < block_size ?
		 search_data_len : block_size) == 0)
	{
	  rep = 0;
	  if (sector_size < block_size)
	    if (!ReadFile(hIn, block+sector_size,
			  block_size-sector_size, &rep, NULL))
	      {
		win_perror(search_file);
		break;
	      }

	  byte_counter += rep+sector_size;
	  if (!WriteFile(hOut, block, rep+sector_size, &rep, NULL))
	    {
	      win_perror(output_file);
	      break;
	    }

	  counter++;
	  fprintf(stderr, "%u blocks found... Searching...\r", counter);
	}

      Sleep(0);
    }

  if (ctrlbreak)
    fprintf(stderr, "\nBreak.");

  fprintf(stderr, "\nDone - %I64u bytes saved in file '%s'.\n",
	  byte_counter,
	  output_file == NULL ? "stdout" : output_file);

  return 0;
}

