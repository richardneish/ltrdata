!IF "$(CPU)" == ""
CPU=$(_BUILDARCH)
!ENDIF

!IF "$(CPU)" == ""
CPU=i386
!ENDIF

!IF "$(CPU)" == "i386"
STDIRARCH=$(STDIR32)
!IF "$(TARGETDIR)" == ""
TARGETDIR=$(MAKEDIR)
!ENDIF
PUBLISHDIR=$(PUBLISHDIR)
!ELSEIF "$(CPU)" == "AMD64"
STDIRARCH=$(STDIR64)
!IF "$(TARGETDIR)" == ""
TARGETDIR=$(MAKEDIR)64
!ENDIF
PUBLISHDIR=$(PUBLISHDIR)\win64
!ELSE
!ERROR Unsupported machine architecture: $(CPU)
!ENDIF

.SUFFIXES : .res .rc

.SUFFIXES : .res .src

COMPILER=cl /nologo
!IF "$(CPU)" == "i386"
MAX_OPTIMIZE=/O2 /G7 /GR-
!ELSE
MAX_OPTIMIZE=/O2 /GS- /GR-
!ENDIF
MAX_WARNINGS=/WX /W4 /wd4100 /wd4201 /wd4115 /wd4214
CRITICAL_WARNINGS=/WX /W2
HIGH_WARNINGS=/WX /W3
LINK_OPTIONS=/opt:nowin98,ref,icf=10 /largeaddressaware /release /nologo
!IF "$(CPU)" == "i386"
LINK_WIN2K_COMPAT=/defaultlib:bufferoverflowU.lib
LINK_DOS_COMPAT=/fixed:no
!ELSE
LINK_OPTIONS=/defaultlib:bufferoverflowU.lib $(LINK_OPTIONS)
!ENDIF

compileandlink: changedir winmgmt process fileutil utils cxr sockapp simul libs testapps win32_ports win32_only

publish: changedir compileandlink signall updatetargets $(PUBLISHDIR)\exefiles.zip $(PUBLISHDIR)\exefiles.7z $(PUBLISHDIR)\source.7z $(STDIRARCH)\exefiles.sa.lzma

updatetargets: changedir
	updsource.cmd
	updutils.cmd

changedir:
	cd $(TARGETDIR)

signall:
	signall.cmd

$(STDIRARCH)\exefiles.zip: *.exe *.dll *.txt
	exefiles.cmd

$(STDIRARCH)\exefiles.7z: *.exe *.dll *.txt
	exefiles7z.cmd

$(STDIRARCH)\exefiles.sa.lzma: *.exe *.dll *.txt
	$(STDIR32)\strarc -cfls:s -d:$(STDIRARCH)\ "-z:lzma e -si -so" $(STDIRARCH)\exefiles.sa.lzma < updated.txt

$(STDIRARCH)\source.7z: *.exe *.dll *.txt
	updsourcetar.cmd

$(PUBLISHDIR)\exefiles.7z: $(STDIRARCH)\exefiles.7z
	copy /y $(STDIRARCH)\exefiles.7z $(PUBLISHDIR)\

$(PUBLISHDIR)\exefiles.zip: $(STDIRARCH)\exefiles.zip
	copy /y $(STDIRARCH)\exefiles.zip $(PUBLISHDIR)\

$(PUBLISHDIR)\source.7z: $(STDIRARCH)\source.7z
	copy /y $(STDIRARCH)\source.7z $(PUBLISHDIR)\

clean:
	..\clbcc.cmd

