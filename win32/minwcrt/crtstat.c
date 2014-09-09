#include "crtintrn.h"
#include "crtlib.h"
#include "fcntl.h"

char _osfile[20] = "";

int __argc = 0;
char **__argv = NULL;
char **_environ = NULL;
char *_pgmptr = NULL;

wchar_t *_wcmdln = NULL;
wchar_t ** __wargv = NULL;  /* pointer to table of wide cmd line args */
wchar_t * _wenviron[MAX_WENVIRON_COUNT] = { NULL };
/* pointer to wide environment */
wchar_t * _wpgmptr = NULL;  /* points to the module (EXE) wide name */
wchar_t **__winitenv = NULL;
char **__initenv = NULL;
int _fmode = O_TEXT;
int _commode = 0;
