!IF "$(CPU)" == ""
CPU=$(_BUILDARCH)
!ENDIF

!IF "$(CPU)" == ""
CPU=i386
!ENDIF

!IF "$(CPU)" == "i386"
MAX_OPTIMIZE=/O2 /G7 /GR-
!ELSE
MAX_OPTIMIZE=/O2 /GS- /GR-
!ENDIF

MAX_WARNINGS=/WX /W4 /wd4100 /wd4201 /wd4115 /wd4214

CFLAGS=$(MAX_OPTIMIZE) $(MAX_WARNING) /MD

LINK_OPTIONS=/defaultlib:bufferoverflowU.lib /opt:nowin98,ref,icf=10 /largeaddressaware /release /nologo

LIB=$(LIB);$(CPU)

all: $(CPU)\noexcept.exe $(CPU)\noexcept.dll

$(CPU)\noexcept.exe: $(CPU)\noexcept.obj $(CPU)\noexcept.lib
	link /out:$(CPU)\noexcept.exe $(LINK_OPTIONS) $(CPU)\noexcept.obj

$(CPU)\noexcept.obj: noexcept.c ..\include\winstrct.h
	cl /c /Fp$(CPU)\noexcept.pch /Fo$(CPU)\noexcept.obj $(CFLAGS) noexcept.c

$(CPU)\noexcept.lib: $(CPU)\noexcept.dll

$(CPU)\noexcept.dll: $(CPU)\noexceptdll.obj
	link /out:$(CPU)\noexcept.dll /entry:DllMain /dll $(LINK_OPTIONS) $(CPU)\noexceptdll.obj

$(CPU)\noexceptdll.obj: noexceptdll.c
	cl /c /Fp$(CPU)\noexceptdll.pch /Fo$(CPU)\noexceptdll.obj $(CFLAGS) noexceptdll.c

test.exe: test.c
	cl /WX /W3 /Ox /GFS- /GR- /MD test.c /link /opt:ref,icf=10

clean:
	del *~ *.obj *.pch
