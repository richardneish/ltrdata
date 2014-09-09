#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <imagehlp.h>
#include <stdio.h>

#pragma comment(lib, "imagehlp")

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

int
main(int argc, char **argv)
{
  DWORD dwHeaderSum, dwCheckSum;

  switch (MapFileAndCheckSum(argv[1], &dwHeaderSum, &dwCheckSum))
    {
    case CHECKSUM_SUCCESS:
      break;

    case CHECKSUM_OPEN_FAILURE:
      fputs("Error opening file.\r\n", stderr);
      return -1;

    case CHECKSUM_MAP_FAILURE:
      fputs("Error creating file mapping.\r\n", stderr);
      return -1;

    case CHECKSUM_MAPVIEW_FAILURE:
      fputs("Error mapping view of file.\r\n", stderr);
      return -1;

    case CHECKSUM_UNICODE_FAILURE:
      fputs("Error converting filename to unicode.\r\n", stderr);
      return -1;

    default:
      fputs("Unexpected error.", stderr);
      return -1;
    }

  printf("Header: %p, Counted: %p\n", dwHeaderSum, dwCheckSum);

  return dwHeaderSum != dwCheckSum;
}

