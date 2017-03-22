
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

int main ( int argc, char *argv[] )
{
   char _far *p1;
   char _far *p2;

   p1 = _dos_getvect(0x61);
   _dos_setvect(0x61,0);
   p2 = _dos_getvect(0x61);

   printf("Interrupt 0x61 was %p\n",p1);
   printf("          Reset to %p\n",p2);
   return(0);
}
