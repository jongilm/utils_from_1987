#include <stdlib.h>
#include <stdio.h>
#include <process.h>

int main(int argc, char *argv[]);

int main(int argc, char *argv[])
{
  int result = 0;

  if (argc > 1)
     result = spawnlp(P_WAIT,argv[1],argv[1],argv[2],argv[3],argv[4],NULL);

  printf("Result code = %d\n",result);

  return(result);
}
