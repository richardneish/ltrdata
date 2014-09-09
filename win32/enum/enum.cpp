#define WINNT

#include <windows.h>

#include <time.h>
#include <stdio.h>

#include <lmcons.h>     // LAN Manager common definitions
#include <lmerr.h>      // LAN Manager network error definitions
#include <lmaccess.h>   // Access, Domain, Group and User classes
#include <lmshare.h>    // Connection, File, Session and Share classes
#include <lmapibuf.h>   // NetApiBuffer class
#include <ntsecapi.h>

#include "getopt.h"

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "mpr.lib")
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "advapi32.lib")

void enum_names(char*);
void enum_names_brief(char*);
void enum_passwd_policy(char*);
void enum_shares(char*,int);
void enum_lsa(char *);
void enum_machines(char *, int);
void enum_users(char *, int);
void enum_groups(char *, int);

void dict_crack(NETRESOURCE*,char*,char*);
void brute_crack(NETRESOURCE*,char*,char*);

void cancelsession(char *);

void lsastrcpy(PLSA_UNICODE_STRING, LPWSTR);
void describe(DWORD);
void usage(char *);


char username[1024] = "", password[1024] = "";
char server[1024] = "", path[1024] = "";
char dictfile[1024] = "";
NETRESOURCE res;


void main(int argc, char **argv) {
  int enumnames = 0, enumpasswd = 0, enumshares = 0, enumlsa = 0, enummachines = 0,
    enumusers = 0, enumgroups = 0;
  int dictcrack = 0, brutecrack = 0;
  int detailed = 0, clean = 1; 
  int c, done = 0;
  DWORD ret = 0;

  if (argc == 1) { 
    usage(argv[0]);
    exit(1);
  }

  while ((c = getopt(argc, argv, "cdGUNPSLMDBu:p:f:")) && !done) {
    switch (c) {		
    case EOF: {
      if (!argv[optind]) {
	printf("fatal: no server specified!\n");
	exit(1);
      }
      strncpy(server,argv[optind],1024); 
      printf("server: %s\n", server);
      done++; break;
    }
    case 'u': {
      strncpy(username,optarg,1024); 
      printf("username: %s\n", username);
      break;
    }
    case 'p': {
      strncpy(password,optarg,1024); 
      printf("password: %s\n", password);
      break;
    }
    case 'f': {
      strncpy(dictfile,optarg,1024);
      printf("dictfile: %s\n",dictfile);
      break;
    }

    case 'd': detailed++; break;
    case 'c': clean--; break;

    case 'U': enumusers++; break;
    case 'N': enumnames++; break;
    case 'P': enumpasswd++; break;
    case 'S': enumshares++; break;
    case 'L': enumlsa++; break;
    case 'M': enummachines++; break;
    case 'G': enumgroups++; break;

    case 'D': dictcrack++; break;
    case 'B': brutecrack++; break;

    default: printf("fatal: unknown switch\n"); exit(1);
    }	
  }

  if (!(enumnames || enumpasswd || enumshares || enumlsa || enummachines || enumusers ||
	enumgroups || brutecrack || dictcrack)) {
    printf("fatal: nothing to do!\n");
    exit(1);
  }

  if ((dictcrack || brutecrack) && !strlen(username)) {
    printf("fatal: cracking requires a username\n");
    exit(1);
  }

  sprintf(path,"\\\\%s\\ipc$",server);

  res.lpLocalName = NULL;
  res.lpProvider = NULL;
  res.dwType = RESOURCETYPE_ANY;
  res.lpRemoteName = (char*)&path;

  if (clean)
    cancelsession(path);

  if (dictcrack) {
    if (!strlen(dictfile)) 
      printf("fatal: no dictionary specified\n");
    else dict_crack(&res,(char*)&username,(char*)&dictfile);
    exit(1);
  } 

  printf("setting up session... ");
  ret = WNetAddConnection2(&res,(const char *)&password,(const char *)&username,0);
  if (ret != ERROR_SUCCESS) {
    printf("fail.\n");
    describe(ret);
    exit(1);
  } else printf("success.\n");


  if (enumpasswd)
    enum_passwd_policy((char*)&server);

  if (enumlsa)
    enum_lsa((char*)&server);

  if (enumnames)
    if (detailed)
      enum_names((char*)&server);
    else enum_names_brief((char*)&server);

  if (enumusers)
    enum_users((char*)&server,detailed);

  if (enumshares) 
    enum_shares((char*)&server,detailed);

  if (enummachines) 
    enum_machines((char*)&server,detailed);

  if (enumgroups)
    enum_groups((char*)&server,detailed);

  if (clean) {
    printf("cleaning up... ");
    ret = WNetCancelConnection2((char*)&path,0,TRUE);
    if (ret != ERROR_SUCCESS) {
      printf("fail.\n");
      describe(ret);
      exit(1);
    } else printf("success.\n");
  }

  exit (0);
}
 

