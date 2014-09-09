nbnbs.exe: nbnbs.obj getopt.obj
	link /opt:ref,nowin98 /fixed:no nbnbs.obj getopt.obj

nbnbs.obj: nbnbs.c
	cl /DUSE_MINWCRT /Ox /G7F /MD /c nbnbs.c

getopt.obj: ..\..\c\getopt.c
	cl /DUSE_MINWCRT /Ox /G7F /MD /c ..\..\c\getopt.c

install: p:\utils\nbnbs.exe

p:\utils\nbnbs.exe: nbnbs.exe
	copy /y nbnbs.exe p:\utils\

clean:
	del *.obj *~
