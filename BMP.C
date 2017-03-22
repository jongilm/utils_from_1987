
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

typedef unsigned char BYTE;
typedef unsigned char BOOLEAN;
typedef unsigned int  UINT;
typedef unsigned int  WORD;
typedef unsigned long DWORD;
typedef long          LONG;

#define TRUE 1
#define FALSE 0

typedef void far *FP;
#ifndef MK_FP
#define MK_FP(seg,ofs)  ((FP)(((DWORD)(seg) << 16) | (ofs)))
#endif

#define Sequencer_Register        0x3c4
#define Sequencer_DataRegister    0x3c5
#define SEQ_Reset                 0x00
#define SEQ_Clocking_Mode         0x01
#define SEQ_Map_Mask              0x02
#define SEQ_Char_Map_Select       0x03
#define SEQ_Memory_Mode           0x04
#define Control_Register          0x3ce
#define Control_DataRegister      0x3cf
#define CTL_Set_Reset             0x00
#define CTL_Enable_Set_Reset      0x01
#define CTL_Color_Compare         0x02
#define CTL_Data_Rotate           0x03
#define CTL_Read_Map_Select       0x04
#define CTL_Mode                  0x05
#define CTL_MISC                  0x06
#define CTL_Color_Dont_Care       0x07
#define CTL_Bit_Mask              0x08
#define Attr_Control_Register     0x3c0
#define Attr_Control_DataRegister 0x3c1