void enum_names(char *s) {
  DWORD ret;
  DWORD level, prefmaxlen;
  USER_INFO_2 *cur, *buf = NULL;
  DWORD filter, read, total, resume = 0;

  char serv[1024];
  wchar_t server[1024];
	
  sprintf(serv,"\\\\%s",s);
  mbstowcs(server,serv,1024);

  level = 2;  
  filter = 0;	
  prefmaxlen = 1024;

  int pass = 1;
  do {
    printf("enumerating names (pass %d)... ",pass);

    ret = NetUserEnum(server,level,0,(BYTE **)&buf,prefmaxlen,
		      &read,&total,&resume);

    if (ret != NERR_Success && ret != ERROR_MORE_DATA) {
      printf("fail\n");
      describe(ret);
      break;
    } 

    if (total) {
      printf("got %d accounts, %d left:\n",read,total-read);

      cur = buf;
      for (int i = 0; i < read; ++i , ++cur) {

	switch(cur->usri2_priv) {
	case USER_PRIV_GUEST: printf("guest: "); break;
	case USER_PRIV_USER: printf("user: "); break;
	case USER_PRIV_ADMIN: printf("admin: "); break;
	}

	printf("%S ",   cur->usri2_name);
				
	if (wcslen(cur->usri2_full_name)) 
	  printf("(%S) ", cur->usri2_full_name);
	
	if (wcslen(cur->usri2_home_dir)) 
	  printf("[%S]", cur->usri2_home_dir);

	printf("\n");

	if (wcslen(cur->usri2_comment)) 
	  printf("  comment: %S\n",cur->usri2_comment);

	if (cur->usri2_last_logon) 
	  printf("  login: %s",ctime((time_t*)&cur->usri2_last_logon));
	
	if (cur->usri2_last_logoff) 
	  printf("  logoff: %s",ctime((time_t*)&cur->usri2_last_logoff));

	if (cur->usri2_acct_expires != TIMEQ_FOREVER)
	  printf("  expires: %s\n", ctime((time_t*)&cur->usri2_acct_expires));

	if (cur->usri2_num_logons) 
	  printf("  good logins: %d\n", cur->usri2_num_logons);

	if (cur->usri2_bad_pw_count) 
	  printf("  bad logins: %d\n", cur->usri2_bad_pw_count);
      }

      NetApiBufferFree(buf);
    } else printf("none\n");

    pass++;
  } while (ret == ERROR_MORE_DATA);
}


void enum_names_brief(char *s) {
  DWORD ret;
  DWORD level, prefmaxlen;
  USER_INFO_0 *cur, *buf = NULL;
  DWORD filter, read, total, resume = 0;

  char serv[1024];
  wchar_t server[1024];
	
  sprintf(serv,"\\\\%s",s);
  mbstowcs(server,serv,1024);
	
  level = 0;  
  filter = 0;	
  prefmaxlen = 1024;

  int pass = 1;

  do {
    printf("getting namelist (pass %d)... ",pass);

    ret = NetUserEnum(server,level,0,(BYTE **)&buf,prefmaxlen,
		      &read,&total,&resume);

    if (ret != NERR_Success && ret != ERROR_MORE_DATA) {
      printf("fail\n");
      describe(ret);
      break;
    } 

    if (total) {	
      char namelist[4*UNLEN] = "";

      cur = buf;

      printf("got %d, %d left:\n",read,total-read);

      for (int i = 0; i < read; ++i , ++cur) {
	size_t len = strlen(namelist);
	char name[UNLEN];
	sprintf(name,"  %S",cur->usri0_name);

	if (len + strlen(name) < 75) {
	  strcat(namelist,name);
	} else {
	  printf("%s\n",namelist);
	  strcpy(namelist,name);
	}
      }
      printf("%s\n",namelist);

      NetApiBufferFree(buf);
    } else printf("none\n");

    pass++;
  } while (ret == ERROR_MORE_DATA);
}


