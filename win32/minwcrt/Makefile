OPTIMIZATION=/nologo /Ox /GFS- /GR- /MD /WX /W4 /DWIN32_LEAN_AND_MEAN

all_libs: ..\lib\minwcrt.lib ..\lib\nullcrt.lib

install: p:\utils\minwcrt.lib p:\utils\nullcrt.lib

p:\utils\minwcrt.lib: ..\lib\minwcrt.lib
	copy /y ..\lib\minwcrt.lib p:\utils\

p:\utils\nullcrt.lib: ..\lib\nullcrt.lib
	copy /y ..\lib\nullcrt.lib p:\utils\

..\lib\minwcrt.lib: crtac.obj crtwc.obj crtaw.obj crtww.obj crtstat.obj clstat.obj chkstk.obj ..\lib\llmath.lib
	lib /nologo /out:..\lib\minwcrt.lib crtdll.lib kernel32.lib shell32.lib crtac.obj crtwc.obj crtaw.obj crtww.obj crtstat.obj clstat.obj chkstk.obj ..\lib\llmath.lib

..\lib\nullcrt.lib: ncrtac.obj ncrtwc.obj ncrtaw.obj ncrtww.obj chkstk.obj crtstat.obj clstat.obj ..\lib\llmath.lib
	lib /nologo /out:..\lib\nullcrt.lib crtdll.lib ncrtac.obj ncrtwc.obj ncrtaw.obj ncrtww.obj chkstk.obj crtstat.obj clstat.obj ..\lib\llmath.lib

chkstk.obj: chkstk.c
	cl /c $(OPTIMIZATION) chkstk.c

crtac.obj: crtexe.c
	cl /Focrtac.obj /D_CRTBLD /c $(OPTIMIZATION) crtexe.c

crtwc.obj: crtexe.c
	cl /Focrtwc.obj /D_CRTBLD /DWPRFLAG /DUNICODE /D_UNICODE /c $(OPTIMIZATION) crtexe.c

crtaw.obj: crtexe.c
	cl /Focrtaw.obj /D_CRTBLD /D_WINMAIN_ /c $(OPTIMIZATION) crtexe.c

crtww.obj: crtexe.c
	cl /Focrtww.obj /D_CRTBLD /D_WINMAIN_ /DWPRFLAG /DUNICODE /D_UNICODE /c $(OPTIMIZATION) crtexe.c

crtstat.obj: crtstat.c
	cl /D_CRTBLD /c $(OPTIMIZATION) crtstat.c

clstat.obj: clstat.c
	cl /D_CRTBLD /c $(OPTIMIZATION) clstat.c

ncrtac.obj: ncrtexe.c
	cl /Foncrtac.obj /c $(OPTIMIZATION) ncrtexe.c

ncrtwc.obj: ncrtexe.c
	cl /Foncrtwc.obj /DWPRFLAG /DUNICODE /D_UNICODE /c $(OPTIMIZATION) ncrtexe.c

ncrtaw.obj: ncrtexe.c
	cl /Foncrtaw.obj /D_WINMAIN_ /c $(OPTIMIZATION) ncrtexe.c

ncrtww.obj: ncrtexe.c
	cl /Foncrtww.obj /D_WINMAIN_ /DWPRFLAG /DUNICODE /D_UNICODE /c $(OPTIMIZATION) ncrtexe.c