winmgmt: showwin.exe enumwin.exe denywin.exe keepmax.exe onshutdown.exe allowsetfg.exe keylock.exe
process: cpretpid.exe runwait.exe runwaitw.exe kill.exe killthrd.exe pdhobj.exe logonwrap.exe shlexec.exe logout.exe rdown.exe reboot.exe sysinf.exe calldll.exe calldllw.exe w32ver.exe w32verc.exe msgbox.exe msgboxw.exe importini.exe setprio.exe finddll.exe psmod.exe enumps.exe noexcept.exe noexcept.dll
fileutil: cut32.exe char32.exe fileopen.exe radera.exe reptxt32.exe validate.exe shortren.exe ntrm.exe ntmd.exe ntrmkey.exe dosdev.exe exelist.exe rawcopy.exe trunczerotail.exe xcopynt.exe chsize32.exe sizdir32.exe sizeof.exe txtcnv32.exe chr7filter.exe rot13.exe movent.exe newest32.exe devioctl.exe umount.exe copyacls.exe fsplit32.exe fileinfo.exe windir.exe lowerfn.exe tracef32.exe fwpipes.exe nameansi2oem.exe junc.exe fdf.exe strarc.exe strarc.txt xorsum.exe hardlink.exe lzma.exe copyft.exe fsplitw.exe dos2nt.exe ntname.exe lockdrv.exe roundft.exe geteltorito.exe extrbin.exe undecorateurl.exe undecoratefilename.exe fixjuncs.exe
utils: synctime.exe personnr.exe tickcount.exe tickcntw.exe playwavs.exe getuser.exe fraction.exe primtal.exe dot2hex.exe sleep.exe wavplay.exe veckodag.exe getoemcp.exe getacp.exe joindomain.exe getkey.exe genpwds.exe errmsg.exe username.exe extrtext32.exe readclp.exe writeclp.exe lookupacc.exe viewenv.exe expandenv.exe assd.exe searchtreeforfile.exe strmchr.exe stripcr.exe unsecwinsta.exe regrepl.exe bzip2.exe enum.exe cerrmsg.exe winlogoncfg.exe privproc.exe waitpid.exe regdump.exe hexstr.exe swapadd.exe lrc.exe enumbatt.exe
cxr: cxr.exe cxrbackup.exe cxrrestore.exe countchars.exe
sockapp: webclient.exe popman.exe popdump.exe finger.exe gethost.exe gethostw.exe ftpdll32.dll chkwsdll.exe ftpcp.exe txfile.exe webechod.dll getftp.exe enumproto.exe tcpfw.exe whead.exe
simul: filosof.exe nyfilosof.exe lotto.exe gissatal.exe
libs: simpapi.dll tcpconn.dll
testapps: fmtest.exe first.exe 10sek.exe aaofiltr.exe readline.exe mpgsplit.exe event.exe
win32_ports: whois.exe cut.exe factor.exe primes.exe bzip2.exe bunzip2.exe bzcat.exe
!IF "$(CPU)" == "i386"
win32_only: asuser.exe conterm.exe nbnbs.exe sdonexit.exe tscwrap.exe
!ELSE
win32_only:
!ENDIF

bzip2.exe bunzip2.exe bzcat.exe: ..\win32_ports\bzip2\$(CPU)\bzip2.exe
	$(STDIR32)\hardlink -f ..\win32_ports\bzip2\$(CPU)\bzip2.exe bzip2.exe bunzip2.exe bzcat.exe

..\win32_ports\bzip2\$(CPU)\bzip2.exe:
	cd ..\win32_ports\bzip2
	nmake /nologo /$(MAKEFLAGS)
	cd $(TARGETDIR)

winlogoncfg.exe: ..\winlogoncfg\$(CPU)\winlogoncfg.exe
	$(STDIR32)\hardlink -f ..\winlogoncfg\$(CPU)\winlogoncfg.exe winlogoncfg.exe

..\winlogoncfg\$(CPU)\winlogoncfg.exe:
	cd ..\winlogoncfg
	nmake /nologo /$(MAKEFLAGS)
	cd $(TARGETDIR)

enum.exe: ..\enum\$(CPU)\enum.exe
	$(STDIR32)\hardlink -f ..\enum\$(CPU)\enum.exe enum.exe

..\enum\$(CPU)\enum.exe: ..\enum\Makefile ..\enum\enum.cpp
	cd ..\enum
	nmake /nologo /$(MAKEFLAGS)
	cd $(TARGETDIR)