void enum_passwd_policy(char *s) {
  DWORD ret, level;
  USER_MODALS_INFO_0 *buf0 = NULL;
  USER_MODALS_INFO_3 *buf3 = NULL;

  char serv[1024];
  wchar_t server[1024];
	
  sprintf(serv,"\\\\%s",s);
  mbstowcs(server,serv,1024);
	
  level = 0;  
  if ((ret = NetUserModalsGet(server,level,(BYTE **)&buf0)) != NERR_Success) {
    printf("couldn't get password policy\n");
    describe(ret);
  } else {
    printf("password policy:\n");

    printf("  min length: ");
    if (buf0->usrmod0_min_passwd_len) 
      printf("%d chars\n", buf0->usrmod0_min_passwd_len);
    else printf("none\n");

    printf("  min age: ");
    if (buf0->usrmod0_min_passwd_age) 
      printf("%d days\n",buf0->usrmod0_min_passwd_age/(60*60*24));
    else printf("none\n");

    printf("  max age: ");
    if (buf0->usrmod0_max_passwd_age != TIMEQ_FOREVER)
      printf("%d days\n",buf0->usrmod0_max_passwd_age/(60*60*24));
    else printf("none\n");

    NetApiBufferFree(buf0);
  }

  level = 3;
  if ((ret = NetUserModalsGet(server,level,(BYTE **)&buf3)) != NERR_Success) {
    printf("couldn't get lockout policy\n");
    describe(ret);
  } else {
    printf("  lockout threshold: ");
    if (buf3->usrmod3_lockout_threshold) 
      printf("%d attempts\n", buf3->usrmod3_lockout_threshold);
    else printf("none\n");

    printf("  lockout duration: %d mins\n", buf3->usrmod3_lockout_duration/(60));
    printf("  lockout reset: %d mins\n",buf3->usrmod3_lockout_observation_window/(60));

    NetApiBufferFree(buf3);
  }
}

void enum_shares(char *s, int d) {
  LPVOID buff = NULL;
  DWORD ret, level, prefmaxlen;
  DWORD read, total, resume = 0;

  char cserver[1024];
  wchar_t wserver[1024] = L"";
	
  sprintf(cserver,"\\\\%s",s);
  mbstowcs(wserver,cserver,1023);

  level = d?1:0;
  prefmaxlen = 1023;

  int pass = 1;
  do {
    printf("enumerating shares (pass %d)... ",pass);

    ret = NetShareEnum(wserver,level,(unsigned char **)&buff,prefmaxlen,
		       &read,&total,&resume);

    resume = total-(total-read); // NetShareEnum is borked

    if (ret != NERR_Success && ret != ERROR_MORE_DATA) {
      printf("fail\n");
      describe(ret);
      break;
    } 

    if (total) {
      printf("got %d shares, %d left:\n",read,total-read);			

      if (d) {
	PSHARE_INFO_1 cur = (PSHARE_INFO_1)buff;

	for (int i = 0; i < read; ++i , ++cur) {
	  switch(cur->shi1_type & ~STYPE_SPECIAL) {
	  case STYPE_DISKTREE: printf("  fs: "); break;
	  case STYPE_DEVICE: printf("  dev: "); break;
	  case STYPE_PRINTQ: printf("  print: "); break;
	  case STYPE_IPC: printf("  ipc: "); break;
	  default: printf("  unknown (%08lx): ",cur->shi1_type);
	  }

	  printf("%S (%S)\n",cur->shi1_netname,cur->shi1_remark);
	}
      } else {
	PSHARE_INFO_0 cur = (PSHARE_INFO_0)buff;
	char sharelist[4*UNLEN] = "";
	char share[UNLEN];

	for (int i = 0; i < read; ++i , ++cur) {
	  size_t len = strlen(sharelist);

	  sprintf(share,"  %S",cur->shi0_netname);

	  if (len + strlen(share) < 75) {
	    strcat(sharelist,share);
	  } else {
	    printf("%s\n",sharelist);
	    strcpy(sharelist,share);
	  }
	}
	printf("%s\n",sharelist);
      }

      NetApiBufferFree(buff);
    }

    pass++;
  } while (ret == ERROR_MORE_DATA);
}


