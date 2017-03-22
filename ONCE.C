
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>
#include <time.h>

int main ( int argc, char *argv[] )
{
   int   execute = 0;
   char  newdate[10];
   char  olddate[10];
   FILE *f;

   _strdate(newdate);

   f = fopen("c:\\once.dat","rb");
   if (f==NULL)
      execute = 1;
   else
   {
      if (fgets(olddate,10,f) == NULL) execute = 1;
      fclose(f);
      if (strcmp(olddate,newdate)) execute = 1;
   }

   if ((argc == 2) && (execute)) system(argv[1]);

   if (execute)
   {
      f = fopen("c:\\once.dat","wb");
      if (f==NULL)
      {
         puts("Error creating c:\\once.dat");
         exit(1);
      }

      if (fputs(newdate,f))
         puts("Error writing to c:\\once.dat");

      fclose(f);
   }
   return(0);
}
