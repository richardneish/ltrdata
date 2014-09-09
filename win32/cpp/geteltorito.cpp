#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <stdlib.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif

#pragma pack(1)

int
main(int argc, char **argv)
{
  if (argc == 2 ? (argv[1][0] | 0x02) == '/' : true)
    {
      puts
	("geteltorito, freeware by Olof Lagerkvist. Version 1.1 "
	 __TIMESTAMP__ "\r\n"
	 "http://www.ltr-data.se   e-mail: olof@ltr-data.se\r\n"
	 "More info, including distribution permissions and\r\n"
	 "source code is available on the website.\r\n"
	 "\n"
	 "This is a tool for extracting El Torito default boot image from a CD/DVD ISO\r\n"
	 "image.\r\n"
	 "\n"
	 "Usage:\r\n"
	 "geteltorito isofile > extractedimagefile\r\n"
	 "\n"
	 "Where 'isofile' is an existing ISO image file and extractedimagefile is a file\r\n"
	 "to which the extracted image is saved.\r\n"
	 "\n"
	 "To extract from a physical CD/DVD instead of an ISO image file, use \\\\.\\D:\r\n"
	 "as file name, where D is the drive letter of the CD/DVD reader.");
      return -1;
    }


  HANDLE hISOImage = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, 0, NULL);

  if (hISOImage == INVALID_HANDLE_VALUE)
    {
      win_perror(argv[1]);
      return 1;
    }

  union ELTORITO_HEADER
  {
    struct
    {
      char header[71];
      unsigned long offset;
    };
    char sector_fill[2048];
  } eltorito_header;

  const char typical_eltorito_header[71] =
    "\x00"
    "CD001"
    "\x01"
    "EL TORITO SPECIFICATION";

  OVERLAPPED ol = { 0 };
  ol.Offset = 17 * 2048;

  DWORD dwReadLen;
  if (!ReadFile(hISOImage, &eltorito_header, sizeof eltorito_header,
		&dwReadLen, &ol))
    {
      win_perror(argv[1]);
      return 1;
    }

  if (memcmp(eltorito_header.header, typical_eltorito_header,
	     sizeof typical_eltorito_header))
    {
      fputs("Image does not contain an El Torito default boot image.\r\n",
	    stderr);
      return 0;
    }

  union ELTORITO_IMAGE_HEADER
  {
    struct
    {
      char header[38];
      unsigned short length;
      unsigned long start;
    };
    char sector_fill[2048];
  } eltorito_image_header;

  LARGE_INTEGER offset;
  offset.QuadPart = (LONGLONG) eltorito_header.offset << 11;
  ZeroMemory(&ol, sizeof ol);
  ol.Offset = offset.LowPart;
  ol.OffsetHigh = offset.HighPart;

  if (!ReadFile(hISOImage, &eltorito_image_header,
		sizeof eltorito_image_header, &dwReadLen, &ol))
    {
      win_perror(argv[1]);
      return 1;
    }

  char *image = (char*) malloc(eltorito_image_header.length << 11);
  if (image == NULL)
    {
      perror(NULL);
      return 3;
    }

  offset.QuadPart = (LONGLONG) eltorito_image_header.start << 11;
  ZeroMemory(&ol, sizeof ol);
  ol.Offset = offset.LowPart;
  ol.OffsetHigh = offset.HighPart;

  if (!ReadFile(hISOImage, image, eltorito_image_header.length << 9,
		&dwReadLen, &ol))
    {
      win_perror(argv[1]);
      return 1;
    }

  if (!WriteFile(hStdOut, image, dwReadLen, &dwReadLen, NULL))
    {
      win_perror("stdout");
      return 4;
    }

  return 0;
}