void enum_lsa(char *s) {
  char cserver[1024];
  wchar_t wserver[1024];
  LSA_UNICODE_STRING server;
  DWORD err;
  ULONG ret;
    
  LSA_OBJECT_ATTRIBUTES attrs;
  LSA_HANDLE hLsa;

  ZeroMemory(&attrs, sizeof(attrs));

  sprintf(cserver,"\\\\%s",s);
  mbstowcs(wserver,cserver,1024);
  lsastrcpy(&server, wserver);

  printf("opening lsa policy... ");
  err = LsaOpenPolicy(&server, &attrs,
		      POLICY_VIEW_LOCAL_INFORMATION|POLICY_LOOKUP_NAMES, &hLsa);
  if (err < 0) {
    printf("fail.\n");
    describe(err);
    return;
  } else printf("success.\n");

  PVOID buff;

  /*
  LSA_UNICODE_STRING user;
  PLSA_REFERENCED_DOMAIN_LIST doms;
  PLSA_TRANSLATED_SID sids;
  */

  if (LsaQueryInformationPolicy(hLsa,PolicyLsaServerRoleInformation,&buff) >= 0) {
    PPOLICY_LSA_SERVER_ROLE_INFO info = (PPOLICY_LSA_SERVER_ROLE_INFO)buff;
    printf("server role: %d [%s]\n", info->LsaServerRole,
	   info->LsaServerRole==2?"backup (BDC)":"primary (unknown)");
    LsaFreeMemory(buff);
  }

  printf("names:\n");
  if ((ret = LsaQueryInformationPolicy(hLsa,PolicyAccountDomainInformation,&buff)) >= 0) {
    PPOLICY_ACCOUNT_DOMAIN_INFO info = (PPOLICY_ACCOUNT_DOMAIN_INFO) buff;
    wchar_t buf[100];
		
    wcsncpy (buf, info->DomainName.Buffer, info->DomainName.Length/2);
    buf[info->DomainName.Length/2] = L'\0';
    printf("  netbios: %S\n",buf);
    LsaFreeMemory(buff);
  } else printf("  couldn't get netbios name\n");

  if (LsaQueryInformationPolicy(hLsa,PolicyPrimaryDomainInformation,&buff) >= 0) {
    PPOLICY_PRIMARY_DOMAIN_INFO info = (PPOLICY_PRIMARY_DOMAIN_INFO)buff;
    wchar_t buf[100];

    wcsncpy (buf, info->Name.Buffer, info->Name.Length/2);
    buf[info->Name.Length/2] = L'\0';
    printf("  domain: %S\n",buf);
    LsaFreeMemory(buff);
  } else printf("  couldn't get domain name\n");


  if (LsaQueryInformationPolicy(hLsa,PolicyDefaultQuotaInformation,&buff) >= 0) {
    PPOLICY_DEFAULT_QUOTA_INFO info = (PPOLICY_DEFAULT_QUOTA_INFO)buff;

    printf("quota:\n");
    printf("  paged pool limit: %d\n",info->QuotaLimits.PagedPoolLimit);
    printf("  non paged pool limit: %d\n",info->QuotaLimits.NonPagedPoolLimit);
    printf("  min work set size: %d\n",info->QuotaLimits.MinimumWorkingSetSize);
    printf("  max work set size: %d\n",info->QuotaLimits.MaximumWorkingSetSize);
    printf("  pagefile limit: %d\n",info->QuotaLimits.PagefileLimit);
    printf("  time limit: %d\n",info->QuotaLimits.TimeLimit);
    LsaFreeMemory(buff);
  } 

  LSA_ENUMERATION_HANDLE hEnum = 0;
  unsigned int pass = 1;

  printf("trusted domains:\n");

  do {
    LONG ret = 0;
    ULONG count;

    ret = LsaEnumerateTrustedDomains(hLsa,&hEnum,&buff,1000,&count);

    if (ret < 0) {
      if (pass == 1) 
	if (ret != 0x8000001a) 
	  printf("  none\n");
	else printf("  indeterminate\n");
      break;
    }

    //		printf("got %d accounts\n",count);

    wchar_t buf[100];
    PLSA_TRUST_INFORMATION info = (PLSA_TRUST_INFORMATION)buff;

    unsigned int i;
    for (i=0; i<count; i++) {
      wcsncpy (buf, info[i].Name.Buffer, info[i].Name.Length/2);

      buf[info[i].Name.Length/2] = L'\0';
      printf("  %S\n",buf);
    }
    pass++;
  } while ((err != ERROR_NO_MORE_ITEMS) && (err != 0x8000001a)); 

  if (NetUserModalsGet(wserver,1,(unsigned char **)&buff) == NERR_Success) {
    PUSER_MODALS_INFO_1 info = (PUSER_MODALS_INFO_1)buff;

    if (wcslen(info->usrmod1_primary)) 
      printf("PDC: %S\n", info->usrmod1_primary);

    printf("netlogon done by a ");
    switch(info->usrmod1_role) {
    case UAS_ROLE_STANDALONE: printf("standalone");
    case UAS_ROLE_MEMBER: printf("member");
    case UAS_ROLE_BACKUP: printf("BDC");
    case UAS_ROLE_PRIMARY: printf("PDC");
    }
    printf(" server\n");

    NetApiBufferFree (buff);
  }


}