fsplitw.exe: ..\fsplitw\$(CPU)\fsplitw.exe
	$(STDIR32)\hardlink -f ..\fsplitw\$(CPU)\fsplitw.exe fsplitw.exe

..\fsplitw\$(CPU)\fsplitw.exe: ..\fsplitw\Makefile ..\fsplitw\fsplitw.cpp
	cd ..\fsplitw
	nmake /nologo /$(MAKEFLAGS)
	cd $(TARGETDIR)

regrepl.exe: ..\regrepl\$(CPU)\regrepl.exe
	$(STDIR32)\hardlink -f ..\regrepl\$(CPU)\regrepl.exe regrepl.exe

..\regrepl\$(CPU)\regrepl.exe:
	cd ..\regrepl
	nmake /nologo /$(MAKEFLAGS)
	cd $(TARGETDIR)

lzma.exe: ..\lzma406\SRC\7zip\Compress\LZMA_Alone\$(CPU)\lzma.exe
	$(STDIR32)\hardlink -f ..\lzma406\SRC\7zip\Compress\LZMA_Alone\$(CPU)\lzma.exe lzma.exe

..\lzma406\SRC\7zip\Compress\LZMA_Alone\$(CPU)\lzma.exe:
	cd ..\lzma406\SRC\7zip\Compress\LZMA_Alone
	nmake /nologo /$(MAKEFLAGS)
	cd $(TARGETDIR)

noexcept.exe: ..\noexcept\$(CPU)\noexcept.exe
	$(STDIR32)\hardlink -f ..\noexcept\$(CPU)\noexcept.exe noexcept.exe

..\noexcept\$(CPU)\noexcept.exe: ..\noexcept\noexcept.c
	cd ..\noexcept
	nmake /nologo /$(MAKEFLAGS)
	cd $(TARGETDIR)

noexcept.dll: ..\noexcept\$(CPU)\noexcept.dll
	$(STDIR32)\hardlink -f ..\noexcept\$(CPU)\noexcept.dll noexcept.dll

..\noexcept\$(CPU)\noexcept.dll: ..\noexcept\noexceptdll.c
	cd ..\noexcept
	nmake /nologo /$(MAKEFLAGS)
	cd $(TARGETDIR)

strarc.exe: ..\strarc\$(CPU)\strarc.exe
	$(STDIR32)\hardlink -f ..\strarc\$(CPU)\strarc.exe strarc.exe

..\strarc\$(CPU)\strarc.exe:
	cd ..\strarc
	nmake /nologo /$(MAKEFLAGS)
	cd $(TARGETDIR)

strarc.txt: ..\strarc\strarc.txt
	$(STDIR32)\hardlink -f ..\strarc\strarc.txt strarc.txt

fdf.exe: ..\fdf\$(CPU)\fdf.exe
	$(STDIR32)\hardlink -f ..\fdf\$(CPU)\fdf.exe fdf.exe

..\fdf\$(CPU)\fdf.exe: ..\fdf\fdf.cpp ..\fdf\Makefile
	cd ..\fdf
	nmake /nologo /$(MAKEFLAGS)
	cd $(TARGETDIR)

xorsum.exe: ..\fdf\$(CPU)\xorsum.exe
	$(STDIR32)\hardlink -f ..\fdf\$(CPU)\xorsum.exe xorsum.exe

..\fdf\$(CPU)\xorsum.exe:
	cd ..\fdf
	nmake /nologo /$(MAKEFLAGS)
	cd $(TARGETDIR)

nbnbs.exe: ..\win32_ports\nbnbs\nbnbs.exe
	$(STDIR32)\hardlink -f ..\win32_ports\nbnbs\nbnbs.exe nbnbs.exe

..\win32_ports\nbnbs\nbnbs.exe:
	cd ..\win32_ports\nbnbs
	nmake /nologo /$(MAKEFLAGS)
	cd $(TARGETDIR)

asuser.exe: ..\inetd\asuser.exe
	$(STDIR32)\hardlink -f ..\inetd\asuser.exe asuser.exe

..\inetd\asuser.exe:
	cd ..\inetd
	nmake /nologo /$(MAKEFLAGS)
	cd $(TARGETDIR)

