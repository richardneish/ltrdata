makeall: factor.exe primes.exe

factor.exe: factor.c pr_tbl.c ..\..\c\getopt.c ..\..\c\err.c
	cl /Ox /G7F /DUSE_MINWCRT /MD factor.c pr_tbl.c ..\..\c\getopt.c ..\..\c\err.c /link /fixed:no /opt:ref,nowin98

primes.exe: primes.c pr_tbl.c pattern.c ..\..\c\getopt.c ..\..\c\err.c
	cl /Ox /G7F /DUSE_MINWCRT /MD primes.c pr_tbl.c pattern.c ..\..\c\getopt.c ..\..\c\err.c /link /fixed:no /opt:ref,nowin98
