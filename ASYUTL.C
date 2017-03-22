/*
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³ ASYUTL  ASYDVR Utility                            ³
;³                                                   ³
;³ Jonathan Gilmore, Computer Lab Jhb, 1991          ³
;³                                                   ³
;³ Extracts from "Interrupt driven communications"   ³
;³ by Jim Kyle, 1987, "The MS-DOS Encyclopedia".     ³
;³                                                   ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <dos.h>

#include <tools.h>

#define DOS    0x07
#define NORMAL 0x17
#define BRIGHT 0x1F
#define URGENT 0x1C

/* Driver status bits */
#define HWINT 0x0800  /* MCR, 1st word, HW ints gated */
#define o_DTR 0x0200  /* MCR, 1st word, output DTR    */
#define o_RTS 0x0100  /* MCR, 1st word, output RTS    */

#define m_PG  0x0010  /* LCR, 1st word, parity ON     */
#define m_PE  0x0008  /* LCR, 1st word, parity EVEN   */
#define m_XS  0x0004  /* LCR, 1st word, 2 stop bits   */
#define m_WL  0x0003  /* LCR, 1st word, wordlen mask  */

#define i_CD  0x8000  /* MSR, 2nd word, CD            */
#define i_RI  0x4000  /* MSR, 2nd word, RI            */
#define i_DSR 0x2000  /* MSR, 2nd word, DSR           */
#define i_CTS 0x1000  /* MSR, 2nd word, CTS           */

#define l_SRE 0x0040  /* LSR, 2nd word, Xmtr SR empty */
#define l_HRE 0x0020  /* LSR, 2nd word, Xmtr HR empty */
#define l_BRK 0x0010  /* LSR, 2nd word, Break received*/
#define l_ER1 0x0008  /* LSR, 2nd word, Framing error */
#define l_ER2 0x0004  /* LSR, 2nd word, Parity error  */
#define l_ER3 0x0002  /* LSR, 2nd word, Overrun error */
#define l_RRF 0x0001  /* LSR, 2nd word, Rcvr DR full  */

FILE *dvp;
int iobf[5];
int unit;
int ch = 0;

void paint_screen(void);
void update_screen(void);
void write_IOCTL(void);
void read_IOCTL(void);
char *yesno(int x);
char *onoff(int x);
void make_asy(int unit);

void main(void)
{
   char *savescr;
   int  savex,savey;

   savescr = pushwindow(1,1,80,25);
   savex = wherex();
   savey = wherey();
   paint_screen();

   unit = 1;
   dvp = fopen("ASY1","rb+");

   while ((ch != 'Q') && (ch != 27))
   {
      read_IOCTL();
      update_screen();
      ch = toupper(getch());
      switch(ch)
      {
         case '1' : /* select port 1 */
                    fclose(dvp);
                    dvp = fopen("ASY1","rb+");
                    unit = 1;
                    break;
         case '2' : /* select port 2 */
                    fclose(dvp);
                    dvp = fopen("ASY2","rb+");
                    unit = 2;
                    break;
         case 'I' : /* initialise MCR/LCR to 8N1 */
                    iobf[0] = (HWINT + o_DTR + o_RTS + m_WL);
                    write_IOCTL();
                    break;
         case 'B' : /* set baud rate */
                    if      (iobf[4] ==  300) iobf[4] = 1200;
                    else if (iobf[4] == 1200) iobf[4] = 2400;
                    else if (iobf[4] == 2400) iobf[4] = 9600;
                    else                      iobf[4] =  300;
                    write_IOCTL();
                    break;
         case 'E' : /* set parity even */
                    iobf[0] |= (m_PG + m_PE);
                    write_IOCTL();
                    break;
         case 'N' : /* set parity off */
                    iobf[0] &= ~(m_PG + m_PE);
                    write_IOCTL();
                    break;
         case 'O' : /* set parity odd */
                    iobf[0] |=  m_PG;
                    iobf[0] &= ~m_PE;
                    write_IOCTL();
                    break;
         case 'L' : /* toggle word length */
                    iobf[0] ^= 1;
                    write_IOCTL();
                    break;
         case 'S' : /* toggle stop bits */
                    iobf[0] ^= m_XS;
                    write_IOCTL();
                    break;
         case 'F' : /* toggle flow control */
                    if      (iobf[3] == 1) iobf[3] = 2;
                    else if (iobf[3] == 2) iobf[3] = 4;
                    else if (iobf[3] == 4) iobf[3] = 0;
                    else                   iobf[3] = 1;
                    write_IOCTL();
                    break;
         case 'R' : /* toggle error reports */
                    iobf[2] ^= 1;
                    write_IOCTL();
                    break;
         case 'W' : /* write ASY.COM using port number <unit> */
                    make_asy(unit);
                    break;
      }
   }
   fclose(dvp);

   popwindow(savescr,1,1,80,25);
   gotoxy(savex,savey);
}

