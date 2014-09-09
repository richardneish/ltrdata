#include <iostream.h>
#include "java_aether.h"

int main()
{
  doublecell aether_data[3];
  open_aether_connection();
  read_from_aether_connection(aether_data, sizeof(aether_data)/sizeof(doublecell));
  close_aether_connection();
  for (int i=0; i < sizeof(aether_data)/sizeof(doublecell); i++)
    cout << "Data " << i << " is " << aether_data[i] << endl;
}
