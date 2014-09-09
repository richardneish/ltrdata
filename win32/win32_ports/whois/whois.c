/* Copyright 1999-2003 by Marco d'Itri <md@linux.it>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/* System library */
#if defined(_WIN64) && defined(USE_MINWCRT)
#undef USE_MINWCRT
#endif
#include <stdio.h>
#ifdef USE_MINWCRT
#include <minwcrtlib.h>
#else
#include <stdlib.h>
#endif
#include <stdarg.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include "config.h"
#ifdef HAVE_GETOPT_LONG
#include <getopt.h>
#endif
#ifdef HAVE_REGEXEC
#include <regex.h>
#endif
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif
#include <errno.h>
#include <signal.h>

/********  BEGIN WIN32 */

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock.h>
#include <winstrct.h>
#include <strsafe.h>
#pragma warning(disable:4995)

#define strncasecmp strnicmp
#define snprintf StringCbPrintf
#define alarm(n)

#pragma comment(lib, "user32")
#pragma comment(lib, "wsock32")
#pragma comment(lib, "winstrcp")
#ifdef USE_MINWCRT
#pragma comment(lib, "minwcrt")
#endif

#else

typedef int HANDLE;
#define h_perror(m) perror(m)

#endif

/*************  END WIN32  */

/* Application-specific */
#include "data.h"
#include "whois.h"

#ifdef _WIN32
#include <wio.h>
#else
#include "iohlp.h"
#endif

/* Global variables */
SOCKET sockfd, verb = 0;

#ifdef ALWAYS_HIDE_DISCL
int hide_discl = 0;
#else
int hide_discl = 2;
#endif

char *client_tag = (char *)IDSTRING;

#ifdef HAVE_GETOPT_LONG
static struct option longopts[] = {
    {"help",	no_argument,		NULL, 0  },
    {"version",	no_argument,		NULL, 1  },
    {"verbose",	no_argument,		NULL, 2  },
    {"server",	required_argument,	NULL, 'h'},
    {"host",	required_argument,	NULL, 'h'},
    {"port",	required_argument,	NULL, 'p'},
    {NULL,	0,			NULL, 0  }
};
#elif _WIN32
#define optarg __argv
#define optind __argc
#else
extern char *optarg;
extern int optind;
#endif

int main(int argc, char *argv[])
{
    int ch, nopar = 0;
    const char *server = NULL, *port = NULL;
    char *p, *q, *qstring, fstring[64] = "\0";
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(0x0101, &wsa))
    {
        fputs("Winsock initalization error.", stderr);
        return -1;
    }
#endif

#ifdef ENABLE_NLS
    setlocale(LC_ALL, "");
    bindtextdomain(NLS_CAT_NAME, LOCALEDIR);
    textdomain(NLS_CAT_NAME);
#endif

    while ((ch = GETOPT_LONGISH(argc, argv, "acdFg:h:Hi:KlLmMp:q:rRs:St:T:v:V:x",
				longopts, 0)) > 0) {
	/* RIPE flags */
	if (strchr(ripeflags, ch)) {
	    for (p = fstring; *p; p++);
	    sprintf(p--, "-%c ", ch);
	    continue;
	}
	if (strchr(ripeflagsp, ch)) {
	    for (p = fstring; *p; p++);
	    snprintf(p--, sizeof(fstring), "-%c %s ", ch, optarg);
	    if (ch == 't' || ch == 'v' || ch == 'q')
		nopar = 1;
	    continue;
	}
	/* program flags */
	switch (ch) {
	case 'h':
	    server = q = malloc(strlen(optarg) + 1);
	    for (p = optarg; *p && *p != ':'; *q++ = tolower(*p++));
	    if (*p == ':')
		port = p + 1;
	    *q = '\0';
	    break;
	case 'V':
	    client_tag = optarg;
	case 'H':
	    hide_discl = 0;	/* enable disclaimers hiding */
	    break;
	case 'p':
	    port = optarg;
	    break;
	case 2:
	    verb = 1;
	    break;
	case 1:
#ifdef VERSION
	    fprintf(stderr, _("Version %s.\n\nReport bugs to %s.\n"),
		    VERSION, "<md+whois@linux.it>");
	    fputs("Win32 port by Olof Lagerkvist, http://www.ltr-data.se\r\n",
		  stderr);
#else
	    fprintf(stderr, "%s %s\n", inetutils_package, inetutils_version);
#endif
	    exit(0);
	default:
	    usage();
	}
    }
    argc -= optind;
    argv += optind;

    if (argc == 0 && !nopar)	/* there is no parameter */
	usage();

    /* On some systems realloc only works on non-NULL buffers */
    qstring = malloc(64);
    *qstring = '\0';

    /* parse other parameters, if any */
    if (!nopar) {
	int qslen = 0;

	while (1) {
	  qslen += (int) strlen(*argv) + 1 + 1;
	    qstring = realloc(qstring, qslen);
	    strcat(qstring, *argv++);
	    if (argc == 1)
		break;
	    strcat(qstring, " ");
	    argc--;
	}
    }

    /* -v or -t has been used */
    if (!server && !*qstring)
	server = "whois.ripe.net";

