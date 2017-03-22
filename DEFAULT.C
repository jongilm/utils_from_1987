
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

FILE *f;
char str[256];
char *ptr;
long pos;
char arg[9] = "        ";

int main ( int argc, char *argv[] )
{
   if (argc!=2)
   {
      puts("must have 1 arg\n");
      return(1);
   }
   if (strlen(argv[1])>8)
   {
      puts("length of arg must <= 8\n");
      return(1);
   }
   memcpy(arg,argv[1],strlen(argv[1]));

   f = fopen("c:\\config.sys","r+t");
   if (!f)
   {
      puts("cannot open config.sys\n");
      return(1);
   }
   pos=ftell(f);
   fgets(str,255,f);
   while (!feof(f))
   {
      if (!memicmp(str,"menudefault=",12))
      {
         if ( str[22] != '\n')
         {
            fclose(f);
            puts("line must have format\"menudefault=xxxxxxxx,n\"\n");
            return(1);
         }

         memcpy(str+12,arg,8);

         fseek(f,pos,SEEK_SET);
         fputs(str,f);
         fclose(f);
         str[strlen(str)-1] = 0;
         //printf("Default CONFIG set to \"%s\"\n",argv[1]);
         return(0);
      }
      pos=ftell(f);
      fgets(str,255,f);
   }

   fclose(f);
   puts("cannot find line with format \"menudefault=xxxxxxxx,n\"\n");
   return(1);
}
