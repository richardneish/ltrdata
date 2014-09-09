#define WIN32_LEAN_AND_MEAN
#include <wfind.h>

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

int main(int argc, char **argv)
{
	if (argc != 3)
		return -1;

   WFileFinder wf1(argv[1]);
   WFileFinder wf2(argv[2]);

   if (!wf1)
   	return 3;

   if (!wf2)
   	return 4;

   if ((QWORD)wf1.LastWriteTime() == 0)
   	wf1.LastWriteTime() = wf1.CreationTime();

   if ((QWORD)wf2.LastWriteTime() == 0)
   	wf2.LastWriteTime() = wf2.CreationTime();

	switch (CompareFileTime(&wf1.ftLastWriteTime, &wf2.ftLastWriteTime))
	{
   	case +1:
			return 1;
      case -1:
      	return 2;
   	default:
      	return 0;
   }
}

