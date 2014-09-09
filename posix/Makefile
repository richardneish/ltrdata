UNAME=Cygwin
UNAME!=uname

.if $(UNAME) == "FreeBSD"
CC_OPTIONS+=-DHAVE_SOCKLEN_T 
.elif $(UNAME) == "Interix"
CC_OPTIONS+=-D_ALL_SOURCE -I/usr/local/include -L/usr/local/lib 
.endif

makeall: $(UNAME)/filosof $(UNAME)/webclient $(UNAME)/gethost $(UNAME)/nyfilosof $(UNAME)/sampleserver $(UNAME)/websrv $(UNAME)/webecho $(UNAME)/popman $(UNAME)/popdump $(UNAME)/sampleserver $(UNAME)/finger $(UNAME)/lowerfn $(UNAME)/lotto $(UNAME)/radera $(UNAME)/unixlogg $(UNAME)/timetest $(UNAME)/fraction $(UNAME)/whois $(UNAME)/genpwds $(UNAME)/slowtx $(UNAME)/whead

$(UNAME)/slowtx: slowtx.c
	gcc $(CC_OPTIONS) -o $(UNAME)/slowtx slowtx.c

$(UNAME)/stripcr: stripcr.c
	gcc $(CC_OPTIONS) -o $(UNAME)/stripcr stripcr.c

$(UNAME)/gissatal: gissatal.c
	gcc $(CC_OPTIONS) -o $(UNAME)/gissatal gissatal.c

$(UNAME)/genpwds: genpwds.c
	gcc $(CC_OPTIONS) -o $(UNAME)/genpwds genpwds.c

$(UNAME)/primtal: primtal.c imathhlp.c
	gcc $(CC_OPTIONS) -o $(UNAME)/primtal primtal.c imathhlp.c

$(UNAME)/fraction: fraction.c imathhlp.c
	gcc $(CC_OPTIONS) -o $(UNAME)/fraction fraction.c imathhlp.c

$(UNAME)/primeser.o: primeser.cpp
	g++ $(CC_OPTIONS) -o $(UNAME)/primeser.o -c primeser.cpp

$(UNAME)/timetest: timetest.c
	gcc $(CC_OPTIONS) -o $(UNAME)/timetest timetest.c

$(UNAME)/unixlogg: unixlogg.c
	gcc $(CC_OPTIONS) -o $(UNAME)/unixlogg unixlogg.c

$(UNAME)/radera: radera.c
	gcc $(CC_OPTIONS) -o $(UNAME)/radera radera.c

$(UNAME)/lotto: lotto.c
	gcc $(CC_OPTIONS) -o $(UNAME)/lotto lotto.c

$(UNAME)/lowerfn: lowerfn.c
	gcc $(CC_OPTIONS) -o $(UNAME)/lowerfn lowerfn.c strlwr.c

$(UNAME)/ftpcp: ftpcp.cpp sockhlp.cpp iohlp.cpp
	g++ $(CC_OPTIONS) -lpthread -o $(UNAME)/ftpcp ftpcp.cpp sockhlp.cpp iohlp.cpp

$(UNAME)/lab4: $(UNAME)/lab4client $(UNAME)/lab4server
$(UNAME)/je: $(UNAME)/je_write $(UNAME)/je_read

$(UNAME)/je_write: $(UNAME)/java_aether.o $(UNAME)/je_write.o
	g++ $(CC_OPTIONS) -o $(UNAME)/je_write $(UNAME)/java_aether.o $(UNAME)/je_write.o

$(UNAME)/je_read: $(UNAME)/java_aether.o $(UNAME)/je_read.o
	g++ $(CC_OPTIONS) -o $(UNAME)/je_read $(UNAME)/java_aether.o $(UNAME)/je_read.o

$(UNAME)/java_aether.o: java_aether.c java_aether.h
	gcc $(CC_OPTIONS) -o $(UNAME)/java_aether.o -c java_aether.c

$(UNAME)/je_write.o: je_write.cpp java_aether.h
	g++ $(CC_OPTIONS) -o $(UNAME)/je_write.o -c je_write.cpp

$(UNAME)/je_read.o: je_read.cpp java_aether.h
	g++ $(CC_OPTIONS) -o $(UNAME)/je_read.o -c je_read.cpp

$(UNAME)/sockhlp.o: sockhlp.cpp
	g++ $(CC_OPTIONS) -o $(UNAME)/sockhlp.o -c sockhlp.cpp

$(UNAME)/iohlp.o: iohlp.cpp
	g++ $(CC_OPTIONS) -o $(UNAME)/iohlp.o -c iohlp.cpp

$(UNAME)/nyfilosof: nyfilosof.cpp
	g++ $(CC_OPTIONS) -o $(UNAME)/nyfilosof nyfilosof.cpp -lpthread

$(UNAME)/filosof: filosof.cpp
	g++ $(CC_OPTIONS) -o $(UNAME)/filosof filosof.cpp

$(UNAME)/finger: finger.c
	gcc $(CC_OPTIONS) -o $(UNAME)/finger finger.c

$(UNAME)/whois: whois.cpp
	g++ $(CC_OPTIONS) -o $(UNAME)/whois whois.cpp

$(UNAME)/popman: popman.cpp iohlp.cpp sockhlp.cpp
	g++ $(CC_OPTIONS) -o $(UNAME)/popman popman.cpp iohlp.cpp sockhlp.cpp

$(UNAME)/popdump: popdump.cpp iohlp.cpp sockhlp.cpp
	g++ $(CC_OPTIONS) -o $(UNAME)/popdump popdump.cpp iohlp.cpp sockhlp.cpp

$(UNAME)/whead: whead.cpp iohlp.cpp sockhlp.cpp
	g++ $(CC_OPTIONS) -o $(UNAME)/whead whead.cpp iohlp.cpp sockhlp.cpp

$(UNAME)/webclient: webclient.cpp
	g++ $(CC_OPTIONS) -o $(UNAME)/webclient webclient.cpp

$(UNAME)/websrv: websrv.cpp
	g++ $(CC_OPTIONS) -o $(UNAME)/websrv websrv.cpp

$(UNAME)/webecho: webecho.cpp
	g++ $(CC_OPTIONS) -o $(UNAME)/webecho webecho.cpp

$(UNAME)/sampleserver: sampleserver.c
	gcc $(CC_OPTIONS) -o $(UNAME)/sampleserver sampleserver.c

$(UNAME)/gethost: gethost.c
	gcc $(CC_OPTIONS) -o $(UNAME)/gethost gethost.c

$(UNAME)/lab4server: lab4server.cpp
	g++ $(CC_OPTIONS) -o $(UNAME)/lab4server lab4server.cpp -lpthread

$(UNAME)/lab4client: lab4client.cpp
	g++ $(CC_OPTIONS) -o $(UNAME)/lab4client lab4client.cpp
