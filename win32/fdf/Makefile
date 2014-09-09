!IF "$(CPU)" == ""
CPU=$(_BUILDARCH)
!ENDIF

!IF "$(CPU)" == ""
CPU=i386
!ENDIF

WARNING_LEVEL=/nologo /WX /W4 /wd4214 /wd4201 /wd4206
!IF "$(CPU)" == "i386"
OPTIMIZATION=/O2 /GF /G7 /GR- /MD
!ELSE
OPTIMIZATION=/O2 /GFS- /GR- /MD
!ENDIF
LINK_SWITCHES=/nologo /defaultlib:bufferoverflowU.lib /release /opt:nowin98,ref,icf=10 /fixed:no /release

all: $(CPU)\fdf.exe $(CPU)\xorsum.exe

$(CPU)\fdf.exe: Makefile $(CPU)\fdf.obj $(CPU)\chkfile.obj $(CPU)\lnk.obj ..\lib\winstrct.lib fdf.res
	link $(LINK_SWITCHES) /out:$(CPU)\fdf.exe $(CPU)\fdf.obj $(CPU)\chkfile.obj $(CPU)\lnk.obj fdf.res

$(CPU)\xorsum.exe: Makefile $(CPU)\xorsum.obj $(CPU)\chkfile.obj
	link $(LINK_SWITCHES) /out:$(CPU)\xorsum.exe $(CPU)\xorsum.obj $(CPU)\chkfile.obj

$(CPU)\fdf.obj: Makefile fdf.cpp fdftable.hpp lnk.h ..\include\winstrct.h ..\include\wfind.h ..\include\wconsole.h
	cl /c $(WARNING_LEVEL) $(OPTIMIZATION) /Fp$(CPU)\fdf /Fo$(CPU)\fdf.obj fdf.cpp

$(CPU)\chkfile.obj: Makefile chkfile.c chkfile.h
	cl /c $(WARNING_LEVEL) $(OPTIMIZATION) /Fp$(CPU)\chkfile /Fo$(CPU)\chkfile.obj chkfile.c

$(CPU)\lnk.obj: Makefile lnk.c lnk.h ..\include\winstrct.h
	cl /c $(WARNING_LEVEL) $(OPTIMIZATION) /Fplnk /Fo$(CPU)\lnk.obj lnk.c

$(CPU)\xorsum.obj: Makefile xorsum.cpp chkfile.h ..\include\winstrct.h ..\include\winstrct.h ..\include\wfind.h
	cl /c $(WARNING_LEVEL) $(OPTIMIZATION) /Fpxorsum /Fo$(CPU)\xorsum.obj xorsum.cpp

fdf.res: fdf.rc
	rc fdf.rc

clean:
	del $(CPU)\*.obj *~ $(CPU)\*.pch
