all: ws2_32.dll mswsock.dll

ws2_32.dll: ws2_32.def
	link /noentry /out:ws2_32.dll /dll /machine:x86 /def:ws2_32.def

mswsock.dll: mswsock.def
	link /noentry /out:mswsock.dll /dll /machine:x86 /def:mswsock.def
