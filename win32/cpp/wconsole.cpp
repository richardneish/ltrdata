#define WIN32_LEAN_AND_MEAN
#include <wconsole.h>

#ifdef _MSC_VER
#pragma comment(lib, "user32")
#endif

//-- wconsole.h definitions
//--------------- "virtual" functions need storage for empty classes
const class WConsoleStdIn wcStdIn;
const class WConsoleStdOut wcStdOut;
const class WConsoleStdErr wcStdErr;
