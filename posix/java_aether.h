#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>  /* inet_ntoa inet_addr */
#include<fcntl.h>

typedef unsigned long long int  doublecell ;

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

EXTERN_C int open_aether_connection()
/*
 *
 * returns -1 on failure and prints a message
 * returns 0  on success and opens a multicast udp socket 
*/
;

EXTERN_C int  close_aether_connection (void)
/* should also reclaim address structure */
;

EXTERN_C int  read_from_aether_connection (doublecell *buffer,int buffer_size)
/*
   *int recvfrom(int s,void *buf,int len,unsigned int flags,struct sockaddr *from,int *fromlen);
   *
   * If the socket is non-blocking ReadSocket and empty string if the socket is empty.
   * If the socket is blocking and there is an error its also returns an empty string. 
   * From_socket holds the address of who sent the message, this can be null if 
   * we are not interested. 
*/
;

EXTERN_C int write_to_aether_connection (doublecell *msg,int msglen)
/* should check max length and the msg size */
;