void enum_machines(char *s, int d) {
  LPVOID buff = NULL;
  DWORD read = 0;
  DWORD index = 0;
  DWORD ret = 0;

  char cserver[1024];
  wchar_t wserver[1024];
	
  sprintf(cserver,"\\\\%s",s);
  mbstowcs(wserver,cserver,1024);

  int pass = 1;
  do {
    printf("getting machine list (pass %d, index %d)... ",pass, index);

    ret = NetQueryDisplayInformation(wserver,2,index,1000,1024,&read,&buff);

    if (ret != NERR_Success && ret != ERROR_MORE_DATA) {
      printf("fail\n");
      describe(ret);
      break;
    } else printf("success, got %d.\n",read);

    if (read) {	
      PNET_DISPLAY_MACHINE cur = (PNET_DISPLAY_MACHINE)buff;
      int i;

      if (d) {
	for (i = 0; i < read; ++i , ++cur) {
	  printf("  %S ",cur->usri2_name);
	  if (wcslen(cur->usri2_comment))
	    printf("(%S) ",cur->usri2_comment);
				
	  printf("[ trust: ");
	  if (cur->usri2_flags & UF_NORMAL_ACCOUNT)
	    printf("normal");
	  if (cur->usri2_flags & UF_WORKSTATION_TRUST_ACCOUNT)
	    printf("workstation");
	  if (cur->usri2_flags & UF_SERVER_TRUST_ACCOUNT)
	    printf("server");
	  if (cur->usri2_flags & UF_INTERDOMAIN_TRUST_ACCOUNT)
	    printf("interdomain");
	  printf("  ");

	  printf("flags: ");
	  if (cur->usri2_flags & UF_SCRIPT)
	    printf("script ");
	  if (cur->usri2_flags & UF_ACCOUNTDISABLE)
	    printf("disabled ");
	  if (cur->usri2_flags & UF_HOMEDIR_REQUIRED)
	    printf("need_homedir ");
	  if (cur->usri2_flags & UF_LOCKOUT)
	    printf("locked_out ");
	  if (cur->usri2_flags & UF_PASSWD_NOTREQD)
	    printf("no_passwd ");
	  if (cur->usri2_flags & UF_PASSWD_CANT_CHANGE)
	    printf("passwd_unchangable ");
	  printf("]\n");
	} 
      } else {
	char machinelist[4*UNLEN] = "";
	char machine[UNLEN];

	for (i = 0; i < read; ++i , ++cur) {
	  size_t len = strlen(machinelist);

	  sprintf(machine,"  %S",cur->usri2_name);

	  if (len + strlen(machine) < 75) {
	    strcat(machinelist,machine);
	  } else {
	    printf("%s\n",machinelist);
	    strcpy(machinelist,machine);
	  }
	}
	printf("%s\n",machinelist);
      }

      index += i;

      NetApiBufferFree(buff);
			
      //  PNET_DISPLAY_USER pUInfo = (PNET_DISPLAY_USER) pBuf;
      //  PNET_DISPLAY_MACHINE pMInfo = (PNET_DISPLAY_MACHINE) pBuf;
      //  PNET_DISPLAY_GROUP pGInfo = (PNET_DISPLAY_GROUP) pBuf;
    }
    pass++;
  } while (ret == ERROR_MORE_DATA);
}