#ifdef CONFIG_FILE
    if (!server) {
	server = match_config_file(qstring);
	if (verb && server)
	    printf(_("Using server %s.\n"), server);
    }
#endif

    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);
#ifndef _WIN32
    signal(SIGALRM, alarm_handler);
#endif
    alarm(60);

    if (!server) {
	char *tmp;

	tmp = normalize_domain(qstring);
	server = whichwhois(tmp);
	free(tmp);

	switch (server[0]) {
	    case 0:
		if (!(server = getenv("WHOIS_SERVER")))
		    server = DEFAULTSERVER;
		if (verb)
		    printf(_("Using default server %s.\n"), server);
		break;
	    case 1:
		puts(_("This TLD has no whois server, but you can access the "
			    "whois database at"));
	    case 2:
		puts(server + 1);
		exit(0);
	    case 3:
		puts(_("This TLD has no whois server."));
		exit(0);
	    case 4:
		if (verb)
		    puts(_("Connecting to whois.crsnic.net."));
		sockfd = openconn("whois.crsnic.net", NULL);
		server = query_crsnic(sockfd, qstring);
		closesocket(sockfd);
		if (!server)
		    exit(0);
		printf(_("\nFound a referral to %s.\n\n"), server);
		alarm(60);
		break;
	    case 9:
		if (verb)
		    puts(_("Connecting to whois.nic.cc."));
		sockfd = openconn("whois.nic.cc", NULL);
		server = query_crsnic(sockfd, qstring);
		closesocket(sockfd);
		if (!server)
		    exit(0);
		printf(_("\nFound a referral to %s.\n\n"), server);
		alarm(60);
		break;
	    case 7:
		if (verb)
		    puts(_("Connecting to whois.publicinterestregistry.net."));
		sockfd = openconn("whois.publicinterestregistry.net", NULL);
		server = query_pir(sockfd, qstring);
		closesocket(sockfd);
		if (!server)
		    exit(0);
		printf(_("\nFound referral to %s.\n\n"), server);
		alarm(60);
		break;
	    case 5:
		puts(_("No whois server is known for this kind of object."));
		exit(0);
	    case 6:
		puts(_("Unknown AS number or IP network. Please upgrade this program."));
		exit(0);
	    default:
		if (verb)
		    printf(_("Using server %s.\n"), server);
	}
    }

    if (getenv("WHOIS_HIDE"))
	hide_discl = 0;

    p = queryformat(server, fstring, qstring);
    if (verb)
	printf(_("Query string: \"%s\"\n\n"), p);
    strcat(p, "\r\n");

    sockfd = openconn(server, port);

    /*
     * Now we are connected and the query is supposed to complete quickly.
     * This will help people who run whois ... | less
     */
    alarm(0);
    do_query(sockfd, p);

    exit(0);
}

