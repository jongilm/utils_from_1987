#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

void main(void)
{
   union REGS inregs,outregs;
   int error,status,cyls,sects,heads,disks;
   size_t i,j;
   long unsigned space;
   char tempstr[40];
   char spacestr[40];

   inregs.h.ah = 0x08;
   inregs.h.dl = 0x80; /* drive 0 */
   int86(0x13,&inregs,&outregs);
   error  = outregs.x.cflag;
   if (error)
      printf("Error");
   else
   {
      status = outregs.h.ah;
      cyls   = ((outregs.h.cl & 0x00C0)<<2) + outregs.h.ch + 1;
      sects  = outregs.h.cl & 0x3F;
      heads  = outregs.h.dh + 1;
      disks  = outregs.h.dl;
      space  = (long unsigned)heads * (long unsigned)cyls * (long unsigned)sects * 512L;
      ltoa(space,tempstr,10);
      strrev(tempstr);
      /*memset(spacestr,'\0',40);*/
      for (i=0,j=0;i<=strlen(tempstr);i++,j++)
      {
         if (i && (i%3 == 0)) spacestr[j++] = ',';
         spacestr[j] = tempstr[i];
      }
      strrev(spacestr);

      printf("status = %d\n",status);
      printf("disks  = %d\n\n",disks );
      printf("cyls   = %d\n",cyls  );
      printf("sects  = %d\n",sects );
      printf("heads  = %d\n\n",heads );
      printf("space  = %s bytes (assuming 512 bytes/sector)\n",spacestr);

   }

}
