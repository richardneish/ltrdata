#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <process.h>
#  include <wsafenew.h>
#  define sleep(n)    Sleep(n*1000)
#else
#  include <unistd.h>
#  define _USERENTRY
typedef int HANDLE;
#endif

#define N             5
#define Busy_Eating   1
#define Busy_Thinking 1
#define Left(p)       (p)
#define Right(p)      (((p) + 1) % N)

typedef HANDLE * semaphore;

semaphore chopstick[N];

semaphore make_semaphore( void );
void signal( semaphore );
void philosopher( void* );

int main( int, char **, char** )
{
  for( int i=0; i < N; ++i )
  {
    chopstick[i] = make_semaphore();
    signal( chopstick[i] );
  }

  for( int i=0; i < N; ++i )
#ifdef _WIN32
    _beginthread( philosopher, 4096, &((*new int)=i) );
  ExitThread(0);
#else
    if( fork() )
      philosopher( &i );
#endif
}

semaphore make_semaphore( void )
{
  semaphore sema = new HANDLE[2];
#ifdef _WIN32
  CreatePipe( &sema[0], &sema[1], NULL, NULL );
#else
  pipe( sema );
#endif
  return sema;
}

void waitsema( semaphore s )
{
  char c;

#ifdef _WIN32
  DWORD dw;
  if( !ReadFile( s[0], &c, 1, &dw, NULL ) )
#else
  if( read( s[0], &c, 1 ) != 1 )
#endif
  {
    fputs("ERROR: waitsema() failed, check semaphore creation.\r\n", stderr);
    exit( 1 );
  }
}

void signal( semaphore s )
{
#ifdef _WIN32
  DWORD dw;
  if( !WriteFile( s[1], "x", 1, &dw, NULL ) )
#else
  if( write( s[1], "x", 1 ) != 1 )
#endif
  {
    fputs("ERROR: signal() failed, check semaphore creation.\r\n", stderr);
    exit( 1 );
  }
}

void pick_up( int me )
{
  if( !me )
  {
    waitsema( chopstick[Right(me)] );
    printf("Philosopher %i picks up right chopstick.\n", me);
    sleep( 1 );
    waitsema( chopstick[Left(me)] );
    printf("Philosopher %i picks up left chopstick.\n", me);
  }
  else
  {
    waitsema( chopstick[Left(me)] );
    printf("Philosopher %i picks up left chopstick.\n", me);
    sleep( 1 );
    waitsema( chopstick[Right(me)] );
    printf("Philosopher %i picks up right chopstick.\n", me);
  }
}

void put_down( int me )
{
  signal( chopstick[Left(me)] );
  printf("Philosopher %i puts left chopstick.\n", me);
  signal( chopstick[Right(me)] );
  printf("Philosopher %i puts right chopstick.\n", me);
}

void philosopher( void *meptr )
{
  int me = *(int*)meptr;

  for( int i=1; ; ++i )
  {
    pick_up( me );
    printf("Philosopher %i eating for the %i%s time.\n",
	   me, i, i==1 ? "st" : i==2 ? "nd" : i==3 ? "rd" : "th");

    sleep( Busy_Eating );
    put_down( me );
    printf("Philosopher %i thinking.\n", me);
    sleep( Busy_Thinking );
  }
}