#pragma pack(1)
typedef struct tagBITMAPFILEHEADER {    /* bmfh */
    UINT    bfType;
    DWORD   bfSize;
    UINT    bfReserved1;
    UINT    bfReserved2;
    DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {    /* bmih */
    DWORD   biSize;
    LONG    biWidth;
    LONG    biHeight;
    WORD    biPlanes;
    WORD    biBitCount;
    DWORD   biCompression;
    DWORD   biSizeImage;
    LONG    biXPelsPerMeter;
    LONG    biYPelsPerMeter;
    DWORD   biClrUsed;
    DWORD   biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {     /* rgbq */
    BYTE    rgbBlue;
    BYTE    rgbGreen;
    BYTE    rgbRed;
    BYTE    rgbReserved;
} RGBQUAD;

#pragma pack()

char                  Filename[_MAX_PATH];
BITMAPFILEHEADER      bmfh;
BITMAPINFOHEADER      bmih;
RGBQUAD               rgbq[256];
BYTE                  InBuffer    [1024];

int                   a256Modes_640_480  [7] = {  0x62,0x79,0x5c,0x5f,0x5d,0x2e,0x67 };
int                   a256Modes_800_600  [7] = {  0x63,0x7b,0x5e,0x5c,0x5e,0x30,0x69 };
int                   a256Modes_1024_768 [7] = {  0x63,0x7b,0x5e,0x5c,0x62,0x38,0x69 };
char far             *Vio          = (char far *)0xA0000000L;
char far             *Vio_1        = (char far *)0;
int                   ScreenWidth;
BOOLEAN               DisplayInfo = FALSE;
int                   VgaType;           /* 1 = ATI        */
                                         /* 2 = C&T        */
                                         /* 3 = GENOA      */
                                         /* 4 = PARADISE   */
                                         /* 5 = TRIDENT    */
                                         /* 6 = TSENG      */
                                         /* 7 = VIDEO7     */

static void __inline DrawPixel ( BYTE Color, BYTE WhichPixel, WORD OffsetAddress )
//旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
//�
//읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
{
   _asm mov dx,Control_Register
   _asm mov al,CTL_Bit_Mask
   _asm mov ah,WhichPixel
   _asm out dx,ax
   _asm mov al,CTL_Set_Reset
   _asm mov ah,Color
   _asm out dx,ax
   _asm mov ax, 0a000h
   _asm mov es, ax
   _asm mov di,OffsetAddress
   _asm mov ah,es:[di]
   _asm mov es:[di],al
}

static int GetVgaType ( void )
//旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
//�
//읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
{
   BYTE      old;
   BYTE      val;
   BYTE      id;
   char far *para;
   char      str[4];
   int       base;

   // ATI
   para = MK_FP(0xc000,0x0031);
   if (_fstrncmp (para,(char far *)"761295520",9) == 0)
       return 1;

   // C&T
   if ((BYTE)inp(0x0104) == 0xa5)
      return 2;

   // GENOA
   para = MK_FP(0xc000,0x0037);
   memcpy (str,para,4);
   if ((str[0] == 0x77) && (str[2] == 0x66) &&(str[3] == 0x99))
       return 3;

   // PARADISE
   para = MK_FP(0xc000,0x007d);
   if (_fstrncmp (para,(char far *)"VGA=",4) == 0)
      return 4;

   // TRIDENT
   outp(0x03c4,0x0e);
   old = (BYTE)inp(0x03c5);
   outp(0x03c5,0);
   val = (BYTE)inp(0x3c5);
   val &= 0x000f;
   outp(0x3c5,old);
   if (val == 0x02)
      return 5;

   // VIDEO 7
   if ( (BYTE)inp(0x3cc) & 0x01 == 1)
      base = 0x3d0;
   else
      base = 0x3b0;
   outp(0x3d4,0x0c);
   _asm mov dx,03d5h
   _asm in ax,dx
   _asm mov old,al
   //old = inp(0x3d5);
   outp(0x3d5,0x55);
   //val = inp(0x3d5);
   _asm mov dx,03d5h
   _asm in ax,dx
   _asm mov val,al
   outp(0x3d4,0x1f);
   //id = inp(0x3d5);
   _asm mov dx,03d5h
   _asm in ax,dx
   _asm mov id,al
   outp(0x3d4,0x0c);
   outp(0x3d5,old);
   if (id==(0x55^0xea))
      return 7;

   // ASSUME TSENG
   return 6;
}

static void SetPaletteRegisters256 ( void )
//旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
//�
//읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
{
   int i;

   for(i=0;i<256;i++)
   {
      outp(0x3c8,i);
      outp(0x3c9,(BYTE)rgbq[i].rgbRed  /4);
      outp(0x3c9,(BYTE)rgbq[i].rgbGreen/4);
      outp(0x3c9,(BYTE)rgbq[i].rgbBlue /4);
   }
}
/*
static void SetPaletteRegisters16 ( void )
//旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
//�
//읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
{
   int i;

   for(i=0;i<16;i++)
   {
      outp(0x3c8,i);
      outp(0x3c9,(BYTE)rgbq[i].rgbRed  /4);
      outp(0x3c9,(BYTE)rgbq[i].rgbGreen/4);
      outp(0x3c9,(BYTE)rgbq[i].rgbBlue /4);
   }
}
*/
/*
static void gotoxy(int r,int c)
//旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
//�
//읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
{
   union REGS regs;

   regs.h.ah = 0x02;
   regs.h.bh = 0;
   regs.h.dh = (BYTE)r;
   regs.h.dl = (BYTE)c;
   int86(0x10,&regs,&regs);
}
*/


static void ResetVideoModeAndExit ( char *Msg1, char *Msg2 )
//旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
//�
//읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
{
   union REGS regs;

   regs.h.ah = 0x00;
   regs.h.al = 3;
   int86(0x10,&regs,&regs);
   if (Msg1 && Msg1[0])
   {
      printf("ERROR: %s",Msg1);
      if (Msg2 && Msg2[0])
      {
         printf(": %s",Msg2);
      }
      printf("\n");
      printf("Press any key to exit\n");
      getch();
   }
   exit(0);
}

static void ResetVideoModeAndExiti ( char *Msg1, int Val )
//旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
//�
//읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
{
   char s[32];
   ResetVideoModeAndExit ( Msg1, itoa(Val,s,10) );
}

static BOOLEAN SetVideoMode ( int NewMode )
//旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
//�
//읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
{
   union REGS regs;
   BYTE Original_Mode;

   // Get video mode
   regs.h.ah = 0x0f;
   int86(0x10,&regs,&regs);
   Original_Mode = regs.h.al;

   if (Original_Mode == (BYTE)NewMode)
        return TRUE;

   // Set video mode
   regs.h.ah = 0x00;
   regs.h.al = (BYTE)NewMode;
   int86(0x10,&regs,&regs);

   // Get video mode
   regs.h.ah = 0x0f;
   int86(0x10,&regs,&regs);
   if ( regs.h.al == (BYTE)NewMode )
      return TRUE;

   // Restore video mode
   regs.h.ah = 0x00;
   regs.h.al = Original_Mode;
   int86(0x10,&regs,&regs);

   {
      char s[64];
      sprintf(s,"%xh\n", NewMode);
      ResetVideoModeAndExit("Unsupported Video Mode",s);
      return FALSE; // academic
   }
}

static void DetermineAndSetVideoMode ( int BitCount, int MinimumResolution )
//旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
//�
//읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
{
   switch ( BitCount )
   {
      case 1 :
      case 4 :
         SetVideoMode ( 0x12 );
         ScreenWidth = 80;
         return;
      case 8  :
      case 16 :
      case 24 :
         if ((bmih.biWidth <= 320) && ( bmih.biHeight <= 200) && MinimumResolution <= 320)
         {
            SetVideoMode(0x13);
            ScreenWidth = 320;
            return;
         }
         if ((bmih.biWidth <= 640) && ( bmih.biHeight <= 480) && MinimumResolution <= 640)
         {
            SetVideoMode(a256Modes_640_480[VgaType-1]);
            ScreenWidth = 640;
            return;
         }
         if ((bmih.biWidth <= 800) && ( bmih.biHeight <= 600) && MinimumResolution <= 800)
         {
            SetVideoMode(a256Modes_800_600[VgaType-1]);
            ScreenWidth = 800;
            return;
         }
         if ((bmih.biWidth <= 1024) && ( bmih.biHeight <= 768) && MinimumResolution <= 1024)
         {
            SetVideoMode(a256Modes_1024_768[VgaType-1]);
            ScreenWidth = 1024;
            return;
         }
      default: ResetVideoModeAndExiti("Unsupported BitCount", BitCount );
   }
   ResetVideoModeAndExit("Unsupported Video Mode", NULL );
}

static void SetupVGA ( void )
//旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
//�
//읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
{
   _asm
   {
       MOV    DX,03c4h
       MOV    ah,0fh
       MOV    al,002h
       out    dx,ax

       mov dx,Control_Register
       mov al,CTL_Enable_Set_Reset
       out dx,al
       inc dx
       mov al,0fh   /* all panes */
       out dx,al

       mov dx,Control_Register
       mov al,SEQ_Map_Mask
       out dx,al
       inc dx
       mov al,0fh   /* modify all panes */
       out dx,al
   }
}

static void Set_Vga_Page ( BYTE page )
//旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
//�
//읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
{
  if (VgaType == 4)
  _asm
    {
      mov dx,3ceh
      mov al,09h
      mov ah,page
      shl ah,1
      shl ah,1
      shl ah,1
      shl ah,1
      out dx,ax
    }
  if (VgaType == 5)
  _asm
    {
      /*mov dx,3c4h
      mov ax,000bh
      out dx,ax
      */
      mov dx,3c4h
      mov al,0eh
      mov bl,page
      xor bl,02
      mov ah,bl
      out dx,ax
    }
  if (VgaType == 6)
  _asm
    {
      mov dx,3cdh
      in al,dx
      and al,0f0h
      mov bl,page
      and bl,0fh
      or  al,bl
      out dx,al
    }
}

static BYTE Get_Vga_Page(void)
//旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
//�
//읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
{
  BYTE x;
  if (VgaType == 4)
  _asm
    {
      mov dx,3ceh
      mov al,09h
      out dx,al
      mov dx,3cfh
      in  al,dx
      mov x,al
    }
  if (VgaType == 5)
  _asm
    {
      mov dx,3c4h
      mov al,0eh
      out dx,al
      mov dx,3c5h
      in  al,dx
      mov x,al
    }
  if (VgaType == 6)
  _asm
    {
      mov dx,3cdh
      in  al,dx
      mov x,al
    }
  return x;
}

static void write_pixel_line (char _far *Data, int len, int line, int start_col, DWORD start_row )
//旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
//�
//읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
{
   BYTE _far *Vid_ptr;
   int        k;
   WORD       off2;
   DWORD      offset1;
   DWORD      start_pos;
   BYTE       page      = 0;
   BYTE       oldpage;

   oldpage   = Get_Vga_Page();
   Vid_ptr   = Vio;
   offset1   = (start_row*(DWORD)ScreenWidth)+(DWORD)start_col;
   start_pos = offset1;
   page      = (BYTE)(offset1/65536L);

   offset1 %= 65536L;

   if ( page > 0)
      Set_Vga_Page(page);

   if ((FP_OFF(Data) < 0xffff - len) && (offset1+len <  65535L))
   {
      off2 = (WORD)offset1;
      _asm
      {
         push es
         push ds
         push di
         push si
         push cx
         les DI,Vid_ptr
         add di,off2
         lds SI,Data
         cld
         mov cx,len
         rep movsb
         pop  cx
         pop  si
         pop  di
         pop  ds
         pop  es
      }
      offset1 += len;
      Data    += len;
   }
   else
   {
      for ( k=0; k<len; k++ )
      {
         if ( offset1 > 65535L)
         {
            offset1 %= 65536L;
            page++;
            Set_Vga_Page(page);
         }
         *(Vid_ptr+offset1++) = *(Data++);
      }
   }
   Set_Vga_Page ( oldpage );
}

static void DisplayHeaders ( char *Filename )
//旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
//�
//읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
{
   printf( "Filename             = %s\n",   Filename              );
   printf( "\n");
   printf( "bmfh.bfType          = %u\n",   bmfh.bfType           );
   printf( "bmfh.bfSize          = %lu\n",  bmfh.bfSize           );
   printf( "bmfh.bfReserved1     = %u\n",   bmfh.bfReserved1      );
   printf( "bmfh.bfReserved2     = %u\n",   bmfh.bfReserved2      );
   printf( "bmfh.bfOffBits       = %lu\n",  bmfh.bfOffBits        );
   printf( "\n");
   printf( "bmih.biSize          = %lu\n",  bmih.biSize           );
   printf( "bmih.biWidth         = %ld\n",  bmih.biWidth          );
   printf( "bmih.biHeight        = %ld\n",  bmih.biHeight         );
   printf( "bmih.biPlanes        = %u\n",   bmih.biPlanes         );
   printf( "bmih.biBitCount      = %u\n",   bmih.biBitCount       );
   printf( "bmih.biCompression   = %lu\n",  bmih.biCompression    );
   printf( "bmih.biSizeImage     = %lu\n",  bmih.biSizeImage      );
   printf( "bmih.biXPelsPerMeter = %ld\n",  bmih.biXPelsPerMeter  );
   printf( "bmih.biYPelsPerMeter = %ld\n",  bmih.biYPelsPerMeter  );
   printf( "bmih.biClrUsed       = %lu\n",  bmih.biClrUsed        );
   printf( "bmih.biClrImportant  = %lu\n",  bmih.biClrImportant   );
   getch();
}

static int DrawBitMap ( char *FileName, int xPos, int yPos, int MinimumResolution )
//旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
//�
//읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
{
   int   i;
   int   j;
   int   numpals;
   int   k;
   BYTE  l;
   long  size;
   BYTE  PixelColor;
   int   Width;
   int   ReadWidth;
   int   offset=0;
   DWORD Row;
   FILE *f;

   if ((f = fopen(FileName,"rb")) == NULL)
      ResetVideoModeAndExit("Error opening file",FileName);

   if (fread(&bmfh,sizeof(BITMAPFILEHEADER),1,f) == 0)
      ResetVideoModeAndExit("Error reading BITMAPFILEHEADER",FileName);

   if (fread(&bmih,sizeof(BITMAPINFOHEADER),1,f) == 0)
      ResetVideoModeAndExit("Error reading BITMAPINFOHEADER",FileName);
   if (DisplayInfo)
      DisplayHeaders ( FileName );

   switch ( bmih.biBitCount )
   {
      case 1 : numpals = 0;   break;
      case 4 : numpals = 16;  break;
      case 8 : numpals = 256; break;
      //case 16 : numpals = 0; break;
      //case 24: numpals = 0; break;
      case 16: numpals = 256; break; // Jonnie, Fri 16-Aug-1996, 17:55:16
      case 24: numpals = 256; break; // Jonnie, Fri 16-Aug-1996, 17:55:16
      default: ResetVideoModeAndExiti("Unrecognised BitCount",bmih.biBitCount);
   }
   for ( i=0; i<numpals; i++ )
      if (fread(&rgbq[i],sizeof(RGBQUAD),1,f) == 0)
         ResetVideoModeAndExiti("Error reading colour record",i);

   size = ftell(f);
   if (size != bmfh.bfOffBits)
      fseek(f,bmfh.bfOffBits,SEEK_SET);

   VgaType = GetVgaType ();
   DetermineAndSetVideoMode ( (int)bmih.biBitCount, MinimumResolution );

   if (bmih.biBitCount > 4)
       SetPaletteRegisters256 ();

   Vio = (char far *)0xA0000000L;

   switch(bmih.biBitCount)
   {
      case 1 :  break;
      case 4 :
               offset = ((int)bmih.biHeight+yPos) * 80  + xPos ;

               if (bmih.biWidth%2)
                 Width =((int) bmih.biWidth/2)+1;
               else
                 Width =(int) bmih.biWidth/2;

               if (bmih.biWidth % 8)
                 ReadWidth = (int)bmih.biWidth + ( 8 - ((int)bmih.biWidth % 8 ));
               else
                 ReadWidth = (int)bmih.biWidth;

               SetupVGA ();
               for(j=0;j<(int)bmih.biHeight;j++)  // For each row in the file
               {
                  char s[32];
                  if (ReadWidth/2 > sizeof(InBuffer))
                      ResetVideoModeAndExiti("ReadWidth too large",ReadWidth);
                  l=0x80;
                  fread(InBuffer,1,ReadWidth/2,f);
                  for(k=0;k<Width;k++)
                  {
                     WORD i;
                     i=offset + (k*2)/8;
                     PixelColor = InBuffer[k]>>4;
                     switch(InBuffer[k]&0xf0)
                     {
                       case 0x00  : DrawPixel (  0, l, i ); break;
                       case 0x10  : DrawPixel (  4, l, i ); break;
                       case 0x20  : DrawPixel (  2, l, i ); break;
                       case 0x30  : DrawPixel (  6, l, i ); break;
                       case 0x40  : DrawPixel (  1, l, i ); break;
                       case 0x50  : DrawPixel (  5, l, i ); break;
                       case 0x60  : DrawPixel (  3, l, i ); break;
                       case 0x70  : DrawPixel (  7, l, i ); break;
                       case 0x80  : DrawPixel (  8, l, i ); break;
                       case 0x90  : DrawPixel ( 12, l, i ); break;
                       case 0xA0  : DrawPixel ( 10, l, i ); break;
                       case 0xB0  : DrawPixel ( 14, l, i ); break;
                       case 0xC0  : DrawPixel (  9, l, i ); break;
                       case 0xD0  : DrawPixel ( 13, l, i ); break;
                       case 0xE0  : DrawPixel ( 11, l, i ); break;
                       case 0xF0  : DrawPixel ( 15, l, i ); break;
                     }
                     l/=2;
                     switch(InBuffer[k]&0x0f)
                     {
                       case 0x00  : DrawPixel (  0 , l, i ); break;
                       case 0x01  : DrawPixel (  4 , l, i ); break;
                       case 0x02  : DrawPixel (  2 , l, i ); break;
                       case 0x03  : DrawPixel (  6 , l, i ); break;
                       case 0x04  : DrawPixel (  1 , l, i ); break;
                       case 0x05  : DrawPixel (  5 , l, i ); break;
                       case 0x06  : DrawPixel (  3 , l, i ); break;
                       case 0x07  : DrawPixel (  7 , l, i ); break;
                       case 0x08  : DrawPixel (  8 , l, i ); break;
                       case 0x09  : DrawPixel ( 12 , l, i ); break;
                       case 0x0A  : DrawPixel ( 10 , l, i ); break;
                       case 0x0B  : DrawPixel ( 14 , l, i ); break;
                       case 0x0C  : DrawPixel (  9 , l, i ); break;
                       case 0x0D  : DrawPixel ( 13 , l, i ); break;
                       case 0x0E  : DrawPixel ( 11 , l, i ); break;
                       case 0x0F  : DrawPixel ( 15 , l, i ); break;
                     }
                     l /= 2;
                     if (l<1) l = 0x80;
                  }
                  offset -= 80;
               }
               break;
      case 8 :
      case 16:
      case 24:
        /*
                if (ScreenWidth == 320)
                   for(j=200;j>-1;j--)
                      fread (&Vio[j*320],320,1,f);
                else
        */
                {
                   ReadWidth = (int)bmih.biWidth;
                   size = 0;
                   Row  =  bmih.biHeight;
                   for(j=0;j<(int)bmih.biHeight;j++,Row--)  // For each row in the file
                   {
                      char s[32];
                      if (ReadWidth > sizeof(InBuffer))
                         ResetVideoModeAndExiti("ReadWidth too large",ReadWidth);

                      fread(InBuffer,1,ReadWidth,f);
                    //  for(k=0;k<ReadWidth;k++)
                      write_pixel_line ((char _far *)InBuffer,ReadWidth,1,0,(int)Row);
                   }
                }
                break;
   }
   fclose (f);

   return 0;
}

int main (int argc , char **argv)
//旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
//�
//읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
{
   int MinimumResolution;
   int i;

   if ( argc < 2)
   {
      printf("Geoff's Bitmap Viewer\n");
      printf("Usage : BMP [/I] <FileName>\n");
      printf("  While 8/24 bit image is displayed, press:\n");
      printf("     1 to set minimum resolution to 320x200\n");
      printf("     2 to set minimum resolution to 640x480\n");
      printf("     3 to set minimum resolution to 800x600\n");
      printf("     4 to set minimum resolution to 1024x768\n");
      exit(0);
   }
   for (i=1;i<argc;i++)
   {
      if (argv[i][0]=='/'||argv[i][0]=='-')
      {
         if (toupper(argv[i][1])=='I')
            DisplayInfo = TRUE;
      }
      else if (Filename[0] == 0)
      {
         strcpy(Filename, argv[i]);
      }
   }
   MinimumResolution = 320;
   while (MinimumResolution)
   {
      DrawBitMap ( Filename, 0 , 0, MinimumResolution );
      switch ( getch() )
      {
         case 0x1B: MinimumResolution =    0; break;
         case '1' : MinimumResolution =  320; break;
         case '2' : MinimumResolution =  640; break;
         case '3' : MinimumResolution =  800; break;
         case '4' : MinimumResolution = 1024; break;
      }
   }
   ResetVideoModeAndExit(NULL,NULL);

   return 0;
}

#if 0
///////////////////////////////////////////////////////////////////////////
Topic:  Graphics File Formats 

                           Graphics File Formats

This topic describes the graphics-file formats used by the Microsoft Windows
operating system. Graphics files include bitmap files, icon-resource files,
and cursor-resource files.

                            Bitmap-File Formats

Windows bitmap files are stored in a device-independent bitmap (DIB) format
that allows Windows to display the bitmap on any type of display device. The
term "device independent" means that the bitmap specifies pixel color in a
form independent of the method used by a display to represent color. The
default filename extension of a Windows DIB file is .BMP.

Bitmap-File Structures

Each bitmap file contains a bitmap-file header, a bitmap-information header,
a color table, and an array of bytes that defines the bitmap bits. The file
has the following form:

BITMAPFILEHEADER bmfh;
BITMAPINFOHEADER bmih;
RGBQUAD          aColors[];
BYTE             aBitmapBits[];

The bitmap-file header contains information about the type, size, and layout
of a device-independent bitmap file. The header is defined as a
BITMAPFILEHEADER structure.

The bitmap-information header, defined as a BITMAPINFOHEADER structure,
specifies the dimensions, compression type, and color format for the
bitmap.

The color table, defined as an array of RGBQUAD structures, contains as many
elements as there are colors in the bitmap. The color table is not present
for bitmaps with 24 color bits because each pixel is represented by 24-bit
red-green-blue (RGB) values in the actual bitmap data area. The colors in
the table should appear in order of importance. This helps a display driver
render a bitmap on a device that cannot display as many colors as there are
in the bitmap. If the DIB is in Windows version 3.0 or later format, the
driver can use the biClrImportant member of the BITMAPINFOHEADER structure
to determine which colors are important.

The BITMAPINFO structure can be used to represent a combined
bitmap-information header and color table.

The bitmap bits, immediately following the color table, consist of an array
of BYTE values representing consecutive rows, or "scan lines," of the
bitmap. Each scan line consists of consecutive bytes representing the pixels
in the scan line, in left-to-right order. The number of bytes representing a
scan line depends on the color format and the width, in pixels, of the
bitmap. If necessary, a scan line must be zero-padded to end on a 32-bit
boundary. However, segment boundaries can appear anywhere in the bitmap. The
scan lines in the bitmap are stored from bottom up. This means that the
first byte in the array represents the pixels in the lower-left corner of
the bitmap and the last byte represents the pixels in the upper-right
corner.

The biBitCount member of the BITMAPINFOHEADER structure determines the
number of bits that define each pixel and the maximum number of colors in
the bitmap. These members can have any of the following values:

Value  Meaning
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
1      Bitmap is monochrome and the color table contains two entries. Each
       bit in the bitmap array represents a pixel. If the bit is clear, the
       pixel is displayed with the color of the first entry in the color
       table. If the bit is set, the pixel has the color of the second entry
       in the table.

4      Bitmap has a maximum of 16 colors. Each pixel in the bitmap is
       represented by a 4-bit index into the color table. For example, if
       the first byte in the bitmap is 0x1F, the byte represents two pixels.
       The first pixel contains the color in the second table entry, and the
       second pixel contains the color in the sixteenth table entry.

8      Bitmap has a maximum of 256 colors. Each pixel in the bitmap is
       represented by a 1-byte index into the color table. For example, if
       the first byte in the bitmap is 0x1F, the first pixel has the color
       of the thirty-second table entry.

24     Bitmap has a maximum of 2^24 colors. The bmiColors (or bmciColors)
       member is NULL, and each 3-byte sequence in the bitmap array
       represents the relative intensities of red, green, and blue,
       respectively, for a pixel.

The biClrUsed member of the BITMAPINFOHEADER structure specifies the number
of color indexes in the color table actually used by the bitmap. If the
biClrUsed member is set to zero, the bitmap uses the maximum number of
colors corresponding to the value of the biBitCount member.

An alternative form of bitmap file uses the BITMAPCOREINFO,
BITMAPCOREHEADER, and RGBTRIPLE structures.

Bitmap Compression

Windows versions 3.0 and later support run-length encoded (RLE) formats for
compressing bitmaps that use 4 bits per pixel and 8 bits per pixel.
Compression reduces the disk and memory storage required for a bitmap.

Compression of 8-Bits-per-Pixel Bitmaps

When the biCompression member of the BITMAPINFOHEADER structure is set to
BI_RLE8, the DIB is compressed using a run-length encoded format for a
256-color bitmap. This format uses two modes: encoded mode and absolute
mode. Both modes can occur anywhere throughout a single bitmap.

Encoded Mode

A unit of information in encoded mode consists of two bytes. The first byte
specifies the number of consecutive pixels to be drawn using the color index
contained in the second byte.

The first byte of the pair can be set to zero to indicate an escape that
denotes the end of a line, the end of the bitmap, or a delta. The
interpretation of the escape depends on the value of the second byte of the
pair, which must be in the range 0x00 through 0x02. Following are the
meanings of the escape values that can be used in the second byte:

Second byte  Meaning
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
0            End of line.

1            End of bitmap.

2            Delta. The two bytes following the escape contain unsigned
             values indicating the horizontal and vertical offsets of the
             next pixel from the current position.

Absolute Mode

Absolute mode is signaled by the first byte in the pair being set to zero
and the second byte to a value between 0x03 and 0xFF. The second byte
represents the number of bytes that follow, each of which contains the color
index of a single pixel. Each run must be aligned on a word boundary.

Following is an example of an 8-bit RLE bitmap (the two-digit hexadecimal
values in the second column represent a color index for a single pixel):

Compressed data    Expanded data
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
03 04              04 04 04

05 06              06 06 06 06 06

00 03 45 56 67 00  45 56 67

02 78              78 78

00 02 05 01        Move 5 right and 1 down

02 78              78 78

00 00              End of line

09 1E              1E 1E 1E 1E 1E 1E 1E 1E 1E

00 01              End of RLE bitmap

Compression of 4-Bits-per-Pixel Bitmaps

When the biCompression member of the BITMAPINFOHEADER structure is set to
BI_RLE4, the DIB is compressed using a run-length encoded format for a
16-color bitmap. This format uses two modes: encoded mode and absolute
mode.

Encoded Mode

A unit of information in encoded mode consists of two bytes. The first byte
of the pair contains the number of pixels to be drawn using the color
indexes in the second byte.

The second byte contains two color indexes, one in its high-order nibble
(that is, its low-order 4 bits) and one in its low-order nibble. The first
pixel is drawn using the color specified by the high-order nibble, the
second is drawn using the color in the low-order nibble, the third is drawn
with the color in the high-order nibble, and so on, until all the pixels
specified by the first byte have been drawn.

The first byte of the pair can be set to zero to indicate an escape that
denotes the end of a line, the end of the bitmap, or a delta. The
interpretation of the escape depends on the value of the second byte of the
pair. In encoded mode, the second byte has a value in the range 0x00 through
0x02. The meaning of these values is the same as for a DIB with 8 bits per
pixel.

Absolute Mode

In absolute mode, the first byte contains zero, the second byte contains the
number of color indexes that follow, and subsequent bytes contain color
indexes in their high- and low-order nibbles, one color index for each
pixel. Each run must be aligned on a word boundary.

Following is an example of a 4-bit RLE bitmap (the one-digit hexadecimal
values in the second column represent a color index for a single pixel):

Compressed data    Expanded data
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
03 04              0 4 0

05 06              0 6 0 6 0

00 06 45 56 67 00  4 5 5 6 6 7

04 78              7 8 7 8

00 02 05 01        Move 5 right and 1 down

04 78              7 8 7 8

00 00              End of line

09 1E              1 E 1 E 1 E 1 E 1

00 01              End of RLE bitmap

Bitmap Example

The following example is a text dump of a 16-color bitmap (4 bits per
pixel):

Win3DIBFile
              BitmapFileHeader
                  Type       19778
                  Size       3118
                  Reserved1  0
                  Reserved2  0
                  OffsetBits 118
              BitmapInfoHeader
                  Size            40
                  Width           80
                  Height          75
                  Planes          1
                  BitCount        4
                  Compression     0
                  SizeImage       3000
                  XPelsPerMeter   0
                  YPelsPerMeter   0
                  ColorsUsed      16
                  ColorsImportant 16
              Win3ColorTable
                  Blue  Green  Red  Unused
[00000000]        84    252    84   0
[00000001]        252   252    84   0
[00000002]        84    84     252  0
[00000003]        252   84     252  0
[00000004]        84    252    252  0
[00000005]        252   252    252  0
[00000006]        0     0      0    0
[00000007]        168   0      0    0
[00000008]        0     168    0    0
[00000009]        168   168    0    0
[0000000A]        0     0      168  0
[0000000B]        168   0      168  0
[0000000C]        0     168    168  0
[0000000D]        168   168    168  0
[0000000E]        84    84     84   0
[0000000F]        252   84     84   0
              Image
    .
    .                                           Bitmap data
    .

                         Icon-Resource File Format

An icon-resource file contains image data for icons used by Windows
applications. The file consists of an icon directory identifying the number
and types of icon images in the file, plus one or more icon images. The
default filename extension for an icon-resource file is .ICO.

Icon Directory

Each icon-resource file starts with an icon directory. The icon directory,
defined as an ICONDIR structure, specifies the number of icons in the
resource and the dimensions and color format of each icon image. The
ICONDIR structure has the following form:

typedef struct ICONDIR {
    WORD          idReserved;
    WORD          idType;
    WORD          idCount;
    ICONDIRENTRY  idEntries[1];
} ICONHEADER;

Following are the members in the ICONDIR structure:

idReserved  Reserved; must be zero.

idType      Specifies the resource type. This member is set to 1.

idCount     Specifies the number of entries in the directory.

idEntries   Specifies an array of ICONDIRENTRY structures containing
            information about individual icons. The idCount member specifies
            the number of structures in the array.

The ICONDIRENTRY structure specifies the dimensions and color format for an
icon. The structure has the following form:

struct IconDirectoryEntry {
    BYTE  bWidth;
    BYTE  bHeight;
    BYTE  bColorCount;
    BYTE  bReserved;
    WORD  wPlanes;
    WORD  wBitCount;
    DWORD dwBytesInRes;
    DWORD dwImageOffset;
};

Following are the members in the ICONDIRENTRY structure:

bWidth         Specifies the width of the icon, in pixels. Acceptable values
               are 16, 32, and 64.

bHeight        Specifies the height of the icon, in pixels. Acceptable
               values are 16, 32, and 64.

bColorCount    Specifies the number of colors in the icon. Acceptable values
               are 2, 8, and 16.

bReserved      Reserved; must be zero.

wPlanes        Specifies the number of color planes in the icon bitmap.

wBitCount      Specifies the number of bits in the icon bitmap.

dwBytesInRes   Specifies the size of the resource, in bytes.

dwImageOffset  Specifies the offset, in bytes, from the beginning of the
               file to the icon image.

Icon Image

Each icon-resource file contains one icon image for each image identified in
the icon directory. An icon image consists of an icon-image header, a color
table, an XOR mask, and an AND mask. The icon image has the following form:

BITMAPINFOHEADER    icHeader;
RGBQUAD             icColors[];
BYTE                icXOR[];
BYTE                icAND[];

The icon-image header, defined as a BITMAPINFOHEADER structure, specifies
the dimensions and color format of the icon bitmap. Only the biSize through
biBitCount members and the biSizeImage member are used. All other members
(such as biCompression and biClrImportant) must be set to zero.

The color table, defined as an array of RGBQUAD structures, specifies the
colors used in the XOR mask. As with the color table in a bitmap file, the
biBitCount member in the icon-image header determines the number of elements
in the array. For more information about the color table, see Section 1.1,
"Bitmap-File Formats."

The XOR mask, immediately following the color table, is an array of BYTE
values representing consecutive rows of a bitmap. The bitmap defines the
basic shape and color of the icon image. As with the bitmap bits in a bitmap
file, the bitmap data in an icon-resource file is organized in scan lines,
with each byte representing one or more pixels, as defined by the color
format. For more information about these bitmap bits, see Section 1.1,
"Bitmap-File Formats."

The AND mask, immediately following the XOR mask, is an array of BYTE
values, representing a monochrome bitmap with the same width and height as
the XOR mask. The array is organized in scan lines, with each byte
representing 8 pixels.

When Windows draws an icon, it uses the AND and XOR masks to combine the
icon image with the pixels already on the display surface. Windows first
applies the AND mask by using a bitwise AND operation; this preserves or
removes existing pixel color. Windows then applies the XOR mask by using a
bitwise XOR operation. This sets the final color for each pixel.

The following illustration shows the XOR and AND masks that create a
monochrome icon (measuring 8 pixels by 8 pixels) in the form of an uppercase
K:

Windows Icon Selection

Windows detects the resolution of the current display and matches it against
the width and height specified for each version of the icon image. If
Windows determines that there is an exact match between an icon image and
the current device, it uses the matching image. Otherwise, it selects the
closest match and stretches the image to the proper size.

If an icon-resource file contains more than one image for a particular
resolution, Windows uses the icon image that most closely matches the color
capabilities of the current display. If no image matches the device
capabilities exactly, Windows selects the image that has the greatest number
of colors without exceeding the number of display colors. If all images
exceed the color capabilities of the current display, Windows uses the icon
image with the least number of colors.

                        Cursor-Resource File Format

A cursor-resource file contains image data for cursors used by Windows
applications. The file consists of a cursor directory identifying the number
and types of cursor images in the file, plus one or more cursor images. The
default filename extension for a cursor-resource file is .CUR.

Cursor Directory

Each cursor-resource file starts with a cursor directory. The cursor
directory, defined as a CURSORDIR structure, specifies the number of cursors
in the file and the dimensions and color format of each cursor image. The
CURSORDIR structure has the following form:

typedef struct _CURSORDIR {
    WORD           cdReserved;
    WORD           cdType;
    WORD           cdCount;
    CURSORDIRENTRY cdEntries[];
} CURSORDIR;

Following are the members in the CURSORDIR structure:

cdReserved  Reserved; must be zero.

cdType      Specifies the resource type. This member must be set to 2.

cdCount     Specifies the number of cursors in the file.

cdEntries   Specifies an array of CURSORDIRENTRY structures containing
            information about individual cursors. The cdCount member
            specifies the number of structures in the array.

A CURSORDIRENTRY structure specifies the dimensions and color format of a
cursor image. The structure has the following form:

typedef struct _CURSORDIRENTRY {
    BYTE  bWidth;
    BYTE  bHeight;
    BYTE  bColorCount;
    BYTE  bReserved;
    WORD  wXHotspot;
    WORD  wYHotspot;
    DWORD lBytesInRes;
    DWORD dwImageOffset;
} CURSORDIRENTRY;

Following are the members in the CURSORDIRENTRY structure:

bWidth         Specifies the width of the cursor, in pixels.

bHeight        Specifies the height of the cursor, in pixels.

bColorCount    Reserved; must be zero.

bReserved      Reserved; must be zero.

wXHotspot      Specifies the x-coordinate, in pixels, of the hot spot.

wYHotspot      Specifies the y-coordinate, in pixels, of the hot spot.

lBytesInRes    Specifies the size of the resource, in bytes.

dwImageOffset  Specifies the offset, in bytes, from the start of the file to
               the cursor image.

Cursor Image

Each cursor-resource file contains one cursor image for each image
identified in the cursor directory. A cursor image consists of a
cursor-image header, a color table, an XOR mask, and an AND mask. The cursor
image has the following form:

BITMAPINFOHEADER    crHeader;
RGBQUAD             crColors[];
BYTE                crXOR[];
BYTE                crAND[];

The cursor hot spot is a single pixel in the cursor bitmap that Windows uses
to track the cursor. The crXHotspot and crYHotspot members specify the x-
and y-coordinates of the cursor hot spot. These coordinates are 16-bit
integers.

The cursor-image header, defined as a BITMAPINFOHEADER structure, specifies
the dimensions and color format of the cursor bitmap. Only the biSize
through biBitCount members and the biSizeImage member are used. The
biHeight member specifies the combined height of the XOR and AND masks for
the cursor. This value is twice the height of the XOR mask. The biPlanes and
biBitCount members must be 1. All other members (such as biCompression and
biClrImportant) must be set to zero.

The color table, defined as an array of RGBQUAD structures, specifies the
colors used in the XOR mask. For a cursor image, the table contains exactly
two structures, since the biBitCount member in the cursor-image header is
always 1.

The XOR mask, immediately following the color table, is an array of BYTE
values representing consecutive rows of a bitmap. The bitmap defines the
basic shape and color of the cursor image. As with the bitmap bits in a
bitmap file, the bitmap data in a cursor-resource file is organized in scan
lines, with each byte representing one or more pixels, as defined by the
color format. For more information about these bitmap bits, see Section 1.1,
"Bitmap-File Formats."

The AND mask, immediately following the XOR mask, is an array of BYTE values
representing a monochrome bitmap with the same width and height as the XOR
mask. The array is organized in scan lines, with each byte representing 8
pixels.

When Windows draws a cursor, it uses the AND and XOR masks to combine the
cursor image with the pixels already on the display surface. Windows first
applies the AND mask by using a bitwise AND operation; this preserves or
removes existing pixel color. Window then applies the XOR mask by using a
bitwise XOR operation. This sets the final color for each pixel.

The following illustration shows the XOR and the AND masks that create a
cursor (measuring 8 pixels by 8 pixels) in the form of an arrow:

Following are the bit-mask values necessary to produce black, white,
inverted, and transparent results:

Pixel         AND       XOR mask
result        mask
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�

Black         0         0

White         0         1

Transparent   1         0

Inverted      1         1

Windows Cursor Selection

If a cursor-resource file contains more than one cursor image, Windows
determines the best match for a particular display by examining the width
and height of the cursor images.

////////////////////////////////////////////////////////////////////////////
Topic:  BITMAPFILEHEADER (3.0) 

typedef struct tagBITMAPFILEHEADER {    /* bmfh */
    UINT    bfType;
    DWORD   bfSize;
    UINT    bfReserved1;
    UINT    bfReserved2;
    DWORD   bfOffBits;
} BITMAPFILEHEADER;

The BITMAPFILEHEADER structure contains information about the type, size,
and layout of a device-independent bitmap (DIB) file.

Member       Description
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

bfType       Specifies the type of file. This member must be BM.

bfSize       Specifies the size of the file, in bytes.

bfReserved1  Reserved; must be set to zero.

bfReserved2  Reserved; must be set to zero.

bfOffBits    Specifies the byte offset from the BITMAPFILEHEADER structure
             to the actual bitmap data in the file.

Comments

A BITMAPINFO or BITMAPCOREINFO structure immediately follows the
BITMAPFILEHEADER structure in the DIB file.

See Also

BITMAPCOREINFO, BITMAPINFO
///////////////////////////////////////////////////////////////////////////
Topic:  BITMAPFILEHEADER (3.0) 

typedef struct tagBITMAPFILEHEADER {    /* bmfh */
    UINT    bfType;
    DWORD   bfSize;
    UINT    bfReserved1;
    UINT    bfReserved2;
    DWORD   bfOffBits;
} BITMAPFILEHEADER;

The BITMAPFILEHEADER structure contains information about the type, size,
and layout of a device-independent bitmap (DIB) file.

Member       Description
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

bfType       Specifies the type of file. This member must be BM.

bfSize       Specifies the size of the file, in bytes.

bfReserved1  Reserved; must be set to zero.

bfReserved2  Reserved; must be set to zero.

bfOffBits    Specifies the byte offset from the BITMAPFILEHEADER structure
             to the actual bitmap data in the file.

Comments

A BITMAPINFO or BITMAPCOREINFO structure immediately follows the
BITMAPFILEHEADER structure in the DIB file.

See Also

BITMAPCOREINFO, BITMAPINFO
///////////////////////////////////////////////////////////////////////////
Topic:  BITMAPCOREINFO (3.0) 

typedef struct tagBITMAPCOREINFO {  /* bmci */
    BITMAPCOREHEADER bmciHeader;
    RGBTRIPLE        bmciColors[1];
} BITMAPCOREINFO;

The BITMAPCOREINFO structure fully defines the dimensions and color
information for a device-independent bitmap (DIB). Windows applications
should use the BITMAPINFO structure instead of BITMAPCOREINFO whenever
possible.

Member      Description
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

bmciHeader  Specifies a BITMAPCOREHEADER structure that contains information
            about the dimensions and color format of a DIB.

bmciColors  Specifies an array of RGBTRIPLE structures that define the
            colors in the bitmap.

Comments

The BITMAPCOREINFO structure describes the dimensions and colors of a
bitmap. It is followed immediately in memory by an array of bytes which
define the pixels of the bitmap. The bits in the array are packed together,
but each scan line must be zero-padded to end on a LONG boundary. Segment
boundaries, however, can appear anywhere in the bitmap. The origin of the
bitmap is the lower-left corner.

The bcBitCount member of the BITMAPCOREHEADER structure determines the
number of bits that define each pixel and the maximum number of colors in
the bitmap. This member may be set to any of the following values:

Value  Meaning
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
1      The bitmap is monochrome, and the bmciColors member must contain two
       entries. Each bit in the bitmap array represents a pixel. If the bit
       is clear, the pixel is displayed with the color of the first entry in
       the bmciColors table. If the bit is set, the pixel has the color of
       the second entry in the table.

4      The bitmap has a maximum of 16 colors, and the bmciColors member
       contains 16 entries. Each pixel in the bitmap is represented by a
       four-bit index into the color table.

       For example, if the first byte in the bitmap is 0x1F, the byte
       represents two pixels. The first pixel contains the color in the
       second table entry, and the second pixel contains the color in the
       sixteenth table entry.

8      The bitmap has a maximum of 256 colors, and the bmciColors member
       contains 256 entries. In this case, each byte in the array represents
       a single pixel.

24     The bitmap has a maximum of 2^24 colors. The bmciColors member is
       NULL, and each 3-byte sequence in the bitmap array represents the
       relative intensities of red, green, and blue, respectively, of a
       pixel.

The colors in the bmciColors table should appear in order of importance.

Alternatively, for functions that use DIBs, the bmciColors member can be an
array of 16-bit unsigned integers that specify an index into the currently
realized logical palette instead of explicit RGB values. In this case, an
application using the bitmap must call DIB functions with the wUsage
parameter set to DIB_PAL_COLORS.

Note:  The bmciColors member should not contain palette indexes if the
       bitmap is to be stored in a file or transferred to another
       application. Unless the application uses the bitmap exclusively and
       under its complete control, the bitmap color table should contain
       explicit RGB values.

See Also

BITMAPINFO, BITMAPCOREHEADER, RGBTRIPLE
//////////////////////////////////////////////////////////////////////////

Topic:  RGBTRIPLE (3.0) 

typedef struct tagRGBTRIPLE {   /* rgbt */
    BYTE    rgbtBlue;
    BYTE    rgbtGreen;
    BYTE    rgbtRed;
} RGBTRIPLE;

The RGBTRIPLE structure describes a color consisting of relative intensities
of red, green, and blue. The bmciColors member of the BITMAPCOREINFO
structure consists of an array of RGBTRIPLE structures.

Windows applications should use the BITMAPINFO structure instead of
BITMAPCOREINFO whenever possible. The BITMAPINFO structure uses an RGBQUAD
structure instead of the RGBTRIPLE structure.

Member     Description
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

rgbtBlue   Specifies the intensity of blue in the color.

rgbtGreen  Specifies the intensity of green in the color.

rgbtRed    Specifies the intensity of red in the color.

See Also

BITMAPCOREINFO, BITMAPINFO, RGBQUAD
///////////////////////////////////////////////////////////////////////////
Topic:  RGBTRIPLE (3.0) 

typedef struct tagRGBTRIPLE {   /* rgbt */
    BYTE    rgbtBlue;
    BYTE    rgbtGreen;
    BYTE    rgbtRed;
} RGBTRIPLE;

The RGBTRIPLE structure describes a color consisting of relative intensities
of red, green, and blue. The bmciColors member of the BITMAPCOREINFO
structure consists of an array of RGBTRIPLE structures.

Windows applications should use the BITMAPINFO structure instead of
BITMAPCOREINFO whenever possible. The BITMAPINFO structure uses an RGBQUAD
structure instead of the RGBTRIPLE structure.

Member     Description
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

rgbtBlue   Specifies the intensity of blue in the color.

rgbtGreen  Specifies the intensity of green in the color.

rgbtRed    Specifies the intensity of red in the color.

See Also

BITMAPCOREINFO, BITMAPINFO, RGBQUAD
/////////////////////////////////////////////////////////////////////
Topic:  BITMAPINFO (3.0) 

typedef struct tagBITMAPINFO {  /* bmi */
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
} BITMAPINFO;

The BITMAPINFO structure fully defines the dimensions and color information
for a Windows 3.0 or later device-independent bitmap (DIB).

Member     Description
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

bmiHeader  Specifies a BITMAPINFOHEADER structure that contains information
           about the dimensions and color format of a DIB.

bmiColors  Specifies an array of RGBQUAD structures that define the colors
           in the bitmap.

Comments

A Windows 3.0 or later DIB consists of two distinct parts: a BITMAPINFO
structure, which describes the dimensions and colors of the bitmap, and an
array of bytes defining the pixels of the bitmap. The bits in the array are
packed together, but each scan line must be zero-padded to end on a LONG
boundary. Segment boundaries, however, can appear anywhere in the bitmap.
The origin of the bitmap is the lower-left corner.

The biBitCount member of the BITMAPINFOHEADER structure determines the
number of bits which define each pixel and the maximum number of colors in
the bitmap. This member may be set to any of the following values:

Value  Meaning
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
1      The bitmap is monochrome, and the bmciColors member must contain two
       entries. Each bit in the bitmap array represents a pixel. If the bit
       is clear, the pixel is displayed with the color of the first entry in
       the bmciColors table. If the bit is set, the pixel has the color of
       the second entry in the table.

4      The bitmap has a maximum of 16 colors, and the bmciColors member
       contains 16 entries. Each pixel in the bitmap is represented by a
       four-bit index into the color table.

       For example, if the first byte in the bitmap is 0x1F, the byte
       represents two pixels. The first pixel contains the color in the
       second table entry, and the second pixel contains the color in the
       sixteenth table entry.

8      The bitmap has a maximum of 256 colors, and the bmciColors member
       contains 256 entries. In this case, each byte in the array represents
       a single pixel.

24     The bitmap has a maximum of 2^24 colors. The bmciColors member is
       NULL, and each 3-byte sequence in the bitmap array represents the
       relative intensities of red, green, and blue, respectively, of a
       pixel.

The biClrUsed member of the BITMAPINFOHEADER structure specifies the number
of color indexes in the color table actually used by the bitmap. If the
biClrUsed member is set to zero, the bitmap uses the maximum number of
colors corresponding to the value of the biBitCount member.

The colors in the bmiColors table should appear in order of importance.

Alternatively, for functions that use DIBs, the bmiColors member can be an
array of 16-bit unsigned integers that specify an index into the currently
realized logical palette instead of explicit RGB values. In this case, an
application using the bitmap must call DIB functions with the wUsage
parameter set to DIB_PAL_COLORS.

Note:  The bmiColors member should not contain palette indexes if the bitmap
       is to be stored in a file or transferred to another application.
       Unless the application uses the bitmap exclusively and under its
       complete control, the bitmap color table should contain explicit RGB
       values.

See Also

BITMAPINFOHEADER, RGBQUAD
//////////////////////////////////////////////////////////////////
Topic:  BITMAPINFOHEADER (3.0) 

typedef struct tagBITMAPINFOHEADER {    /* bmih */
    DWORD   biSize;
    LONG    biWidth;
    LONG    biHeight;
    WORD    biPlanes;
    WORD    biBitCount;
    DWORD   biCompression;
    DWORD   biSizeImage;
    LONG    biXPelsPerMeter;
    LONG    biYPelsPerMeter;
    DWORD   biClrUsed;
    DWORD   biClrImportant;
} BITMAPINFOHEADER;

The BITMAPINFOHEADER structure contains information about the dimensions and
color format of a Windows 3.0 or later device-independent bitmap (DIB).

Member           Description
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

biSize           Specifies the number of bytes required by the
                 BITMAPINFOHEADER structure.

biWidth          Specifies the width of the bitmap, in pixels.

biHeight         Specifies the height of the bitmap, in pixels.

biPlanes         Specifies the number of planes for the target device. This
                 member must be set to 1.

biBitCount       Specifies the number of bits per pixel. This value must be
                 1, 4, 8, or 24.

biCompression    Specifies the type of compression for a compressed bitmap.
                 It can be one of the following values:

                 Value    Meaning
                 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
                 BI_RGB   Specifies that the bitmap is not compressed.

                 BI_RLE8  Specifies a run-length encoded format for bitmaps
                          with 8 bits per pixel. The compression format is a
                          2-byte format consisting of a count byte followed
                          by a byte containing a color index. For more
                          information, see the following Comments section.

                 BI_RLE4  Specifies a run-length encoded format for bitmaps
                          with 4 bits per pixel. The compression format is a
                          2-byte format consisting of a count byte followed
                          by two word-length color indexes. For more
                          information, see the following Comments section.

biSizeImage      Specifies the size, in bytes, of the image. It is valid to
                 set this member to zero if the bitmap is in the BI_RGB
                 format.

biXPelsPerMeter  Specifies the horizontal resolution, in pixels per meter,
                 of the target device for the bitmap. An application can use
                 this value to select a bitmap from a resource group that
                 best matches the characteristics of the current device.

biYPelsPerMeter  Specifies the vertical resolution, in pixels per meter, of
                 the target device for the bitmap.

biClrUsed        Specifies the number of color indexes in the color table
                 actually used by the bitmap. If this value is zero, the
                 bitmap uses the maximum number of colors corresponding to
                 the value of the biBitCount member. For more information on
                 the maximum sizes of the color table, see the description
                 of the BITMAPINFO structure earlier in this topic.

                 If the biClrUsed member is nonzero, it specifies the actual
                 number of colors that the graphics engine or device driver
                 will access if the biBitCount member is less than 24. If
                 biBitCount is set to 24, biClrUsed specifies the size of
                 the reference color table used to optimize performance of
                 Windows color palettes.

                 If the bitmap is a packed bitmap (that is, a bitmap in
                 which the bitmap array immediately follows the BITMAPINFO
                 header and which is referenced by a single pointer), the
                 biClrUsed member must be set to zero or to the actual size
                 of the color table.

biClrImportant   Specifies the number of color indexes that are considered
                 important for displaying the bitmap. If this value is zero,
                 all colors are important.

Comments

The BITMAPINFO structure combines the BITMAPINFOHEADER structure and a color
table to provide a complete definition of the dimensions and colors of a
Windows 3.0 or later DIB. For more information about specifying a Windows
3.0 DIB, see the description of the BITMAPINFO structure.

An application should use the information stored in the biSize member to
locate the color table in a BITMAPINFO structure as follows:

pColor = ((LPSTR) pBitmapInfo + (WORD) (pBitmapInfo->bmiHeader.biSize))

Windows supports formats for compressing bitmaps that define their colors
with 8 bits per pixel and with 4 bits per pixel. Compression reduces the
disk and memory storage required for the bitmap. The following paragraphs
describe these formats.

BI_RLE8

When the biCompression member is set to BI_RLE8, the bitmap is compressed
using a run-length encoding format for an 8-bit bitmap. This format may be
compressed in either of two modes: encoded and absolute. Both modes can
occur anywhere throughout a single bitmap.

Encoded mode consists of two bytes: the first byte specifies the number of
consecutive pixels to be drawn using the color index contained in the second
byte. In addition, the first byte of the pair can be set to zero to indicate
an escape that denotes an end of line, end of bitmap, or a delta. The
interpretation of the escape depends on the value of the second byte of the
pair. The following list shows the meaning of the second byte:

Value  Meaning
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
0      End of line.

1      End of bitmap.

2      Delta. The two bytes following the escape contain unsigned values
       indicating the horizontal and vertical offset of the next pixel from
       the current position.

Absolute mode is signaled by the first byte set to zero and the second byte
set to a value between 0x03 and 0xFF. In absolute mode, the second byte
represents the number of bytes that follow, each of which contains the color
index of a single pixel. When the second byte is set to 2 or less, the
escape has the same meaning as in encoded mode. In absolute mode, each run
must be aligned on a word boundary.

The following example shows the hexadecimal values of an 8-bit compressed
bitmap:

03 04 05 06 00 03 45 56 67 00 02 78 00 02 05 01
02 78 00 00 09 1E 00 01

This bitmap would expand as follows (two-digit values represent a color
index for a single pixel):

04 04 04
06 06 06 06 06
45 56 67
78 78
move current position 5 right and 1 down
78 78
end of line
1E 1E 1E 1E 1E 1E 1E 1E 1E
end of RLE bitmap

BI_RLE4

When the biCompression member is set to BI_RLE4, the bitmap is compressed
using a run-length encoding (RLE) format for a 4-bit bitmap, which also uses
encoded and absolute modes. In encoded mode, the first byte of the pair
contains the number of pixels to be drawn using the color indexes in the
second byte. The second byte contains two color indexes, one in its
high-order nibble (that is, its low-order four bits) and one in its
low-order nibble. The first of the pixels is drawn using the color specified
by the high-order nibble, the second is drawn using the color in the
low-order nibble, the third is drawn with the color in the high-order
nibble, and so on, until all the pixels specified by the first byte have
been drawn.

In absolute mode, the first byte contains zero, the second byte contains the
number of color indexes that follow, and subsequent bytes contain color
indexes in their high- and low-order nibbles, one color index for each
pixel. In absolute mode, each run must be aligned on a word boundary. The
end-of-line, end-of-bitmap, and delta escapes also apply to BI_RLE4.

The following example shows the hexadecimal values of a 4-bit compressed
bitmap:

03 04 05 06 00 06 45 56 67 00 04 78 00 02 05 01
04 78 00 00 09 1E 00 01

This bitmap would expand as follows (single-digit values represent a color
index for a single pixel):

0 4 0
0 6 0 6 0
4 5 5 6 6 7
7 8 7 8
move current position 5 right and 1 down
7 8 7 8
end of line
1 E 1 E 1 E 1 E 1
end of RLE bitmap

See Also

BITMAPINFO
////////////////////////////////////////////////////////////////////

Topic:  RGBQUAD (3.0) 

typedef struct tagRGBQUAD {     /* rgbq */
    BYTE    rgbBlue;
    BYTE    rgbGreen;
    BYTE    rgbRed;
    BYTE    rgbReserved;
} RGBQUAD;

The RGBQUAD structure describes a color consisting of relative intensities
of red, green, and blue. The bmiColors member of the BITMAPINFO structure
consists of an array of RGBQUAD structures.

Member       Description
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

rgbBlue      Specifies the intensity of blue in the color.

rgbGreen     Specifies the intensity of green in the color.

rgbRed       Specifies the intensity of red in the color.

rgbReserved  Not used; must be set to zero.

See Also

BITMAPINFO
#endif
