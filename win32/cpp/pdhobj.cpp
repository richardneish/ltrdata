#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <shellapi.h>
#include <pdh.h>
#include <pdhmsg.h>

#pragma comment(lib, "shell32")
#pragma comment(lib, "pdh")

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt")
#endif

int main(int, char **argv)
{
    LPSTR      szCounterListBuffer     = "";
    DWORD       dwCounterListSize       = 0;
    LPSTR      szInstanceListBuffer    = "";
    DWORD       dwInstanceListSize      = 0;

    PDH_STATUS pdhStatus;

    if( argv[1] == NULL )
        pdhStatus = PdhEnumObjects(
            NULL,    // reserved
            NULL,  // machine name
            szCounterListBuffer,  // buffer for objects
            &dwCounterListSize,  // size of buffer
            (DWORD)-1,   // detail level
            true          // refresh flag for connected machines
        );
    else
        // Determine the required buffer size for the data.

        pdhStatus = PdhEnumObjectItems(
                NULL,                   // reserved
                NULL,                   // local machine
                argv[1],              // object to enumerate
                szCounterListBuffer,    // pass in NULL buffers
                &dwCounterListSize,     // an 0 length to get
                szInstanceListBuffer,   // required size
                &dwInstanceListSize,    // of the buffers in chars
                (DWORD)-1,     // counter detail level
            0);

    if ((pdhStatus != ERROR_SUCCESS) & (pdhStatus != PDH_MORE_DATA))
    {
        WPDHErrMsg errmsg(pdhStatus);
        fprintf(stderr, "%s", errmsg);
        return -1;
    }

    // Allocate the buffers and try the call again.
    szCounterListBuffer = new char[dwCounterListSize];
    szInstanceListBuffer = new char[dwInstanceListSize];

    szCounterListBuffer[0] = NULL;
    szInstanceListBuffer[0] = NULL;

    if( argv[1] == NULL )
        pdhStatus = PdhEnumObjects(
            NULL,    // reserved
            NULL,  // machine name
            szCounterListBuffer,  // buffer for objects
            &dwCounterListSize,  // size of buffer
            (DWORD)-1,   // detail level
            true          // refresh flag for connected machines
                );
    else
        pdhStatus = PdhEnumObjectItems (
                NULL,   // reserved
                NULL,   // local machine
                argv[1], // object to enumerate
                szCounterListBuffer,    // pass in NULL buffers
                &dwCounterListSize,     // an 0 length to get
                szInstanceListBuffer,   // required size
                &dwInstanceListSize,    // of the buffers in chars
                PERF_DETAIL_WIZARD,     // counter detail level
                0);

    if( pdhStatus != ERROR_SUCCESS )
        return 0;

    puts(">>>  Counter list");
    for( LPTSTR szThisCounter = szCounterListBuffer; *szThisCounter != 0;
                     szThisCounter += strlen(szThisCounter) + 1 )
      puts(szThisCounter);

    // Walk the return instance list.
    puts(">>>  Instance list");
    for( LPTSTR szThisInstance = szInstanceListBuffer; *szThisInstance != 0;
                     szThisInstance += strlen(szThisInstance) + 1 )
      puts(szThisInstance);

    delete szCounterListBuffer;
    delete szInstanceListBuffer;

    return 0;
}
