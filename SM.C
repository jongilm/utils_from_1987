
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

#include <tools.h>

int main ( int argc, char *argv[] )
{
   char _far *ptr;
   unsigned int seg,ofs;
   int x,y,len;

   if (argc == 2)
   {
      len = strlen(argv[1]);
      printf("Searching for \"%s\"\n",argv[1]);
      seg=0;
      for (;;)
      {
	 FP_SEG(ptr)=seg;
	 FP_OFF(ptr)=0;

         ofs=0;
         for (;;)
         {
            FP_OFF(ptr)=ofs;
            if (!_fmemicmp(ptr,argv[1],len))
            {
               printf("\r%Fp",ptr);
               x = wherex()+2;
               y = wherey();
               _put_chars  ( x         , y, 60 , ptr-30+len/2);
               _put_attr   ( x         , y, 60 , 0x07);
               _put_attr   ( x+30-len/2, y, len, 0x0F);
               printf("\n");
            }
            if (ofs == 0xFFFF) break;
            ofs++;
         }
         if (seg == 0xF000) break;
         seg+=0x1000;
      }
   }
   return(0);
}
