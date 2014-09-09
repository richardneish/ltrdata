ritprogr.exe: ritprogr.cpp maliheh.cpp mikael.cpp ritprogr.res
	cl /WX /W4 /Ox /G7F ritprogr.cpp maliheh.cpp mikael.cpp ritprogr.res /link /fixed:no /opt:ref

ritprogr.res: ritprogr.rc
	rc ritprogr.rc

install: p:\utils\ritprogr.exe

p:\utils\ritprogr.exe: ritprogr.exe
	copy /y ritprogr.exe p:\utils\

clean:
	del *~ *.obj
