#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>

char *str1 = "String1";
char str2[11];
char *str2p;

void main(void)
{
   char *str3 = "String3";
   char str4[11];
   char *str4p;


   strcpy(str2,"String2");
   str2p = str2;

   printf("str1 : s=%s   p=%p   size=%2d   SEG=%4.4X   OFF=%4.4X\n",str1 ,str1 ,sizeof(str1 ),FP_SEG(str1 ),FP_OFF(str1 ));
   printf("str2 : s=%s   p=%p   size=%2d   SEG=%4.4X   OFF=%4.4X\n",str2 ,str2 ,sizeof(str2 ),FP_SEG(str2 ),FP_OFF(str2 ));
   printf("str2p: s=%s   p=%p   size=%2d   SEG=%4.4X   OFF=%4.4X\n",str2p,str2p,sizeof(str2p),FP_SEG(str2p),FP_OFF(str2p));

   printf("\n");

   strcpy(str4,"String4");
   str4p = str4;

   printf("str3 : s=%s   p=%p   size=%2d   SEG=%4.4X   OFF=%4.4X\n",str3 ,str3 ,sizeof(str3 ),FP_SEG(str3 ),FP_OFF(str3 ));
   printf("str4 : s=%s   p=%p   size=%2d   SEG=%4.4X   OFF=%4.4X\n",str4 ,str4 ,sizeof(str4 ),FP_SEG(str4 ),FP_OFF(str4 ));
   printf("str4p: s=%s   p=%p   size=%2d   SEG=%4.4X   OFF=%4.4X\n",str4p,str4p,sizeof(str4p),FP_SEG(str4p),FP_OFF(str4p));
}