#ifdef CONFIG_FILE
const char *match_config_file(const char *s)
{
    FILE *fp;
    char buf[512];
    static const char delim[] = " \t";
#ifdef HAVE_REGEXEC
    regex_t re;
#endif

    if ((fp = fopen(CONFIG_FILE, "r")) == NULL) {
	if (errno != ENOENT)
	    err_exit("Cannot open " CONFIG_FILE);
	return NULL;
    }

    while (fgets(buf, sizeof(buf), fp) != NULL) {
	char *p;
	const char *pattern, *server;
#ifdef HAVE_REGEXEC
	int i;
#endif

	for (p = buf; *p; p++)
	    if (*p == '\n')
		*p = '\0';

	p = buf;
	while (*p == ' ' || *p == '\t')	/* eat leading blanks */
	    p++;
	if (!*p)
	    continue;		/* skip empty lines */
	if (*p == '#')
	    continue;		/* skip comments */

	pattern = strtok(p, delim);
	server = strtok(NULL, delim);
	if (!pattern || !server)
	    err_quit(_("Cannot parse this line: %s"), p);
	p = strtok(NULL, delim);
	if (p)
	    err_quit(_("Cannot parse this line: %s"), p);

#ifdef HAVE_REGEXEC
	i = regcomp(&re, pattern, REG_EXTENDED|REG_ICASE|REG_NOSUB);
	if (i != 0) {
	    char m[1024];
	    regerror(i, &re, m, sizeof(m));
	    err_quit("Invalid regular expression '%s': %s", pattern, m);
	}

	i = regexec(&re, s, 0, NULL, 0);
	if (i == 0) {
	    regfree(&re);
	    return strdup(server);
	}
	if (i != REG_NOMATCH) {
	    char m[1024];
	    regerror(i, &re, m, sizeof(m));
	    err_quit("regexec: %s",  m);
	}
	regfree(&re);
#else
	if (domcmp(s, pattern))
	    return strdup(server);
#endif
    }
    return NULL;
}
#endif

const char *whichwhois(const char *s)
{
    unsigned long ip;
    unsigned int i;

    /* IPv6 address */
    if (strchr(s, ':')) {
	if (strncmp(s, "2001:",  5) == 0) {
	    unsigned long v6net = strtol(s + 5, NULL, 16);
	    v6net = v6net & 0xfe00;	/* we care about the first 7 bits */
	    for (i = 0; ip6_assign[i].serv; i++)
		if (v6net == ip6_assign[i].net)
		    return ip6_assign[i].serv;
	    return "\x06";			/* unknown allocation */
	} else if (strncasecmp(s, "3ffe:", 5) == 0)
	    return "whois.6bone.net";
	/* RPSL hierarchical object like AS8627:fltr-TRANSIT-OUT */
	else if (strncasecmp(s, "as", 2) == 0 && isasciidigit(s[2]))
	    return whereas(atoi(s + 2));
	else
	    return "\x05";
    }

    /* email address */
    if (strchr(s, '@'))
	return "\x05";

    /* no dot and no hyphen means it's a NSI NIC handle or ASN (?) */
    if (!strpbrk(s, ".-")) {
	const char *p;

	for (p = s; *p; p++);			/* go to the end of s */
	if (strncasecmp(s, "as", 2) == 0 &&	/* it's an AS */
		(isasciidigit(s[2]) || s[2] == ' '))
	    return whereas(atoi(s + 2));
	else if (strncasecmp(p - 2, "jp", 2) == 0) /* JP NIC handle */
	    return "whois.nic.ad.jp";
	if (*s == '!')	/* NSI NIC handle */
	    return "whois.networksolutions.com";
	else
	    return "\x05";	/* probably a unknown kind of nic handle */
    }

    /* smells like an IP? */
    if ((ip = myinet_aton(s))) {
	for (i = 0; ip_assign[i].serv; i++)
	    if ((ip & ip_assign[i].mask) == ip_assign[i].net)
		return ip_assign[i].serv;
	return "\x05";			/* not in the unicast IPv4 space */
    }

    /* check the TLDs list */
    for (i = 0; tld_serv[i]; i += 2)
	if (domcmp(s, tld_serv[i]))
	    return tld_serv[i + 1];

    /* no dot but hyphen */
    if (!strchr(s, '.')) {
	/* search for strings at the start of the word */
	for (i = 0; nic_handles[i]; i += 2)
	    if (strncasecmp(s, nic_handles[i], strlen(nic_handles[i])) == 0)
		return nic_handles[i + 1];
	/* it's probably a network name */
	return "";
    }

    /* has dot and maybe a hyphen and it's not in tld_serv[], WTF is it? */
    /* either a TLD or a NIC handle we don't know about yet */
    return "\x05";
}

