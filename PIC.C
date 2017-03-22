
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

#define IMR     0x21  /* interrupt mask reg */

int main ( void )
{
   unsigned char imr;

   imr = inp(IMR);
   printf("IMR (port 21h) = %2.2Xh\n\n",imr);

   printf("   IRQ  7 6 5 4 3 2 1 0\n");
   printf("MASKED  %c %c %c %c %c %c %c %c\n",imr&0x80?'X':' '
                                             ,imr&0x40?'X':' '
                                             ,imr&0x20?'X':' '
                                             ,imr&0x10?'X':' '
                                             ,imr&0x08?'X':' '
                                             ,imr&0x04?'X':' '
                                             ,imr&0x02?'X':' '
                                             ,imr&0x01?'X':' '
                                             );
   return(imr);
}
