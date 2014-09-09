#include <stdio.h>

#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <process.h>
# define sleep(n) Sleep(n*1000)
# pragma comment(lib, "kernel32")
# if defined(_DLL) && !defined(_WIN64)
#   pragma comment(lib, "nullcrt")
# endif
#else
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/sem.h>
# include <pthread.h>
# define SEMKEY        IPC_PRIVATE   // Semafornyckel
#endif

#define N             5    // Antal filosofer
#define Busy_Eating   1    // Antal sekunder f�r att �ta
#define Busy_Thinking 1    // Antal sekunder f�r att t�nka
#define Busy_WashingUp 1   // Antal sekunder f�r att diska
#define Wait_Chopstick 1   // Antal sekunder mellan tv� gafflar
#define Left(p)       (p)  // �vers�ttning fr�n gaffel till semafornummer
#define Right(p)      (((p) + 1) % N)

#ifdef _WIN32
HANDLE semaphores[N];
#else
int semaphores = -1;       // Variabeln med id-nummer p� semaforgruppen
#endif

void wait(int);            // Funktionen som v�ntar p� en gaffel
void signal(int);          // Funktionen som frig�r en gaffel
void philosopher(void*);   // Funktionen som startar en filosof efter fork

int main()
{
  // Skapa semaforgrupp
#ifdef _WIN32
  for( int i = 0; i < N; i++ )
    {
      if( (semaphores[i] = CreateSemaphore(NULL, 1, 1, NULL)) == NULL )
        {
          fputs("Can't create semaphores", stderr);
          exit(0);
        }

      _beginthread(philosopher, 4096, &(*(new int)=i));
    }

  ExitThread(0);
#else
  if( (semaphores = semget(SEMKEY, N, 0666|IPC_CREAT)) < 0 )
    {
      perror("Can't create semaphores");
      exit(0);
    }

  union semun
  {
      int val;
      semid_ds *buf;
      ushort *array;
  } semarg;

  // Ettst�ll alla semaforer
  semarg.val = 1;
  pthread_t tid;
  for( int i=0; i < N; i++ )
    {
      if( semctl(semaphores, i, SETVAL, semarg) < 0)
	perror("Can't set semaphore value");
      else
	pthread_create(&tid, NULL, (void*(*)(void*))philosopher, &(*(new int)=i));
    }

  pthread_exit(0);
#endif
}

void wait(int s)
{
  // F�rs�k att ta kontroll �ver en gaffel, v�nta om redan upptagen
#ifdef _WIN32
  WaitForSingleObject(semaphores[s], INFINITE);
#else
  sembuf op;
  op.sem_num = s;
  op.sem_op = -1;
  op.sem_flg = 0;
  semop(semaphores, &op, 1);
#endif
}

void signal(int s)
{
  // Sl�pp kontrollen �ver en gaffel
#ifdef _WIN32
  ReleaseSemaphore(semaphores[s], 1, NULL);
#else
  sembuf op;
  op.sem_num = s;
  op.sem_op = 1;
  op.sem_flg = 0;
  semop(semaphores, &op, 1);
#endif
}

void pick_up(int me)
{
  // L�t varannan filosof f�rs�ka med h�ger gaffel f�rst, och varannan filosof
  // med v�nster gaffel f�rst.
  if( me & 1 )
    {
      wait(Right(me));
      printf("Philosopher %i picks up right chopstick.\n", me);
      sleep(Wait_Chopstick);
      wait(Left(me));
      printf("Philosopher %i picks up left chopstick.\n", me);
    }
  else
    {
      wait(Left(me));
      printf("Philosopher %i picks up left chopstick.\n", me);
      sleep(Wait_Chopstick);
      wait(Right(me));
      printf("Philosopher %i picks up right chopstick.\n", me);
    }
}

void put_down(int me)
{
  // Sl�pp kontrollen �ver en filosofs gafflar
  signal(Left(me));
  printf("Philosopher %i puts left chopstick.\n", me);
  signal(Right(me));
  printf("Philosopher %i puts right chopstick.\n", me);
}

void philosopher(void *meptr)
{
  int me = *(int*)meptr;

  for( int i=1; ; i++ )
  {
    // Lyft upp gafflarna som tillh�r den h�r filosofen, �t, diska, och t�nk
    pick_up(me);
    printf("Philosopher %i eating for the %i%s time.\n", me, i,
	   i==1 ? "st" : i==2 ? "nd" : i==3 ? "rd" : "th");
    sleep(Busy_Eating);
    printf("Philosopher %i washing up.\n", me);
    sleep(Busy_WashingUp);

    put_down(me);
    printf("Philosopher %i is thinking.\n", me);
    sleep(Busy_Thinking);
    printf("Philosopher %i is hungry.\n", me);
  }
}

