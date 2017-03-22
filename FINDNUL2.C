
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

void usage(void)
{
   printf("USAGE : FindNul <wildfilespec>\n");
   exit(1);
}

int main ( int argc, char *argv[] )
{
   FILE *f;
   int ch;
   char full[_MAX_PATH];

   argv++;
   argc--;

   if (!argc) usage();

   while ( argc )
   {
      f = fopen(*argv,"rt");
      if (f)
      {
         if (!feof(f))
         {
            ch = fgetc(f);
            //if (feof(f)) break;

            if (ch==0x00)
                printf("%s\n", strlwr(_fullpath(full,*argv,_MAX_PATH)));
         }
         fclose(f);
      }
      argv++;
      argc--;
   }
   return(0);
}
