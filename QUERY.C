#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void main(int argc, char *argv[])
{
  if (argc > 1) printf("%s=%s\n",strupr(argv[1]),getenv(strupr(argv[1])));
}
