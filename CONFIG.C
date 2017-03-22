
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

#define ON  1
#define OFF 0

#define CENTER   0
#define TOP      1
#define BOTTOM   2

#define LEFT    0
#define RIGHT   1
#define SKIP    2
#define OR      3

#define NORMALVIDEO      0x89
#define COMPRESSEDVIDEO  0x8F
#define CENTERING        0x90
#define DOT720FIXUP      0x91
#define AUTOMAPPING      0x8C
#define STANDARDFONT     0x8B
#define CRT              0x92
#define CGASOMETHING     0x86

void altselect(unsigned int function, unsigned int value)
{
   unsigned char fun;
   unsigned char val;

   fun = (unsigned char)function;
   val = (unsigned char)value;

   _asm mov ah,0x12
   _asm mov al,val
   _asm mov bh,0x00
   _asm mov bl,fun
   _asm int 10h
}

void usage(void)
{
   printf("\n");
   printf("USAGE: CONFIG [options]\n");
   printf("options:  ?\n");
   printf("          DEFAULTS\n");
   printf("          REVERSE    NORMAL\n");
   printf("          EXPANDED   COMPRESSED\n");
   printf("          CENTER     TOP       BOTTOM\n");
   printf("          LEFT       RIGHT      SKIP      OR\n");
   printf("          AUTO       MANUAL\n");
   printf("          STANDARD   BOLD\n");
   printf("          LCD        CRT\n");
   printf("         (defaults)\n");
   exit(0);
}

void setdefaults(void)
{
   altselect(NORMALVIDEO     ,OFF    );
   altselect(COMPRESSEDVIDEO ,OFF    );
   altselect(CENTERING       ,CENTER );
   altselect(DOT720FIXUP     ,LEFT   );
   altselect(AUTOMAPPING     ,ON     );
   altselect(STANDARDFONT    ,ON     );
   altselect(CRT             ,OFF    );
}

int main ( int argc, char *argv[] )
{
   int i;
   int valid;

   printf("Processing... ");
   if (argc<2)
   {
      printf("Defaults:  REVERSE EXPANDED CENTER LEFT AUTO STANDARD LCD, ");
      setdefaults();
   }
   for (i=1;i<argc;i++)
   {
      valid = 1;
      strupr(argv[i]);
      if      (!strcmp(argv[i],"?"         )) usage();
      if      (!strcmp(argv[i],"/?"        )) usage();
      if      (!strcmp(argv[i],"-?"        )) usage();

      if      (!strcmp(argv[i],"DEFAULTS"  )) setdefaults();
      else if (!strcmp(argv[i],"NORMAL"    )) altselect(NORMALVIDEO    ,ON     );
      else if (!strcmp(argv[i],"NOREVERSE" )) altselect(NORMALVIDEO    ,ON     );
      else if (!strcmp(argv[i],"REVERSE"   )) altselect(NORMALVIDEO    ,OFF    );

      else if (!strcmp(argv[i],"COMPRESSED")) altselect(COMPRESSEDVIDEO,ON     );
      else if (!strcmp(argv[i],"EXPANDED"  )) altselect(COMPRESSEDVIDEO,OFF    );

      else if (!strcmp(argv[i],"CENTER"    )) altselect(CENTERING      ,CENTER );
      else if (!strcmp(argv[i],"TOP"       )) altselect(CENTERING      ,TOP    );
      else if (!strcmp(argv[i],"BOTTOM"    )) altselect(CENTERING      ,BOTTOM );

      else if (!strcmp(argv[i],"LEFT"      )) altselect(DOT720FIXUP    ,LEFT   );
      else if (!strcmp(argv[i],"RIGHT"     )) altselect(DOT720FIXUP    ,RIGHT  );
      else if (!strcmp(argv[i],"SKIP"      )) altselect(DOT720FIXUP    ,SKIP   );
      else if (!strcmp(argv[i],"OR"        )) altselect(DOT720FIXUP    ,OR     );

      else if (!strcmp(argv[i],"AUTO"     )) altselect(AUTOMAPPING     ,ON     );
      else if (!strcmp(argv[i],"MANUAL"   )) altselect(AUTOMAPPING     ,OFF    );
      else if (!strcmp(argv[i],"NOAUTO"   )) altselect(AUTOMAPPING     ,OFF    );

      else if (!strcmp(argv[i],"STANDARD" )) altselect(STANDARDFONT    ,ON     );
      else if (!strcmp(argv[i],"NOBOLD"   )) altselect(STANDARDFONT    ,ON     );
      else if (!strcmp(argv[i],"BOLD"     )) altselect(STANDARDFONT    ,OFF    );

      else if (!strcmp(argv[i],"CRT"      )) altselect(CRT             ,ON     );
      else if (!strcmp(argv[i],"LCD"      )) altselect(CRT             ,OFF    );
      else valid = 0;
      if (valid)
         printf("%s, ",argv[i]);
      else
         printf("%s [invalid], ",argv[i]);
   }
   printf("\b\b \n");
   return(0);
}


/*
   _asm
   {
      push ax
      push bx
      push cx
      push dx
      push es
      push ds
      push si

      mov bx,00089h
      //mov cx,0FF03h
      //mov dx,003C5h

      //mov ax,00040h
      //mov es,ax
      //mov ax,00364h
      //mov ds,ax
      //mov si,01cbbh

      mov ax,01201h
      int 10h

      pop si
      pop ds
      pop es
      pop dx
      pop cx
      pop bx
      pop ax
   }
*/
