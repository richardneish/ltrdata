#include <iostream.h>
#include <time.h>

main()
{
	char *timestr;
	
	timestr = ctime( &((*new time_t) = time( NULL )) );
	
	timestr += 11;
	timestr[8] = '\x0';
	
	cout << timestr << endl;
}