conterm.exe: ..\inetd\conterm.exe
	$(STDIR32)\hardlink -f ..\inetd\conterm.exe conterm.exe

..\inetd\conterm.exe:
	cd ..\inetd
	nmake /nologo /$(MAKEFLAGS)
	cd $(TARGETDIR)

copyft.exe: ..\c\copyft.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\copyft.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

enumproto.exe: ..\c\enumproto.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\enumproto.c /link $(LINK_WIN2K_COMPAT) $(LINK_OPTIONS)

tcpfw.exe: ..\cpp\tcpfw.cpp ..\lib\winstrcp.lib ..\include\tcproute.hpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\tcpfw.cpp /link $(LINK_WIN2K_COMPAT) $(LINK_OPTIONS)

whead.exe: ..\cpp\whead.cpp ..\lib\winstrcp.lib
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\whead.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

undecorateurl.exe: ..\cpp\undecorateurl.cpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\undecorateurl.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

undecoratefilename.exe: ..\cpp\undecoratefilename.cpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\undecoratefilename.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

unsecwinsta.exe: ..\cpp\unsecwinsta.cpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\unsecwinsta.cpp /link $(LINK_OPTIONS)

junc.exe: ..\c\junc.c ..\lib\winstrct.lib
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\junc.c /link $(LINK_OPTIONS)

dos2nt.exe: ..\c\dos2nt.c ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\dos2nt.c /link $(LINK_OPTIONS)

ntname.exe: ..\cpp\ntname.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\ntname.cpp /link $(LINK_OPTIONS)

lockdrv.exe: ..\cpp\lockdrv.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\lockdrv.cpp /link $(LINK_OPTIONS)

roundft.exe: ..\c\roundft.c ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\roundft.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

stripcr.exe: ..\c\stripcr.c ..\lib\winstrct.lib
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\stripcr.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

lrc.exe: ..\cpp\lrc.cpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\lrc.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

enumbatt.exe: ..\cpp\enumbatt.cpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\enumbatt.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

txfile.exe: ..\c\txfile.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\txfile.c /link $(LINK_OPTIONS)

nameansi2oem.exe: ..\c\nameansi2oem.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\nameansi2oem.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

psmod.exe: ..\c\psmod.c ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\psmod.c /link $(LINK_OPTIONS)

enumps.exe: ..\c\enumps.c ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\enumps.c /link $(LINK_OPTIONS)

fwpipes.exe: ..\c\fwpipes.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\fwpipes.c /link $(LINK_OPTIONS)

whois.exe: ..\win32_ports\whois\whois.c ..\c\getopt.c ..\lib\winstrcp.lib
	$(COMPILER) $(HIGH_WARNINGS) /DHAVE_GETOPT_LONG /DUSE_MINWCRT $(MAX_OPTIMIZE) /MD ..\win32_ports\whois\whois.c ..\c\getopt.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

cut.exe: ..\win32_ports\cut\cut.c ..\c\getopt.c ..\c\err.c ..\c\strsep.c
	$(COMPILER) $(HIGH_WARNINGS) /wd4005 $(MAX_OPTIMIZE) /DUSE_MINWCRT /MD ..\win32_ports\cut\cut.c ..\c\getopt.c ..\c\err.c ..\c\strsep.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

factor.exe: ..\win32_ports\factor\factor.c ..\win32_ports\factor\pr_tbl.c ..\c\getopt.c ..\c\err.c
	$(COMPILER) $(HIGH_WARNINGS) /wd4005 $(MAX_OPTIMIZE) /DUSE_MINWCRT /MD ..\win32_ports\factor\factor.c ..\win32_ports\factor\pr_tbl.c ..\c\getopt.c ..\c\err.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

primes.exe: ..\win32_ports\factor\primes.c ..\win32_ports\factor\pr_tbl.c ..\win32_ports\factor\pattern.c ..\c\getopt.c ..\c\err.c
	$(COMPILER) $(HIGH_WARNINGS) /wd4005 $(MAX_OPTIMIZE) /DUSE_MINWCRT /MD ..\win32_ports\factor\primes.c ..\win32_ports\factor\pr_tbl.c ..\win32_ports\factor\pattern.c ..\c\getopt.c ..\c\err.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

