#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>

#include <tools.h>

union REGS dosregs;
int button;
int x;
int y;
int buttons;

void mousecursorshow()
{
   dosregs.x.ax = 1;
   int86(0x33, &dosregs, &dosregs);
}

void mousecursorhide()
{
   dosregs.x.ax = 2;
   int86(0x33, &dosregs, &dosregs);
}

void mousegetposbut(int *buttonstatus, int *x, int *y)
{
   dosregs.x.ax = 3;
   int86(0x33, &dosregs, &dosregs);
   *buttonstatus = dosregs.x.bx;
   *x            = dosregs.x.cx;
   *y            = dosregs.x.dx;
}

int mousereset(int *buttons)
{
   unsigned long address;
   unsigned char firstbyte;

   address   = (unsigned long)_dos_getvect(0x33);
   firstbyte = (unsigned char)*(unsigned long _far *)(address);

   if (address == 0 || firstbyte == 0xCF) return(1);        /* 0xCF = IRET */
   else
   {
      dosregs.x.ax = 0;
      int86(0x33, &dosregs, &dosregs);
      switch ((int)dosregs.x.ax)
      {
         case  0 : return(2);   /* mouse not responding */
         case -1 : *buttons = dosregs.x.bx;
                   return(0);
      }
   }
   return(3);     /* unexpected reponse from driver */
}

void dowhat(void)
{
   int xx,yy;
   xx = x/8+1;
   yy = y/8+1;
   if (xx == 1 || xx == 80 || yy == 1 || yy == 25) return;
   _put_char(xx,yy,1,(button == 1) ? 'Û':' ');
   _put_attr(xx,yy,1,0x07);
   mousecursorhide();
   mousecursorshow();
}

void main(void)
{
   gotoxy(1,26);
   clrscr(0x17);
   box(1,1,80,25,0x1F,2);

   switch (mousereset(&buttons))
   {
      case 0 : printf("Mouse software loaded OK (%d buttons)",buttons);
               mousecursorshow();
               do
               {
                  mousegetposbut(&button, &x, &y);
                  /*gotoxy(x/8 + 1, y/8 + 1);*/ /* col and y in pixels */
                  if (button == 1 || button == 2) dowhat();
               } while (button != 3);
               mousecursorhide();
               break;
      case 1 : printf("Mouse software not loaded");
               break;
      case 2 : printf("Mouse not responding");
               break;
      case 3 : printf("Mouse software yeilding unexpected response");
               break;
   }
   clrscr(0x07);
   gotoxy(1,1);
}

