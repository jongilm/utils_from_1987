#include <stdio.h>
#include <dos.h>

#define SCANCODES
#include <tools.h>

union REGS inregs,outregs;
int i;

void main(void)
{
   printf("Keyboard Scan Utility - Press <ESC> to terminate\n\n");

   printf("�����������������Ŀ\n");
   printf("�ASCII�SCAN �CHAR �\n");

   while (outregs.h.al != 27)
   {
      inregs.h.ah=0x00;
      int86(0x16,&inregs,&outregs);
      for (i=0;i<SCANCODES;i++) if (ScanCode[i].AX_083_Std == outregs.x.ax) break;
      printf("�����������������Ĵ\n");
      printf("� %2.2X  � %2.2X  �  %1c  � %-s\n",outregs.h.al,outregs.h.ah,iscntrl(outregs.h.al) ? '*' : outregs.h.al,ScanCode[i].Description);
   }
   printf("�������������������\n");
}