lowerfn.exe: ..\c\lowerfn.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\lowerfn.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

simpapi.dll simpapi.exp simpapi.lib: ..\c\simpapi.c
	$(COMPILER) $(HIGH_WARNINGS) $(MAX_OPTIMIZE) /MD /LD ..\c\simpapi.c /link $(LINK_OPTIONS) /noentry

tcpconn.dll tcpconn.exp tcpconn.lib: ..\cpp\tcpconn.cpp
	$(COMPILER) $(HIGH_WARNINGS) $(MAX_OPTIMIZE) /MD /LD ..\cpp\tcpconn.cpp /link $(LINK_OPTIONS) /noentry

expandenv.exe: ..\cpp\expandenv.cpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\expandenv.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

10sek.exe: ..\c\10sek.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\10sek.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

aaofiltr.exe: ..\c\aaofiltr.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\aaofiltr.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

readline.exe: ..\cpp\readline.cpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\readline.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

mpgsplit.exe: ..\cpp\mpgsplit.cpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\mpgsplit.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

event.exe: ..\cpp\event.cpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\event.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

viewenv.exe: ..\cpp\viewenv.cpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\viewenv.cpp /link $(LINK_WIN2K_COMPAT) $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

tracef32.exe: ..\cpp\tracef32.cpp ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\tracef32.cpp /link $(LINK_WIN2K_COMPAT) $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

geteltorito.exe: ..\cpp\geteltorito.cpp ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\geteltorito.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

windir.exe: ..\c\windir.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\windir.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

lookupacc.exe: ..\cpp\lookupacc.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\lookupacc.cpp /link $(LINK_OPTIONS)

first.exe: ..\c\first.c ..\c\first.res
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\first.c ..\c\first.res /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

..\c\first.res: ..\c\first.rc ..\c\first.rle
	rc ..\c\first.rc

