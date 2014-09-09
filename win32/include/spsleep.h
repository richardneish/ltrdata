
#ifdef _WIN64

#define YieldSingleProcessor()

#else

extern void (__stdcall *YieldSingleProcessor)();

#endif
