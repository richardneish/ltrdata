#include <iostream.h>
#include "java_aether.h"

int main()
{
  doublecell aether_data[] = { 4, 3, 2 };
  open_aether_connection();
  write_to_aether_connection(aether_data, sizeof(aether_data)/sizeof(doublecell));
  close_aether_connection();
}
