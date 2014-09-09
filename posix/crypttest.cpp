#include <iostream.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int rsacrypt(int,int,int);

main(int argc, char **argv)
{
  if( argc < 3 )
    {
      cerr << "Syntax: " << argv[0] << " P Q" << endl;
      return -1;
    }

  const int P=atoi(argv[1]), Q=atoi(argv[2]);
  cout << "P=" << P << ", Q=" << Q << endl;
  
  int N = P * Q;  
  int G = (P-1)*(Q-1);
  cout << "N=" << N << ", G=" << G << endl;

  srand(time(NULL));
  int E=0;
  for(;;)
    {
      E=((rand()%G)&0xFFFFFFFE)+1;
      if( (G % E != 0) & (E != P) & (E != Q) )
	break;
      cout << "Tested E=" << E << ", not valid..." << endl;
    }
     
  if( E==0 )
    {
      cerr << "Can't calculate valid E." << endl;
      return 1;
    }

  cout << "E=" << E << endl;

  int D=0;
  while( ++D )
    if( (D * E % G == 1) & (D != E) & (D != P) & (D != Q) )
      break;

  if( D==0 )
    {
      cerr << "Can't calculate valid D." << endl;
      return 1;
    }

  cout << "D=" << D << endl;

  for(;;)
    {
      cout << "Enter value: " << endl;
      char cbuf[20];
      cin.get(cbuf,sizeof(cbuf));
      while(cin.get()!='\n');
      int i=atoi(cbuf);

      cout << "Encrypted " << i << " is " << rsacrypt(i,E,N) << endl;
     
      cout << "Enter value: " << endl;
      cin.get(cbuf,sizeof(cbuf));
      while(cin.get()!='\n');
      i=atoi(cbuf);

      cout << "Decrypted " << i << " is " << rsacrypt(i,D,N) << endl;
    }
}

int rsacrypt(int BC, int ED, int N)
{
  unsigned long long ST = 1;
  while( ED-- )
    ST *= BC;

  ST %= N;

  return ST;
}
