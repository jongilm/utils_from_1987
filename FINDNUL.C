
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

int main ( int argc, char *argv[] )
{
   FILE *f;
   int ch;
   long count;
   int quick = 0;
   int firstbyteonly = 0;

   if (argc<2) exit(0);
   argv++;
   argc--;

   if (!argc) return(1);

   if ((argv[0][0] == '-') || (argv[0][0] == '/'))
   {
      if ((argv[0][1] == 'q') || (argv[0][1] == 'Q')) quick = 1;
      if ((argv[0][1] == '1')) firstbyteonly = 1;
      argv++;
      argc--;
   }

   if (!argc) return(1);

   printf("Searching %d file(s)\n",argc);

   while ( argc )
   {
      count = 1L;
      if (!quick) printf("Searching %s\n",*argv);

      f = fopen(*argv,"rt");
      if (f)
      {
         while (!feof(f))
         {
            ch = ' ';
            ch = fgetc(f);
            //if (feof(f)) break;

            if (ch=='\n') count++;

            if ((ch==0x00) || (ch==0xFF))
            {
               if (ch==0x00)
                  printf("0x00 found in %s at line %ld\n",*argv,count);
               if (ch==0xFF)
                  printf("0xFF found in %s at line %ld\n",*argv,count);
               if (quick) break;
            }
            if (firstbyteonly) break;
         }
      }
      fclose(f);
      argv++;
      argc--;
   }
   return(0);
}
