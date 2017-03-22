#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>

#include <tools.h>

int main(int argc, char *argv[]);
int getmode(void);

int main(int argc, char *argv[])
{
   int mode;
   unsigned int maxcol;
   unsigned int col;
   unsigned int row;
   FILE *outfile;
   char far *chp;
   char filename[13];

   switch(mode = getmode())
   {
      case 0:
      case 1:    FP_SEG(chp) = 0xb800;
                 maxcol = 40;
                 break;
      case 2:
      case 3:    FP_SEG(chp) = 0xb800;
                 maxcol = 80;
                 break;
      case 7:    FP_SEG(chp) = 0xb000;
                 maxcol = 80;
                 break;
      default:   printf("Unknown video mode \"%d\"",mode);
                 exit(1);
   }
   if (argc != 2)
   {
       printf("Usage : JDump <filename>");
       exit(2);
   }
   strcpy(filename,argv[1]);
   if ((outfile = fopen(filename,"w")) == NULL)
   {
       printf("Error opening file \"%s\"",filename);
       exit(3);
   }
   for (row=0;row<25;row++)
   {
      for (col=0;col<maxcol;col++)
      {
         FP_OFF(chp) = (row*maxcol+col)*2;
         fputc(*chp,outfile);
      }
      fputc(10,outfile);
   }
   if (fclose(outfile))
   {
       printf("Error closing file \"%s\"",filename);
       exit(4);
   }
   printf("Successful screen dump to \"%s\"",filename);
   return(0);
}

int getmode(void)
/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³ GetMode                                                                    ³
³                                                                            ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
{
  union REGS inregs,outregs;

  inregs.h.ah=15;
  int86(0x10,&inregs,&outregs);

  return(outregs.h.al);
}