void paint_screen(void)
{
   _put_text( 3, 2, NORMAL,"ÚÄASYDVR Utility Version 1.00ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿");
   _put_text( 3, 3, NORMAL,"³                                                           ³");
   _put_text( 3, 4, NORMAL,"³           Device :                                        ³");
   _put_text( 3, 5, NORMAL,"³     Flow control :                                        ³");
   _put_text( 3, 6, NORMAL,"³ Reception errors :                             .          ³");
   _put_text( 3, 7, NORMAL,"³                            ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´");
   _put_text( 3, 8, NORMAL,"³               CD :         ³  1 = select port 1           ³");
   _put_text( 3, 9, NORMAL,"³              DSR :         ³  2 = select port 2           ³");
   _put_text( 3,10, NORMAL,"³              CTS :         ³  I = INITIALISE ints,etc     ³");
   _put_text( 3,11, NORMAL,"³               RI :         ³  B = toggle BAUD rate        ³");
   _put_text( 3,12, NORMAL,"³    Xmtr SR empty :         ³  E = set parity EVEN         ³");
   _put_text( 3,13, NORMAL,"³    Xmtr HR empty :         ³  O = set parity ODD          ³");
   _put_text( 3,14, NORMAL,"³     Rcvr DR full :         ³  N = set parity NONE         ³");
   _put_text( 3,15, NORMAL,"³   Break received :         ³  L = toggle word LENGTH      ³");
   _put_text( 3,16, NORMAL,"³            HWINT :         ³  S = toggle STOP bits        ³");
   _put_text( 3,17, NORMAL,"³              DTR :         ³  F = toggle FLOW control     ³");
   _put_text( 3,18, NORMAL,"³              RTS :         ³  R = toggle error REPORTS    ³");
   _put_text( 3,19, NORMAL,"³    Framing error :         ³  W = WRITE ASY.COM with port ³");
   _put_text( 3,20, NORMAL,"³     Parity error :         ³  Q = QUIT                    ³");
   _put_text( 3,21, NORMAL,"³    Overrun error :         ³  COMMAND ==>                 ³");
   _put_text( 3,22, NORMAL,"³                            ³                              ³");
   _put_text( 3,23, NORMAL,"ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄ(c) Computer Lab Jhb 1991ÄÙ");
}

void update_screen(void)
{
   char temp1[81];
   char temp2[81];
   char temp3[81];

   sprintf(temp1,"ASY%d   %4d,%d,%c,%c",unit,
                iobf[4],                                            /* baud rate   */
                5 + (iobf[0] &m_WL),                                /* word length */
                ( iobf[0] &m_PG ? (iobf[0] & m_PE ? 'E':'O') : 'N'),/* parity      */
                ( iobf[0] &m_XS ? '2':'1')                          /* stop bits   */
          );

   if      (iobf[3] & 4) strcpy(temp2,"XON/XOFF");
   else if (iobf[3] & 2) strcpy(temp2,"RTS/CTS ");
   else if (iobf[3] & 1) strcpy(temp2,"DTR/DSR ");
   else                  strcpy(temp2,"disabled");

   if (iobf[2] == 1) strcpy(temp3,"Encoded as graphics in buffer");
   else              strcpy(temp3,"Set failure flag             ");

   _put_text(24, 4, BRIGHT,temp1);
   _put_text(24, 5, BRIGHT,temp2);
   _put_text(24, 6, BRIGHT,temp3);
   _put_text(24, 8, BRIGHT,onoff(iobf[1] & i_CD ));
   _put_text(24, 9, BRIGHT,onoff(iobf[1] & i_DSR));
   _put_text(24,10, BRIGHT,onoff(iobf[1] & i_CTS));
   _put_text(24,11, BRIGHT,onoff(iobf[1] & i_RI ));
   _put_text(24,12, BRIGHT,yesno(l_SRE & iobf[1]));
   _put_text(24,13, BRIGHT,yesno(l_HRE & iobf[1]));
   _put_text(24,14, BRIGHT,yesno(l_RRF & iobf[1]));
   _put_text(24,15, BRIGHT,yesno(l_BRK & iobf[1]));
   _put_text(24,16, BRIGHT,onoff(iobf[0] & HWINT));
   _put_text(24,17, BRIGHT,onoff(iobf[0] & o_DTR));
   _put_text(24,18, BRIGHT,onoff(iobf[0] & o_RTS));
   _put_text(24,19, BRIGHT,yesno(l_ER1 & iobf[1]));
   _put_text(24,20, BRIGHT,yesno(l_ER2 & iobf[1]));
   _put_text(24,21, BRIGHT,yesno(l_ER3 & iobf[1]));

   _put_char(47,21,1,ch);
   _put_attr(47,21,2,BRIGHT);
   gotoxy(47,21);
}