const char *whereas(const unsigned short asn)
{
    int i;

    for (i = 0; as_assign[i].serv; i++)
	if (asn >= as_assign[i].first && asn <= as_assign[i].last)
	    return as_assign[i].serv;
    return "\x06";
}

char *queryformat(const char *server, const char *flags, const char *query)
{
    char *buf;
    int i, isripe = 0;

    /* +2 for \r\n; +1 for NULL */
    buf = malloc(strlen(flags) + strlen(query) + strlen(client_tag) + 4
	    + 2 + 1);
    *buf = '\0';
    for (i = 0; ripe_servers[i]; i++)
	if (strcmp(server, ripe_servers[i]) == 0) {
	    strcat(buf, "-V ");
	    strcat(buf, client_tag);
	    strcat(buf, " ");
	    isripe = 1;
	    break;
	}
    if (!isripe)
	for (i = 0; ripe_servers_old[i]; i++)
	    if (strcmp(server, ripe_servers_old[i]) == 0) {
		strcat(buf, "-V");
		strcat(buf, client_tag);
		strcat(buf, " ");
		isripe = 1;
		break;
	    }
    if (*flags) {
	if (!isripe && strcmp(server, "whois.corenic.net") != 0)
	    puts(_("Warning: RIPE flags used with a traditional server."));
	strcat(buf, flags);
    }
    /* FIXME: /e is not applied to .JP ASN */
    if (!isripe && (strcmp(server, "whois.nic.mil") == 0 ||
	    strcmp(server, "whois.nic.ad.jp") == 0) &&
	    strncasecmp(query, "AS", 2) == 0 && isasciidigit(query[2]))
	sprintf(buf, "AS %s", query + 2);	/* fix query for DDN */
    else if (!isripe && strcmp(server, "whois.nic.ad.jp") == 0) {
	char *lang = getenv("LANG");	/* not a perfect check, but... */
	if (!lang || (strncmp(lang, "ja", 2) != 0))
	    sprintf(buf, "%s/e", query);	/* ask for english text */
	else
	    strcat(buf, query);
    } else
	strcat(buf, query);
    return buf;
}

void do_query(const SOCKET sock, const char *query)
{
    char buf[2000];
    int i = 0, hide = hide_discl;

    if (send(sock, query, (int) strlen(query), 0) == SOCKET_ERROR)
	err_exit("send");
/* Using shutdown breaks the buggy RIPE server.
    if (shutdown(sock, 1) == SOCKET_ERROR)
	err_exit("shutdown");
*/
    for (;;) {
        if (LineRecv((HANDLE)sock, buf, sizeof(buf), 60000) == 0)
            if (win_errno != ERROR_SUCCESS)
	        if (win_errno == ERROR_HANDLE_EOF)
	            break;
	        else
	            err_exit("Receive error");

        if (hide == 1) {
	    if (strncmp(buf, hide_strings[i+1], strlen(hide_strings[i+1]))==0)
		hide = 2;	/* stop hiding */
	    continue;		/* hide this line */
	}
	if (hide == 0) {
	    for (i = 0; hide_strings[i] != NULL; i += 2) {
		if (strncmp(buf, hide_strings[i], strlen(hide_strings[i]))==0){
		    hide = 1;	/* start hiding */
		    break;
		}
	    }
	    if (hide == 1)
		continue;	/* hide the first line */
	}
#ifdef EXT_6BONE
	/* % referto: whois -h whois.arin.net -p 43 as 1 */
	if (strncmp(buf, "% referto:", 10) == 0) {
	    char nh[256], np[16], nq[1024];

	    if (sscanf(buf, REFERTO_FORMAT, nh, np, nq) == 3) {
		SOCKET fd;

		if (verb)
		    printf(_("Detected referral to %s on %s.\n"), nq, nh);
		strcat(nq, "\r\n");
		fd = openconn(nh, np);
		do_query(fd, nq);
		continue;
	    }
	}
#endif
	
	CharToOem(buf, buf);
	puts(buf);
    }

    if (hide == 1)
	err_quit(_("Catastrophic error: disclaimer text has been changed.\r\n"
		   "Please upgrade this program.\r\n"));
}