void enum_users(char *s, int d) {
  LPVOID buff = NULL;
  DWORD read = 0;
  DWORD index = 0;
  DWORD ret = 0;

  char cserver[1024];
  wchar_t wserver[1024];
	
  sprintf(cserver,"\\\\%s",s);
  mbstowcs(wserver,cserver,1024);

  int pass = 1;
  do {
    printf("getting user list (pass %d, index %d)... ",pass, index);

    ret = NetQueryDisplayInformation(wserver,1,index,1000,4096,&read,&buff);

    if (ret != NERR_Success && ret != ERROR_MORE_DATA) {
      printf("fail\n");
      describe(ret);
      break;
    } else printf("success, got %d.\n",read);

    if (read) {	
      PNET_DISPLAY_USER cur = (PNET_DISPLAY_USER)buff;
      int i;

      if (d) {
	for (i = 0; i < read; ++i , ++cur) {
	  printf("  %S ",cur->usri1_name);
	  if (wcslen(cur->usri1_comment))
	    printf("(%S)\n",cur->usri1_comment);
					
	  printf("  attributes: ");
	  if (cur->usri1_flags & UF_ACCOUNTDISABLE)
	    printf("disabled ");
	  if (cur->usri1_flags & UF_HOMEDIR_REQUIRED)
	    printf("need_homedir ");
	  if (cur->usri1_flags & UF_LOCKOUT)
	    printf("locked_out ");
	  if (cur->usri1_flags & UF_PASSWD_NOTREQD)
	    printf("no_passwd ");
	  if (cur->usri1_flags & UF_PASSWD_CANT_CHANGE)
	    printf("passwd_unchangable ");
	  printf("\n");
	}
      } else {
	char userlist[4*UNLEN] = "";
	char user[UNLEN];				

	for	(i = 0; i < read; ++i , ++cur) {
	  size_t	len = strlen(userlist);

	  sprintf(user,"  %S",cur->usri1_name);

	  if (len + strlen(user) < 75) {
	    strcat(userlist,user);
	  } else {
	    printf("%s\n",userlist);
	    strcpy(userlist,user);
	  }
	}
	printf("%s\n",userlist);
      }

      index += i;
				
      NetApiBufferFree(buff);
			
      //  PNET_DISPLAY_USER pUInfo = (PNET_DISPLAY_USER) pBuf;
      //  PNET_DISPLAY_MACHINE pMInfo = (PNET_DISPLAY_MACHINE) pBuf;
      //  PNET_DISPLAY_GROUP pGInfo = (PNET_DISPLAY_GROUP) pBuf;
    }
    pass++;
  } while (ret == ERROR_MORE_DATA);
}