getftp.exe: ..\cpp\getftp.cpp
	$(COMPILER) $(HIGH_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\getftp.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

shortren.exe: ..\c\shortren.c ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\shortren.c /link $(LINK_OPTIONS)

privproc.exe: ..\cpp\privproc.cpp ..\include\winstrct.h ..\include\winstrct.hpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\privproc.cpp /link $(LINK_OPTIONS)

waitpid.exe: ..\c\waitpid.c ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\waitpid.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

regdump.exe: ..\cpp\regdump.cpp ..\include\winstrct.h ..\include\winstrct.hpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\regdump.cpp /link $(LINK_OPTIONS)

fixjuncs.exe: ..\cpp\fixjuncs.cpp ..\include\winstrct.h ..\include\ntfileio.hpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\fixjuncs.cpp /link $(LINK_OPTIONS)

hexstr.exe: ..\c\hexstr.c ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\hexstr.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

ntmd.exe: ..\c\ntmd.c ..\include\winstrct.h ..\lib\winstrct.lib
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\ntmd.c /link $(LINK_OPTIONS)

ntrm.exe: ..\c\ntrm.c ..\include\winstrct.h ..\lib\winstrct.lib
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\ntrm.c /link $(LINK_OPTIONS)

ntrmkey.exe: ..\c\ntrmkey.c ..\include\winstrct.h ..\lib\winstrct.lib
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\ntrmkey.c /link $(LINK_OPTIONS)

dosdev.exe: ..\cpp\dosdev.cpp ..\cpp\dosdev.res ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\dosdev.cpp ..\cpp\dosdev.res /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

..\cpp\dosdev.res: ..\cpp\dosdev.rc
	rc ..\cpp\dosdev.rc

exelist.exe: ..\cpp\exelist.cpp ..\cpp\exelist.res ..\include\winstrct.h ..\include\wfilever.h ..\include\wimghlp.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\exelist.cpp ..\cpp\exelist.res /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

..\cpp\exelist.res: ..\cpp\exelist.rc
	rc ..\cpp\exelist.rc

rawcopy.exe: ..\cpp\rawcopy.cpp ..\cpp\rawcopy.res ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\rawcopy.cpp ..\cpp\rawcopy.res /link $(LINK_WIN2K_COMPAT) $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

..\cpp\rawcopy.res: ..\cpp\rawcopy.rc
	rc ..\cpp\rawcopy.rc

trunczerotail.exe: ..\cpp\trunczerotail.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\trunczerotail.cpp /link $(LINK_WIN2K_COMPAT) $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

xcopynt.exe: ..\cpp\xcopynt.cpp ..\lib\winstrct.lib ..\include\winstrct.h ..\include\wconsole.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\xcopynt.cpp /link $(LINK_OPTIONS)

sdonexit.exe: ..\sdonexit\sdonexit.exe
	$(STDIR32)\hardlink -f ..\sdonexit\sdonexit.exe sdonexit.exe

..\sdonexit\sdonexit.exe:
	cd ..\sdonexit
	nmake /nologo /$(MAKEFLAGS)
	cd $(TARGETDIR)

tscwrap.exe: ..\sdonexit\tscwrap.exe
	$(STDIR32)\hardlink -f ..\sdonexit\tscwrap.exe tscwrap.exe

..\sdonexit\tscwrap.exe:
	cd ..\sdonexit
	nmake /nologo /$(MAKEFLAGS)
	cd $(TARGETDIR)

logonwrap.exe: ..\c\logonwrap.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\logonwrap.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

shlexec.exe: ..\c\shlexec.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\shlexec.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

webclient.exe: ..\cpp\webclient.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\webclient.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

logout.exe: ..\c\logout.c ..\lib\winstrct.lib ..\include\winstrct.h ..\cpp\logout.res
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\logout.c ..\cpp\logout.res /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

..\cpp\logout.res: ..\cpp\logout.rc ..\cpp\logout.ico
	rc ..\cpp\logout.rc

popman.exe: ..\cpp\popman.cpp ..\cpp\popman.res ..\lib\winstrcp.lib ..\include\winstrct.h ..\include\wio.h ..\include\wconsole.h ..\include\wsocket.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\popman.cpp ..\cpp\popman.res /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

popdump.exe: ..\cpp\popdump.cpp ..\cpp\popdump.res ..\lib\winstrcp.lib ..\include\winstrct.h ..\include\wio.h ..\include\wsocket.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\popdump.cpp ..\cpp\popdump.res /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

gethost.exe: ..\c\gethost.c ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\gethost.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

gethostw.exe: ..\cpp\gethostw.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\gethostw.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

chsize32.exe: ..\c\chsize32.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\chsize32.c /link $(LINK_WIN2K_COMPAT) $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

sizdir32.exe: ..\cpp\sizdir32.cpp ..\lib\winstrct.lib ..\include\winstrct.h ..\include\wconsole.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\sizdir32.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

sizeof.exe: ..\cpp\sizeof.cpp ..\lib\winstrct.lib ..\include\winstrct.h ..\include\wconsole.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\sizeof.cpp /link $(LINK_WIN2K_COMPAT) $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

txtcnv32.exe: ..\cpp\txtcnv32.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\txtcnv32.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

rot13.exe: ..\c\rot13.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\rot13.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

rdown.exe: ..\c\shutdown.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD /DREBOOT_FLAG=FALSE /Fordown.obj /Ferdown.exe ..\c\shutdown.c /link $(LINK_OPTIONS)

reboot.exe: ..\c\shutdown.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD /DREBOOT_FLAG=TRUE /Foreboot.obj /Fereboot.exe ..\c\shutdown.c /link $(LINK_OPTIONS)

synctime.exe: ..\cpp\synctime.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\synctime.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

movent.exe: ..\c\movent.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\movent.c /link $(LINK_OPTIONS)

calldll.exe: ..\c\calldll.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\calldll.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

calldllw.exe: ..\c\calldllw.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\calldllw.c /link $(LINK_OPTIONS)

kill.exe: ..\cpp\kill.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\kill.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

killthrd.exe: ..\c\killthrd.c ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\killthrd.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

w32verc.exe: ..\c\w32verc.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\w32verc.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

w32ver.exe: ..\c\w32ver.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\w32ver.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

msgbox.exe: ..\c\msgbox.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\msgbox.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

msgboxw.exe: ..\c\msgboxw.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\msgboxw.c /link $(LINK_OPTIONS)

playwavs.exe: ..\c\playwavs.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\playwavs.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

chr7filter.exe: ..\c\chr7filter.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\chr7filter.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

importini.exe: ..\cpp\importini.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\importini.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

tickcount.exe: ..\c\tickcount.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\tickcount.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

tickcntw.exe: ..\c\tickcntw.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\tickcntw.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

dot2hex.exe: ..\c\dot2hex.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\dot2hex.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

enumwin.exe: ..\c\enumwin.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\enumwin.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

fmtest.exe: ..\cpp\fmtest.cpp ..\include\winstrct.h ..\include\wfilemap.h ..\include\wconsole.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\fmtest.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

readclp.exe: ..\c\readclp.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\readclp.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

writeclp.exe: ..\c\writeclp.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\writeclp.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

showwin.exe: ..\c\showwin.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\showwin.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

keepmax.exe: ..\cpp\keepmax.cpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\keepmax.cpp /link $(LINK_WIN2K_COMPAT) $(LINK_OPTIONS)

keylock.exe: ..\cpp\keylock.cpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\keylock.cpp /link $(LINK_OPTIONS)

onshutdown.exe: ..\cpp\onshutdown.cpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\onshutdown.cpp /link $(LINK_WIN2K_COMPAT) $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

allowsetfg.exe: ..\c\allowsetfg.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\allowsetfg.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

newest32.exe: ..\cpp\newest32.cpp ..\include\wfind.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\newest32.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

devioctl.exe: ..\c\devioctl.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\devioctl.c /link $(LINK_WIN2K_COMPAT) $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

umount.exe: ..\c\umount.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\umount.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

hardlink.exe: ..\cpp\hardlink.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\hardlink.cpp /link $(LINK_OPTIONS)

swapadd.exe: ..\swapadd\swapadd.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\swapadd\swapadd.cpp /link $(LINK_OPTIONS)

copyacls.exe: ..\cpp\copyacls.cpp ..\include\winstrct.h ..\include\wfind.h ..\include\wntsecur.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\copyacls.cpp /link $(LINK_OPTIONS)

wavplay.exe: ..\c\wavplay.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\wavplay.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

veckodag.exe: ..\cpp\veckodag.cpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\veckodag.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

pdhobj.exe: ..\cpp\pdhobj.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\pdhobj.cpp /link $(LINK_OPTIONS)

cut32.exe: ..\c\cut.c
	$(COMPILER) $(MAX_WARNINGS) /Fecut32.exe $(MAX_OPTIMIZE) /MD ..\c\cut.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

char32.exe: ..\c\char.c
	$(COMPILER) $(MAX_WARNINGS) /Fechar32.exe $(MAX_OPTIMIZE) /MD ..\c\char.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

fileopen.exe: ..\cpp\fileopen.cpp
	$(COMPILER) $(MAX_WARNINGS) /Fefileopen.exe $(MAX_OPTIMIZE) /MD ..\cpp\fileopen.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

reptxt32.exe: ..\cpp\reptxt32.cpp ..\lib\winstrct.lib ..\include\wstring.h ..\include\winstrct.h ..\include\wconsole.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\reptxt32.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

validate.exe: ..\c\validate.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\validate.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

getuser.exe: ..\c\getuser.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\getuser.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

primtal.exe: ..\c\primtal.c ..\lib\winstrct.lib ..\include\wimath.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\primtal.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

fraction.exe: ..\c\fraction.c ..\lib\winstrct.lib ..\include\wimath.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\fraction.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

sysinf.exe: ..\c\sysinf.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\sysinf.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

sleep.exe: ..\c\sleep.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\sleep.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

cpretpid.exe: ..\c\cpretpid.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\cpretpid.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

runwait.exe: ..\c\runwait.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\runwait.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

runwaitw.exe: ..\c\runwaitw.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\runwaitw.c /link $(LINK_OPTIONS) $(LINK_WIN2K_COMPAT)

finger.exe: ..\c\finger.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\finger.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

personnr.exe: ..\c\personnr.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\personnr.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

radera.exe: ..\c\radera.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\radera.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

filosof.exe: ..\cpp\filosof.cpp ..\include\wsafenew.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\filosof.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

nyfilosof.exe: ..\cpp\nyfilosof.cpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\nyfilosof.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

cxr.exe: ..\cpp\cxr.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\cxr.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

cxrbackup.exe: ..\cpp\cxrbackup.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\cxrbackup.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

cxrrestore.exe: ..\cpp\cxrrestore.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\cxrrestore.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

countchars.exe: ..\cpp\countchars.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\countchars.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

denywin.exe: ..\c\denywin.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\denywin.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

getoemcp.exe: ..\c\getoemcp.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\getoemcp.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

getacp.exe: ..\c\getacp.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\getacp.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

setprio.exe: ..\c\setprio.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\setprio.c /link $(LINK_WIN2K_COMPAT) $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

finddll.exe: ..\c\finddll.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\finddll.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

joindomain.exe: ..\c\joindomain.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\joindomain.c /link $(LINK_OPTIONS)

getkey.exe: ..\c\getkey.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\getkey.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

ftpdll32.dll ftpdll32.exp ftpdll32.lib: ..\cpp\ftpdll32.cpp ..\cpp\ftpdll32.def
	$(COMPILER) $(HIGH_WARNINGS) $(MAX_OPTIMIZE) /LD /MD ..\cpp\ftpdll32.cpp ..\cpp\ftpdll32.def /link $(LINK_OPTIONS) /noentry

genpwds.exe: ..\c\genpwds.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\genpwds.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

lotto.exe: ..\c\lotto.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\lotto.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

gissatal.exe: ..\c\gissatal.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\gissatal.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

ftpcp.exe: ..\cpp\ftpcp.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\ftpcp.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

extrbin.exe: ..\cpp\extrbin.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\extrbin.cpp /link $(LINK_WIN2K_COMPAT) $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

fsplit32.exe: ..\c\fsplit32.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\fsplit32.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

errmsg.exe: ..\c\errmsg.c ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\errmsg.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

cerrmsg.exe: ..\c\cerrmsg.c
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\cerrmsg.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

username.exe: ..\c\username.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\username.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

webechod.dll webechod.exp webechod.lib: ..\cpp\webechod.cpp ..\lib\winstrct.lib ..\include\winstrct.h ..\include\wio.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /LD /MD ..\cpp\webechod.cpp /link $(LINK_OPTIONS)

extrtext32.exe: ..\c\extrtext32.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\extrtext32.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

fileinfo.exe: ..\cpp\fileinfo.cpp ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\fileinfo.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

assd.exe: ..\cpp\assd.cpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\assd.cpp /link $(LINK_OPTIONS)

boatreg.exe: ..\cpp\boatreg.cpp ..\cpp\boatregclass.cpp ..\cpp\boat.cpp
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\cpp\boatreg.cpp ..\cpp\boatregclass.cpp ..\cpp\boat.cpp /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

whoami.exe: ..\c\whoami.c ..\lib\winstrct.lib ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\whoami.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

chkwsdll.exe: ..\c\chkwsdll.c ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\chkwsdll.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

searchtreeforfile.exe: ..\c\searchtreeforfile.c ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\searchtreeforfile.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)

strmchr.exe: ..\c\strmchr.c ..\include\winstrct.h
	$(COMPILER) $(MAX_WARNINGS) $(MAX_OPTIMIZE) /MD ..\c\strmchr.c /link $(LINK_OPTIONS) $(LINK_DOS_COMPAT)
