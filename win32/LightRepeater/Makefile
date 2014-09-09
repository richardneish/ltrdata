COMPILER_OPTIONS=/nologo /c /WX /W4 /wd4201 /Ox /G7F /EHsc /MD /YX
LINKER_OPTIONS=/nologo /opt:ref,nowin98,icf=10

MakeAll: LightRepeater.exe lightrep_stat.exe lrevents.cpl

LightRepeater.exe: LightRepeater.obj ntservice.obj lightrep_log.obj LightRepeater.res
	link $(LINKER_OPTIONS) LightRepeater.obj ntservice.obj lightrep_log.obj LightRepeater.res

lightrep_stat.exe: lightrep_stat.obj lightrep_log.obj lightrep_stat.res
	link $(LINKER_OPTIONS) lightrep_stat.obj lightrep_log.obj lightrep_stat.res

lrevents.cpl: lrevents.obj lrevents.res
	link $(LINKER_OPTIONS) /dll /out:lrevents.cpl lrevents.obj lrevents.res

LightRepeater.res: Makefile LightRepeater.rc LightRepeater.ico LightRepeater.rc.h
	rc LightRepeater.rc

lightrep_stat.res: Makefile lightrep_stat.rc lightrep_stat.ico lightrep_stat.rc.h
	rc lightrep_stat.rc

lrevents.res: Makefile lrevents.rc LightRepeater.ico lrevents.rc.h
	rc lrevents.rc

LightRepeater.obj: Makefile LightRepeater.cpp lightrep_log.h ntservice.h
	cl $(COMPILER_OPTIONS) /FpLightRepeater LightRepeater.cpp

ntservice.obj: Makefile ntservice.cpp ntservice.h
	cl $(COMPILER_OPTIONS) /Fpntservice ntservice.cpp

lightrep_log.obj: Makefile lightrep_log.cpp lightrep_log.h
	cl $(COMPILER_OPTIONS) /Fplightrep_log lightrep_log.cpp

lightrep_stat.obj: Makefile lightrep_stat.cpp lightrep_log.h
	cl $(COMPILER_OPTIONS) /Fplightrep_stat lightrep_stat.cpp

lrevents.obj: Makefile lrevents.cpp lrevents.rc.h
	cl $(COMPILER_OPTIONS) /Fplrevents lrevents.cpp