void enum_groups(char *s, int) {
  DWORD ret, read, total;
  DWORD_PTR resume = 0;
  LPVOID buff;

  char cserver[1024];
  wchar_t wserver[1024];
	
  sprintf(cserver,"\\\\%s",s);
  mbstowcs(wserver,cserver,1024);

  do {
    ret = NetLocalGroupEnum(wserver, 0, (unsigned char **)&buff, 1024, &read,
			    &total, &resume);

    if (ret != NERR_Success && ret != ERROR_MORE_DATA) {
      printf("fail\n");
      describe(ret);
      break;
    } 

    PLOCALGROUP_INFO_0 info = (PLOCALGROUP_INFO_0) buff;
    unsigned i;

    for (i=0; i<read; i++) {
      printf("Group: %S\n",info[i].lgrpi0_name);

      DWORD ret, read, total;
      DWORD_PTR resume = 0;
      ret = NetLocalGroupGetMembers(wserver,
				    info[i].lgrpi0_name, 3, 
				    (unsigned char **)&buff, 1024, &read,
				    &total, &resume);

      if (ret != NERR_Success && ret != ERROR_MORE_DATA) {
	printf("fail\n");
	describe(ret);
	break;
      } 

      PLOCALGROUP_MEMBERS_INFO_3 info = (PLOCALGROUP_MEMBERS_INFO_3) buff;

      for (unsigned i=0; i<read; i++) {
	printf("%S\n", info[i].lgrmi3_domainandname);
      }
      NetApiBufferFree (buff);
    }

    NetApiBufferFree (buff);

  } while (ret == ERROR_MORE_DATA );
}


void lsastrcpy(PLSA_UNICODE_STRING lsastr, LPWSTR str) {
  if(!str) {
    lsastr->Buffer = NULL;
    lsastr->Length = 0;
    lsastr->MaximumLength = 0;
  } else {
    lsastr->Buffer = str;
    lsastr->Length = (USHORT)wcslen(str) * sizeof(WCHAR);
    lsastr->MaximumLength = lsastr->Length + sizeof(WCHAR);
  }
}


void usage(char *prog) {
  printf("usage:  %s  [switches]  [hostname|ip]\n",prog);
  printf("  -U:  get userlist\n");
  printf("  -M:  get machine list\n");
  printf("  -N:  get namelist dump (different from -U|-M)\n");
  printf("  -S:  get sharelist\n");
  printf("  -P:  get password policy information\n");
  printf("  -G:  get group and member list\n");
  printf("  -L:  get LSA policy information\n");
  printf("  -D:  dictionary crack, needs -u and -f\n");
  printf("  -d:  be detailed, applies to -U and -S\n");
  printf("  -c:  don\'t cancel sessions\n");
  printf("  -u:  specify username to use (default \"\")\n");
  printf("  -p:  specify password to use (default \"\")\n");
  printf("  -f:  specify dictfile to use (wants -D)\n");
}


void dict_crack(NETRESOURCE *r, char *u, char *f) {
  FILE *dict = fopen(f,"r");
  char word[80];
  DWORD ret, i = 1;

  if (!dict) { 
    printf("fatal: couldn't open dictionary %s\n",f);
    exit(1);
  }

  if (!strlen(u)) {
    printf("fatal: what user are we bruteforcing?\n");
    exit(1);
  }

  while (fgets(word,80,dict)) {
    word[strlen(word)-1] = '\0';
    printf("(%d) %s | %s\n",i,u,&word);

    ret = WNetAddConnection2(r,(const char *)&word,(const char*)u,0);
    if (ret == ERROR_SUCCESS) {
      printf("password found: %s\n",word);
      break;
    } else {
      describe(ret);
    }
    i++;
  }
}


void describe(DWORD err) {
  LPTSTR msgbuf;

  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, 0, err,
		MAKELANGID(0, SUBLANG_ENGLISH_US), (LPTSTR)&msgbuf, 0, 0);

  CharToOem(msgbuf, msgbuf);

  printf("return %d, %s",err,(char*)msgbuf);
  LocalFree(msgbuf);
}


void cancelsession(char *path) {
  char curuser[UNLEN] = "";
  DWORD size = sizeof(curuser);

  if (WNetGetUser(path,curuser,&size) == NO_ERROR) {
    printf("connected as %s, disconnecting... ",curuser);
    if (WNetCancelConnection2(path,0,TRUE) == NO_ERROR) 
      printf("success.\n");
    else printf("fail, continuing anyway.\n");
  }
}	