const char *query_crsnic(const SOCKET sock, const char *query)
{
    char *temp, buf[2000], *ret = NULL;
    int state = 0;

    temp = malloc(strlen(query) + 1 + 2 + 1);
    *temp = '=';
    strcpy(temp + 1, query);
    strcat(temp, "\r\n");

    if (send(sock, temp, (int) strlen(temp), 0) == SOCKET_ERROR)
	err_exit("send");

    for (;;) {
        if (LineRecv((HANDLE)sock, buf, sizeof(buf), 60000) == 0)
            if (win_errno != ERROR_SUCCESS)
	        if (win_errno == ERROR_HANDLE_EOF)
	            break;
	        else
	            err_exit("Receive error");

	/* If there are multiple matches only the server of the first record
	   is queried */
	if (state == 0 && strncmp(buf, "   Domain Name:", 15) == 0)
	    state = 1;
	if (state == 1 && strncmp(buf, "   Whois Server:", 16) == 0) {
	    char *p;

	    for (p = buf; *p != ':'; p++);	/* skip until colon */
	    for (p++; *p == ' '; p++);		/* skip colon and spaces */
	    ret = strcpy(malloc(strlen(p) + 2), p);

	    state = 2;
	}
	/* the output must not be hidden or no data will be shown for
	   host records and not-existing domains */
	/* XXX feel free to send a patch to hide the long disclaimer */
	CharToOem(buf, buf);
	puts(buf);
    }

    free(temp);
    return ret;
}

const char *query_pir(const SOCKET sock, const char *query)
{
    char *temp, buf[2000], *ret = NULL;
    int state = 0;

    temp = malloc(strlen(query) + 5 + 2 + 1);
    strcpy(temp, "FULL ");
    strcat(temp, query);
    strcat(temp, "\r\n");

    if (send(sock, temp, (int) strlen(temp), 0) == SOCKET_ERROR)
	err_exit("send");

    for (;;) {
        if (LineRecv((HANDLE)sock, buf, sizeof(buf), 60000) == 0)
            if (win_errno != ERROR_SUCCESS)
	        if (win_errno == ERROR_HANDLE_EOF)
	            break;
	        else
	            err_exit("Receive error");

	/* If there are multiple matches only the server of the first record
	   is queried */
	if (state == 0 &&
    strncmp(buf, "Registrant Name:SEE SPONSORING REGISTRAR", 40) == 0)
	    state = 1;
	if (state == 1 &&
		strncmp(buf, "Registrant Street1:Whois Server:", 32) == 0) {
	    char *p;

	    for (p = buf; *p != ':'; p++);	/* skip until colon */
	    for (p++; *p != ':'; p++);		/* skip until 2nd colon */
	    for (p++; *p == ' '; p++);		/* skip colon and spaces */
	    ret = strcpy(malloc(strlen(p) + 2), p);

	    state = 2;
	}
	CharToOem(buf, buf);
	puts(buf);
    }

    free(temp);
    return ret;
}

SOCKET
openconn(const char *server, const char *port)
{
    SOCKET fd;
#ifdef HAVE_GETADDRINFO
    int err;
    struct addrinfo hints, *res, *ai;
#else
    struct hostent *hostinfo;
    struct servent *servinfo;
    struct sockaddr_in saddr;
#endif

#ifdef HAVE_GETADDRINFO
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((err = getaddrinfo(server, port ? port : "whois", &hints, &res)) != 0)
	err_quit("getaddrinfo: %s", gai_strerror(err));
    for (ai = res; ai; ai = ai->ai_next) {
	if ((fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) ==
	    INVALID_SOCKET)
	    continue;		/* ignore */
	if (connect(fd, (struct sockaddr *)ai->ai_addr, ai->ai_addrlen) == 0)
	    break;		/* success */
	closesocket(fd);
    }
    freeaddrinfo(res);

    if (!ai)
	err_exit("connect");
#else
    if ((hostinfo = gethostbyname(server)) == NULL)
	err_quit(_("Host %s not found."), server);
    if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	err_exit("socket");
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_addr = *(struct in_addr *) hostinfo->h_addr;
    saddr.sin_family = AF_INET;
    if (!port) {
	saddr.sin_port = htons(43);
    } else if ((saddr.sin_port = htons(atoi(port))) == 0) {
	if ((servinfo = getservbyname(port, "tcp")) == NULL)
	    err_quit(_("%s/tcp: unknown service"), port);
	saddr.sin_port = servinfo->s_port;
    }

    if (connect(fd, (struct sockaddr *)&saddr, sizeof(saddr)) == SOCKET_ERROR)
	err_exit("connect");
#endif

    return fd;
}

