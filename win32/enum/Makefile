!IF "$(CPU)" == ""
CPU=$(_BUILDARCH)
!ENDIF

!IF "$(CPU)" == ""
CPU=i386
!ENDIF

WARNING_LEVEL=/nologo /WX /W4 /wd4214 /wd4201 /wd4206
!IF "$(CPU)" == "i386"
CPPFLAGS=/O2 /GF /G7 /GR- /MD
!ELSE
CPPFLAGS=/O2 /GFS- /GR- /MD
!ENDIF
LINK_SWITCHES=/nologo /defaultlib:bufferoverflowU.lib /release /opt:nowin98,ref,icf=10 /fixed:no /release

$(CPU)\enum.exe: $(CPU)\enum.obj $(CPU)\getopt.obj Makefile
	link /opt:ref,icf=10 /fixed:no /out:$(CPU)\enum.exe $(CPU)\enum.obj $(CPU)\getopt.obj

$(CPU)\enum.obj: enum.cpp getopt.h Makefile
	cl $(CPPFLAGS) /c /Fp$(CPU)\enum /Fo$(CPU)\enum.obj enum.cpp

$(CPU)\getopt.obj: getopt.cpp getopt.h Makefile
	cl $(CPPFLAGS) /c /Fp$(CPU)\getopt /Fo$(CPU)\getopt.obj getopt.cpp

clean:
	del $(CPU)\*.obj $(CPU)\*.pch *~
