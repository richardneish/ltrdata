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

$(CPU)\fsplitw.exe: Makefile $(CPU)\fsplitw.obj fsplitw.res fsplitw.rc.h ..\lib\minwcrt.lib
	link $(LINK_SWITCHES) /out:$(CPU)\fsplitw.exe $(CPU)\fsplitw.obj fsplitw.res

$(CPU)\fsplitw.obj: Makefile fsplitw.cpp fsplitw.rc.h ..\include\winstrct.h
	cl /nologo /c $(OPTIMIZATION) /Fp$(CPU)\fsplitw /Fo$(CPU)\fsplitw.obj /MD fsplitw.cpp

fsplitw.res: fsplitw.rc fsplitw.rc.h fsplitw.ico
	rc fsplitw.rc

clean:
	del *~ $(CPU)\*.obj $(CPU)\*.pch *.res
