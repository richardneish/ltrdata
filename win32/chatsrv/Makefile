chatsrv.dll: chatsrv.cpp chatsrv.res ..\lib\winstrcp.lib ..\inetd\inetdexp.hpp ..\include\winstrct.h ..\include\wio.h
	cl /WX /W4 /Ox /G7F /LD /MD chatsrv.cpp chatsrv.res /link /opt:ref

install: chatsrv.dll
	xcopy /d/y chatsrv.dll p:\inetd\

clean:
	del *~ *.obj
