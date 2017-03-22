#include <stdlib.h>
#include <stdio.h>
#include <dos.h>

typedef unsigned char VGA[25][80][2];

VGA *vga = (unsigned char *)(0xB8000000L);
unsigned char *shift = (unsigned char *)0x00000417L;
unsigned intno;
void (_interrupt _far *oldint)(void);
long i;
int toggle = 0;

void _interrupt _far newint(void)
{
   unsigned int port60;  /* was unsigned char - Jonnie, Wed 22-Jun-1994, 00:36:56 */
   int i;
   static int y = 0;
   char str[9];
   /*
   vga++;
   *(vga) = toggle ? 0x17:0x47;
   vga++;
   if (vga>(unsigned char *)(0xb8000000L+3999L))
   {
      vga = (unsigned char *)0xB8000000L;
      toggle = !toggle;
   }
   */
   _asm
   {
      in ax,60h
      mov port60,ax
   }
   itoa(port60,str,16);
   i = 0;
   while(str[i++]) (*vga)[y][i][0] = str[i];
   y++;
   _chain_intr(oldint);
}

void main(int argc, char *argv[])
{
   _asm
   {
      mov ah,2
      mov bh,0
      mov dx,0
      int 10h     // gotoxy(0,0);
      mov ah,6
      mov al,0
      mov bh,07h
      mov cx,0
      mov dh,24
      mov dl,79
      int 10h     // cls to 0x07
   }

   if (argc<2) return;
   sscanf(argv[1],"%x",&intno);
   oldint = _dos_getvect(intno);
   printf("Interrupt 0x%2.2X [%p]\n",intno,oldint);
   //printf(" Now setting it to %p\n",newint);

   _dos_setvect(intno,newint);

   if (argc==3) system(argv[2]);
   else         for (;;) if (*shift & 0x01) break;

   _dos_setvect(intno,oldint);
   //printf("    Restored it to %p\n",oldint);
}
