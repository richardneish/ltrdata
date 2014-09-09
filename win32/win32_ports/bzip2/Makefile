!IF "$(CPU)" == ""
CPU=$(_BUILDARCH)
!ENDIF

!IF "$(CPU)" == ""
CPU=i386
!ENDIF

!IF "$(CPU)" == "i386"
LIBS=/nodefaultlib:msvcrt.lib /defaultlib:minwcrt.lib
!ENDIF

$(CPU)\bzip2.exe: bzip2.c blocksort.c bzlib.c compress.c crctable.c decompress.c huffman.c randtable.c bzlib.h bzlib_private.h ..\..\lib\minwcrt.lib
	cl /nologo /Fe$(CPU)\bzip2.exe /WX /W3 /O2 /GS-R- /MD bzip2.c blocksort.c bzlib.c compress.c crctable.c decompress.c huffman.c randtable.c /link /opt:ref,icf=10 $(LIBS) /fixed:no
	del *.obj

$(CPU)\bzip2dos.exe: bzip2.c blocksort.c bzlib.c compress.c crctable.c decompress.c huffman.c randtable.c bzlib.h bzlib_private.h ..\..\lib\minwcrt.lib
	cl /nologo /Fe$(CPU)\bzip2dos.exe /WX /W3 /O2 /GS-R- bzip2.c blocksort.c bzlib.c compress.c crctable.c decompress.c huffman.c randtable.c /link /out:bzip2dos.exe /opt:ref,icf=10 /fixed:no

clean:
	del *~ *.obj
