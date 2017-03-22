#include <stdlib.h>
#include <stdio.h>
#include <dos.h>

int main(int argc, char *argv[]);

int main(int argc, char *argv[])
/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
�                                                                            �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸
*/
{
   union REGS inregs,outregs;
/* struct SREGS segregs;*/
   unsigned char rate = 0;
   unsigned char delay = 0;

   if (argc == 2 && argv[1][1] == '?')
   {
      printf("Usage : KBDRATE [Rate] [Delay]\n");
      printf("        Rate  = 0..30 (fast..slow)  default=0\n");
      printf("        Delay = 0..3  (short..long) default=0\n");
      printf("\n");
      printf("(c) Computer Lab Jhb 1989.  All rights reserved.\n");
      return(0);
   }

   if (argc>=2)
      rate  = (unsigned char)atoi(argv[1]);

   if (argc>=3)
      delay = (unsigned char)atoi(argv[2]);

   inregs.h.ah = 0x03;
   inregs.h.al = 0x05;
   inregs.h.bl = rate;
   inregs.h.bh = delay;
   int86(0x16,&inregs,&outregs);

   return(0);
}