void alarm_handler(int signum)
{
    closesocket(sockfd);
    err_quit(_("Timeout."));
}

void sighandler(int signum)
{
    closesocket(sockfd);
    err_quit(_("Interrupted by signal %d..."), signum);
}

/* check if dom ends with tld */
int domcmp(const char *dom, const char *tld)
{
    const char *p, *q;

    for (p = dom; *p; p++); p--;	/* move to the last char */
    for (q = tld; *q; q++); q--;
    while (p >= dom && q >= tld && tolower(*p) == *q) {	/* compare backwards */
	if (q == tld)			/* start of the second word? */
	    return 1;
	p--; q--;
    }
    return 0;
}

char *normalize_domain(const char *dom)
{
    char *p, *ret;

    ret = strdup(dom);
    for (p = ret; *p; p++); p--;	/* move to the last char */
    for (; *p == '.' || p == ret; p--)	/* eat trailing dots */
	*p = '\0';
    return ret;
}

unsigned long myinet_aton(const char *s)
{
    int a, b, c, d;

    if (!s)
	return 0;
    if (sscanf(s, "%d.%d.%d.%d", &a, &b, &c, &d) != 4)
	return 0;
    return (a << 24) + (b << 16) + (c << 8) + d;
}

int isasciidigit(const char c) {
    return (c >= '0' && c <= '9') ? 1 : 0;
}

/* http://www.ripe.net/ripe/docs/databaseref-manual.html */

void usage(void)
{
    fputs(
"Usage: whois [OPTION]... OBJECT...\r\n\n"
"-l                     one level less specific lookup [RPSL only]\r\n"
"-L                     find all Less specific matches\r\n"
"-m                     find first level more specific matches\r\n"
"-M                     find all More specific matches\r\n"
"-c                     find the smallest match containing a mnt-irt attribute\r\n"
"-x                     exact match [RPSL only]\r\n"
"-d                     return DNS reverse delegation objects too [RPSL only]\r\n"
"-i ATTR[,ATTR]...      do an inverse lookup for specified ATTRibutes\r\n"
"-T TYPE[,TYPE]...      only look for objects of TYPE\r\n"
"-K                     only primary keys are returned [RPSL only]\r\n"
"-r                     turn off recursive lookups for contact information\r\n"
"-R                     force to show local copy of the domain object even\r\n"
"                       if it contains referral\r\n"
"-a                     search all databases\r\n"
"-s SOURCE[,SOURCE]...  search the database from SOURCE\r\n"
"-g SOURCE:FIRST-LAST   find updates from SOURCE from serial FIRST to LAST\r\n"
"-t TYPE                request template for object of TYPE ('all' for a list)\r\n"
"-v TYPE                request verbose template for object of TYPE\r\n"
"-q [version|sources|types]  query specified server info [RPSL only]\r\n"
"-F                     fast raw output (implies -r)\r\n"
"-h HOST                connect to server HOST\r\n"
"-p PORT                connect to PORT\r\n"
"-H                     hide legal disclaimers\r\n"
"      --verbose        explain what is being done\r\n"
"      --help           display this help and exit\r\n"
"      --version        output version information and exit\r\n", stderr);
    exit(0);
}

void err_exit(const char *msg)
{
  if (win_errno == ERROR_IO_PENDING)
    fputs("Timeout.\r\n", stderr);
  else
    h_perror(msg);
  exit(1);
}

void err_quit(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fputs("\r\n", stderr);
    va_end(ap);
    exit(2);
}

