#include <stdio.h>
#include <dos.h>

void main(void)
{
   unsigned int far *sports;
   unsigned int far *pports;

   FP_SEG(sports) = 0x0000;
   FP_OFF(sports) = 0x0400;
   FP_SEG(pports) = 0x0000;
   FP_OFF(pports) = 0x0408;

   printf("COM1:%4.4X         LPT1:%4.4X\n",*(sports+0),*(pports+0));
   printf("COM2:%4.4X         LPT2:%4.4X\n",*(sports+1),*(pports+1));
   printf("COM3:%4.4X         LPT3:%4.4X\n",*(sports+2),*(pports+2));
   printf("COM4:%4.4X         LPT4:%4.4X    (c) Computer Lab Jhb 1990\n",*(sports+3),*(pports+3));
}
