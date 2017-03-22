
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

int main ( int argc, char *argv[] )
{
   int val;

   if (argc != 2)
   {
      printf("hexdec <number>[h]\n");
      return(0);
   }

   if ( (argv[1][strlen(argv[1])-1] == 'h') ||
        (argv[1][strlen(argv[1])-1] == 'H') )
   {
      argv[1][strlen(argv[1])-1] = 0;
      if (sscanf(argv[1],"%x",&val) != 1)
      {
         printf("Invalid hex number\n");
         return(0);
      }
   }
   else
   {
      if (sscanf(argv[1],"%d",&val) != 1)
      {
         printf("Invalid decimal number\n");
         return(0);
      }
   }


   printf(" DECIMAL: %5u",val);
   if (val<0)
      printf(" (%d)",val);
   printf("\n");
   printf("     HEX: %4Xh",val);
   if (val<0)
      printf(" (-%Xh)",-val);
   printf("\n");
   return(0);
}
