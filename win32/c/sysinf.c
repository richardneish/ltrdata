#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "kernel32")

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "nullcrt")
#endif

typedef void (WINAPI *tGetSystemInfo)(LPSYSTEM_INFO);

int main()
{
  SYSTEM_INFO si;
  tGetSystemInfo pGetSystemInfo = (tGetSystemInfo)
    GetProcAddress(GetModuleHandle("kernel32.dll"),
		   "GetNativeSystemInfo");
  if (pGetSystemInfo == NULL)
    pGetSystemInfo = GetSystemInfo;

  pGetSystemInfo(&si);

  printf("Processor Architecture: ");
  switch (((WORD *)&si)[0])
    {
    case PROCESSOR_ARCHITECTURE_INTEL:
      printf("Intel %ux86", (DWORD)si.wProcessorLevel);
      break;
    case PROCESSOR_ARCHITECTURE_MIPS:
      printf("MIPS R%u000", (DWORD)si.wProcessorLevel);
      break;
    case PROCESSOR_ARCHITECTURE_ALPHA:
      printf("Alpha %u", (DWORD)si.wProcessorLevel);
      break;
    case PROCESSOR_ARCHITECTURE_PPC:
      printf("Power PC ");
      switch (si.wProcessorLevel)
	{
	case 1:
	  printf("601");
	  break;
	case 3:
	  printf("603");
	  break;
	case 4:
	  printf("604");
	  break;
	case 6:
	  printf("603+");
	  break;
	case 9:
	  printf("604+");
	  break;
	case 20:
	  printf("620");
	  break;
	default:
	  printf("Unknown");
	}
      break;
    default:
      printf("Unknown processor %u", si.wProcessorLevel);
    }

  puts("");

  printf("Architecture ID: %u\n"
	 "Page Size: %u\n"
	 "Minimum Application Address: %p\n"
	 "Maximum Application Address: %p\n"
	 "Number of processors: %u\n"
	 "Processor type: ",
	 (DWORD)((WORD *)&si)[1], si.dwPageSize,
	 si.lpMinimumApplicationAddress, si.lpMaximumApplicationAddress,
	 si.dwNumberOfProcessors);

  switch (si.dwProcessorType)
    {
    case PROCESSOR_INTEL_386:
      printf("Intel 386");
      break;
    case PROCESSOR_INTEL_486:
      printf("Intel 486");
      break;
    case PROCESSOR_INTEL_PENTIUM:
      printf("Intel Pentium");
      break;
    case PROCESSOR_MIPS_R4000:
      printf("MIPS 4000");
      break;
    case PROCESSOR_ALPHA_21064:
      printf("Alpha 21064");
      break;
    default:
      printf("Unknown");
    }
  
  printf("\n"
	 "Allocation Granularity: %u\n"
	 "Processor Revision: %p\n",
	 si.dwAllocationGranularity, si.wProcessorRevision);

  return 0;
}
