/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³                                                                            ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <tools.h>

#define ENDOFMEM 0xA000 /*65536*/  /* top of mem. (640k) less one page (1920) */
#define PAGESEGS 0x40   /*64*/     /*120 segments = 1920 bytes = 24 lines of chars*/
#define LINES    0x10   /*16*/
#define LINESEGS 0x04   /*4 */     /*  5 segments =   80 bytes =  1 line  of chars*/
#define CHARSZ          /*1 */     /*  1 char */

unsigned int screentype;

int key;
int row;
int col1;
int col2;
unsigned long int addr;
int count;
char *top_ptr;
char *scr_ptr;
char *mem_ptr;

int read_keyboard(void);
void main(void);

void main()
/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³                                                                            ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
{
   clrscr(7);
   gotoxy(1,1);
   printf("DRAM - Memory Browse Utility                               Jonathan Gilmore 1989");
   gotoxy(1,4);
   printf("   ÚÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄ¿\n");
   printf("   ³        ³        ³        ³        ³        ³        ³        ³        ³\n");
   printf("   ³        ³        ³        ³        ³        ³        ³        ³        ³\n");
   printf("   ³        ³        ³        ³        ³        ³        ³        ³        ³\n");
   printf("   ³        ³        ³        ³        ³        ³        ³        ³        ³\n");
   printf("   ³        ³        ³        ³        ³        ³        ³        ³        ³\n");
   printf("   ³        ³        ³        ³        ³        ³        ³        ³        ³\n");
   printf("   ³        ³        ³        ³        ³        ³        ³        ³        ³\n");
   printf("   ³        ³        ³        ³        ³        ³        ³        ³        ³\n");
   printf("   ³        ³        ³        ³        ³        ³        ³        ³        ³\n");
   printf("   ³        ³        ³        ³        ³        ³        ³        ³        ³\n");
   printf("   ³        ³        ³        ³        ³        ³        ³        ³        ³\n");
   printf("   ³        ³        ³        ³        ³        ³        ³        ³        ³\n");
   printf("   ³        ³        ³        ³        ³        ³        ³        ³        ³\n");
   printf("   ³        ³        ³        ³        ³        ³        ³        ³        ³\n");
   printf("   ³        ³        ³        ³        ³        ³        ³        ³        ³\n");
   printf("   ³        ³        ³        ³        ³        ³        ³        ³        ³\n");
   printf("   ÀÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÙ\n");
   gotoxy(1,24);
   printf("Press <Escape> to return");

   FP_SEG(top_ptr) = 0;
   FP_OFF(top_ptr) = 0;
   FP_SEG(scr_ptr) = getvseg();

   do
   {
      if (FP_OFF(top_ptr) == 0xFFFF)
      {
         FP_SEG(top_ptr)--;
         FP_OFF(top_ptr) = 0x000F;
      }
      if (FP_OFF(top_ptr) == 0x0010)
      {
         FP_SEG(top_ptr)++;
         FP_OFF(top_ptr) = 0x0000;
      }

      addr = (unsigned long int)((unsigned long int)FP_SEG(top_ptr)*0x10 + (unsigned long int)FP_OFF(top_ptr));
      gotoxy(4,3);
      printf("Addr = %p   Hex = %5lX   Dec = %7lu",top_ptr,addr,addr);

      FP_OFF(scr_ptr) = 640;

      mem_ptr = top_ptr;
      for (row=0;row<=15;row++)
      {
         scr_ptr += 8;
         for (col1=0; col1<8; col1++)
         {
            for (col2=0; col2<8; col2++)
            {
/*             FP_OFF(scr_ptr) = 160 + row*160 + col*2;  */
               *scr_ptr = *mem_ptr;
               scr_ptr += 2;
               mem_ptr++;
            }
            scr_ptr += 2;
         }
         scr_ptr += 8;
      }
   } while (read_keyboard() != 27);

   clrscr(7);
   return;
}

int read_keyboard()
/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³                                                                            ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
{
   key = getch();
   if (key == '\0')
   {
      key = getch();
      switch(key)
      {
      case 'Q' /* pgdn */ : FP_SEG(top_ptr) += PAGESEGS;  break;
      case 'I' /* pgup */ : FP_SEG(top_ptr) -= PAGESEGS;  break;
      case 'P' /* down */ : FP_SEG(top_ptr) += LINESEGS;  break;
      case 'H' /* up   */ : FP_SEG(top_ptr) -= LINESEGS;  break;
      case 'M' /* rght */ : FP_OFF(top_ptr)++; break;
      case 'K' /* left */ : FP_OFF(top_ptr)--; break;
      case 'G' /* home */ : FP_SEG(top_ptr) = 0;        FP_OFF(top_ptr) = 0; break;
      case 'O' /* end  */ : FP_SEG(top_ptr) = ENDOFMEM; FP_OFF(top_ptr) = 0; break;

      case ';' /* F1   */ : FP_SEG(top_ptr) = 0x1000;   FP_OFF(top_ptr) = 0;       break;
      case '<' /* F2   */ : FP_SEG(top_ptr) = 0x2000;   FP_OFF(top_ptr) = 0;       break;
      case '=' /* F3   */ : FP_SEG(top_ptr) = 0x3000;   FP_OFF(top_ptr) = 0;       break;
      case '>' /* F4   */ : FP_SEG(top_ptr) = 0x4000;   FP_OFF(top_ptr) = 0;       break;
      case '?' /* F5   */ : FP_SEG(top_ptr) = 0x5000;   FP_OFF(top_ptr) = 0;       break;
      case '@' /* F6   */ : FP_SEG(top_ptr) = 0x6000;   FP_OFF(top_ptr) = 0;       break;
      case 'A' /* F7   */ : FP_SEG(top_ptr) = 0x7000;   FP_OFF(top_ptr) = 0;       break;
      case 'B' /* F8   */ : FP_SEG(top_ptr) = 0x8000;   FP_OFF(top_ptr) = 0;       break;
      case 'C' /* F9   */ : FP_SEG(top_ptr) = 0x9000;   FP_OFF(top_ptr) = 0;       break;
      case 'D' /* F10  */ : FP_SEG(top_ptr) = 0xA000;   FP_OFF(top_ptr) = 0;       break;
      case 'T' /* s-F1 */ : FP_SEG(top_ptr) = 0xB000;   FP_OFF(top_ptr) = 0;       break;
      case 'U' /* s-F2 */ : FP_SEG(top_ptr) = 0xC000;   FP_OFF(top_ptr) = 0;       break;
      case 'V' /* s-F3 */ : FP_SEG(top_ptr) = 0xD000;   FP_OFF(top_ptr) = 0;       break;
      case 'W' /* s-F4 */ : FP_SEG(top_ptr) = 0xE000;   FP_OFF(top_ptr) = 0;       break;
      case 'X' /* s-F5 */ : FP_SEG(top_ptr) = 0xF000;   FP_OFF(top_ptr) = 0;       break;
      case 'Y' /* s-F6 */ : FP_SEG(top_ptr) = 0x0000;   FP_OFF(top_ptr) = 0;       break;
      }
   }
   else
   {
      switch(key)
      {
      case '+' /* pgdn */ : FP_SEG(top_ptr) += 0x1000;  break;
      case '-' /* pgup */ : FP_SEG(top_ptr) -= 0x1000;  break;
      }
   }

   return(key);
}


