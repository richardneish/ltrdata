#define WINVER          0x0400
#define _WIN32_WINDOWS  0x0400
#define _WIN32_WINNT    0x0400
#define WIN32_LEAN_AND_MEAN

#include <winstrct.h>
#include "lightrep_log.h"

WMappedFile *LRStatistics::pfm = NULL;
int LRStatistics::iRefCount = 0;