void write_IOCTL(void)
{
   /*inregs.x.dx = (int)iobf;*/

   union REGS inregs,outregs;
   struct SREGS segregs;
   int *ptr;

   ptr = iobf;
   inregs.x.ax = 0x4403;
   inregs.x.bx = fileno(dvp);
   inregs.x.cx = 10;
   inregs.x.dx = FP_OFF(ptr);
   segregs.ds  = FP_SEG(ptr);
   intdosx(&inregs,&outregs,&segregs);

   /*
   int fno;
   unsigned int seg1,ofs1;
   seg1 = FP_SEG(iobf);
   ofs1 = FP_OFF(iobf);

   fno = fileno(dvp);
   _asm
   {
      push ds
      mov ax,0x4403
      mov bx,fno
      mov cx,10
      mov ds,seg1
      mov dx,ofs1
      int 21
      pop ds
   }
   */
}

void read_IOCTL(void)
{
   union REGS inregs,outregs;
   struct SREGS segregs;
   int *ptr;

   ptr = iobf;
   inregs.x.ax = 0x4402;
   inregs.x.bx = fileno(dvp);
   inregs.x.cx = 10;
   inregs.x.dx = FP_OFF(ptr);
   segregs.ds  = FP_SEG(ptr);
   intdosx(&inregs,&outregs,&segregs);
   /*
   int fno;
   unsigned int seg1,ofs1;
   seg1 = FP_SEG(iobf);
   ofs1 = FP_OFF(iobf);

   fno = fileno(dvp);
   _asm
   {
      push ds
      mov ax,0x4402
      mov bx,fno
      mov cx,10
      mov ds,seg1
      mov dx,ofs1
      int 21
      pop ds
   }
   */
}

char *onoff(int x)
{
   return( x ? "on " : "off");
}

char *yesno(int x)
{
   return( x ? "yes" : "no ");
}

void make_asy(int unit)
{
   FILE *file;
   int  i;
   unsigned char ch[140] =
     {0xba,0x87,0x01,0xb8,0x02,0x3d,0xcd,0x21,0xa3,0x8c,0x01,0x72,0x0e,0xe8,0x0f,0x00,
      0xe8,0x61,0x00,0xe8,0x1e,0x00,0xe8,0x46,0x00,0xeb,0xf2,0xb4,0x4c,0xcd,0x21,0xb9,
      0x00,0x01,0x8b,0x1e,0x8c,0x01,0xba,0x92,0x01,0xb8,0x00,0x3f,0xcd,0x21,0x72,0xeb,
      0xa3,0x90,0x01,0xc3,0xc7,0x06,0x8e,0x01,0x00,0x00,0xb4,0x0b,0xcd,0x21,0xfe,0xc0,
      0x75,0x1c,0xb4,0x07,0xcd,0x21,0x3c,0x03,0x74,0xd1,0xa2,0x92,0x02,0xff,0x06,0x8e,
      0x01,0x3c,0x0d,0x75,0x09,0xc6,0x06,0x93,0x02,0x0a,0xff,0x06,0x8e,0x01,0xc3,0x8b,
      0x0e,0x8e,0x01,0xe3,0x0e,0x8b,0x1e,0x8c,0x01,0xba,0x92,0x02,0xb8,0x00,0x40,0xcd,
      0x21,0x72,0xa8,0xc3,0x8b,0x0e,0x90,0x01,0xe3,0x0c,0xbb,0x01,0x00,0xba,0x92,0x01,
      0xb4,0x40,0xcd,0x21,0x72,0x95,0xc3,0x41,0x53,0x59,0x31,0x00};

   file = fopen("asy.com","wb");
   if (file)
   {
      ch[138] = (unsigned char)'0' + (unsigned char)unit;
      for (i=0;i<140;i++) fputc(ch[i],file);
      for (i=0;i<  6;i++) fputc(0,file);
      for (i=0;i<256;i++) fputc(0,file);
      for (i=0;i< 80;i++) fputc(0,file);
   }
   fclose(file);
}

