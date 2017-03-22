#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <string.h>

main()
{
   union REGS inregs,outregs;
   unsigned int far *envseg;
   char far *env;
   int size;

   inregs.h.ah = 0x62;
   int86(0x21,&inregs,&outregs);

   FP_SEG(envseg) = outregs.x.bx;
   FP_OFF(envseg) = 0x2C;

   FP_SEG(env) = *envseg;
   FP_OFF(env) = 0;

   size = 0;
   while(strlen(env) != 0)
   {
/*      printf("%s\n",env);  */
      size += strlen(env) + 1;
      env += strlen(env) + 1;
   }
   size += 1; /* for trailing null */
   printf("Environment size = %d\n",size);
}
