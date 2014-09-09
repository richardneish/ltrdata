.SUFFIXES : .res .rc

COMPILER=cl /nologo
MAX_OPTIMIZE=/Ox /G7F
MAX_WARNINGS=/WX /W4 /wd4100 /wd4201 /wd4115 /wd4214
CRITICAL_WARNINGS=/WX /W2
HIGH_WARNINGS=/WX /W3
LINK_OPTIONS=/opt:ref,nowin98,icf=10 /release
LINK_DOS_COMPAT=/fixed:no

all: sdonexit.exe tscwrap.exe

sdonexit.exe: sdonexit.cpp sdonexit.res ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD sdonexit.cpp sdonexit.res /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

tscwrap.exe: sdonexit.cpp tscwrap.res ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD /Fetscwrap.exe sdonexit.cpp tscwrap.res /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

