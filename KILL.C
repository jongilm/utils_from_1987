
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <os2.h>

unsigned short pascal far DosKillProcess( unsigned short usScope, unsigned short pidProcess);

void main(int argc, char **argv)
{
   unsigned short pid;
   if (argc == 2)
   {
      sscanf(argv[1],"%x",&pid);
      printf("Killing %4.4X ...",pid);
      if (DosKillProcess(0,atoi(argv[1])) == 0)
	 puts("OK");
      else
	 puts("Error");
   }
}
