
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

typedef unsigned long ULONG;
typedef void far *FP;
#ifndef MK_FP
#define MK_FP(seg,ofs)  ((FP)(((ULONG)(seg) << 16) | (ofs)))
#endif


#pragma pack(1)
struct bmpfileheader
{
int  Filler1;
int  Filler2;
int  Filler3;
char Header[2];
int  Filler5;
int  Filler6;
int  Filler7;
long File_Size;
long HeaderSize;
};

struct bmpheader
{
long HeaderSize;
long Width;
long Height;
int  Planes;
int  BitCount;
long Compression;
long ImageSize;
long xPelsPerMeter;
long yPelsPerMeter;
long ColoursUsed;
long ColoursImporntant;
};

struct RGBTriple
{
unsigned char Blue;
unsigned char Green;
unsigned char Red;
unsigned char Unused;
};
#pragma pack()

unsigned char XORBits  [32][4];

unsigned char aRed    [32][32];
unsigned char aGreen  [32][32];
unsigned char aBlue   [32][32];
unsigned char aIntens [32][32];

unsigned char bRed    [32][32];
unsigned char bGreen  [32][32];
unsigned char bBlue   [32][32];
unsigned char bIntens [32][32];

int SVGA = 0;
unsigned char buf    [640];
struct bmpfileheader fileheader;
struct bmpheader     header;
struct RGBTriple     Colors[256];

char far *Color = 0;
char far *Vio = 0xa0000000L;
char far *Vio_1 = 0;

FILE *f;
unsigned char Original_Mode;
unsigned char Max_Cols;
unsigned char Max_Rows;
int VGA_TYPE;          /* 1 = ATI        */
                       /* 2 = C&T        */
                       /* 3 = GENOA      */
                       /* 4 = PARADISE   */
                       /* 5 = TRIDENT    */
                       /* 6 = TSENG      */
                       /* 7 = VIDEO7     */

void Which_Vga(void)
{
   unsigned char old,val,id;
   char far *para;
   char str[4];
   int base;

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³    ATI                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  para = MK_FP(0xc000,0x0031);
  if(_fstrncmp (para,(char far *)"761295520",9) == 0)
     {
      VGA_TYPE = 1;
      return;
     }
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³    C&T                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
   if((unsigned char)inp(0x0104) == 0xa5)
     {
      VGA_TYPE = 2;
      return;
     }
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³    GENOA                                                ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  para = MK_FP(0xc000,0x0037);
  memcpy (str,para,4);
  if((str[0] == 0x77) && (str[2] == 0x66) &&(str[3] == 0x99))
     {
      VGA_TYPE = 3;
      return;
     }
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³    PARADISE                                             ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  para = MK_FP(0xc000,0x007d);
  if(_fstrncmp (para,(char far *)"VGA=",4) == 0)
     {
      VGA_TYPE = 4;
      return;
     }
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³    TRIDENT                                              ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
   outp(0x03c4,0x0e);
   old = (unsigned char)inp(0x03c5);
   outp(0x03c5,0);
   val = (unsigned char)inp(0x3c5);
   val &= 0x000f;
   outp(0x3c5,old);
   if(val == 0x02)
     {
      VGA_TYPE = 5;
      return;
     }
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³    VIDEO 7                                              ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
   if( (unsigned char)inp(0x3cc) & 0x01 == 1)
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
   if(id==(0x55^0xea))
      {
        VGA_TYPE = 7;
        return;
      }
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³    ASSUME TSENG                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
   VGA_TYPE = 6;
   return;


}


void Setpaletteregisters(void)
{
union REGS regs;
int i;

for(i=0;i<16;i++)
   {
      outp(0x3c8,i);
      outp(0x3c9,(unsigned char)Colors[i].Red  /4);
      outp(0x3c9,(unsigned char)Colors[i].Green/4);
      outp(0x3c9,(unsigned char)Colors[i].Blue /4);
   }
}

void gotoxy(int r,int c)
{
union REGS regs;

regs.h.ah = 0x02;
regs.h.bh = 0;
regs.h.dh = (unsigned char)r;
regs.h.dl = (unsigned char)c;
int86(0x10,&regs,&regs);
}

int  Init_graph(int x)
{
union REGS regs;

regs.h.ah = 0x0f;
int86(0x10,&regs,&regs);
Original_Mode = regs.h.al;

if(Original_Mode == (unsigned char)x)
     return(0);

regs.h.ah = 0x00;
regs.h.al = (unsigned char)x;
int86(0x10,&regs,&regs);

regs.h.ah = 0x0f;
int86(0x10,&regs,&regs);
if(regs.h.al == (unsigned char)x)
  {
     return(0);
  }
regs.h.ah = 0x00;
regs.h.al = Original_Mode;
int86(0x10,&regs,&regs);
   return(1);

}
void DoLine(char *data ,int width)
{
   _asm
      {
	push   ES
	push   DS
	push   DI
	push   SI
	push   CX
	push   DX

	MOV    CX,width
	les di,Vio
	lds si,data
	cld
	rep movsb

	POP  DX
	POP  CX
	POP  SI
	POP  DI
	POP  DS
	POP  ES
      }

}

void DrawIt(int mode1,int width1)
{
  int mode  = mode1;
  int width = width1;
//gotoxy(15,0);
//printf("Vio_1    %p\n",Vio_1);
//printf("Vio      %p\n",Vio);
//printf("width    %d\n",width);
//printf("mode     %d\n",mode);

              _asm
                 {
                   push   ES
                   push   DS
                   push   DI
                   push   SI
		   push   CX
		   push   DX

                   MOV    DX,03c4h
                   MOV    AX,mode
                   out    dx,ax
		   XOR	  ax,ax
		   les	  di,Vio
                   MOV    CX,width
		   cld
		   rep	  stosb

		   POP	DX
		   POP	CX
                   POP  SI
                   POP  DI
                   POP  DS
                   POP  ES
		 }
              _asm
                 {
                   push   ES
                   push   DS
                   push   DI
                   push   SI
		   push   CX
		   push   DX

                   MOV    DX,03c4h
                   MOV    AX,mode
                   out    dx,ax
		   les di,Vio
                   lds si,Color
                   MOV    CX,width
                   cld
                   rep movsb

		   POP	DX
		   POP	CX
                   POP  SI
                   POP  DI
                   POP  DS
                   POP  ES
                 }
}
void DrawBit(int mode,int width,char bytein,char bolor,char bolor1)
{
char byt=0;
int wide = 8 - width;

gotoxy(20,0);
//printf("Vio_1    %p\n",Vio_1);
//printf("Vio      %p\n",Vio);
//printf("width    %d\n",width);
//printf("mode     %d\n",mode);

              _asm
                 {
                   push   ES
                   push   DS
                   push   DI
                   push   SI

                   LES    di,Vio_1
                   MOV    DX,03ceh             // Read mode
                   MOV    al,04h
                   MOV    ah,bolor
                   OUT    dx,ax
		   MOV	  al,BYTE PTR ES:[DI]

		   mov	  cl,wide
		   SHL	  al,cl
		   mov	  ah,0
		   SHR	  al,cl

                   OR     al,bytein
                   push   ax
                   MOV    DX,03c4h             // Read mode
                   MOV    al,02h
                   MOV    ah,bolor1
                   OUT    dx,ax
                   pop    ax
                   MOV    BYTE PTR ES:[DI],al

                   POP    SI
                   POP    DI
                   POP    DS
                   POP    ES
                }
}

void ClearScreen (char color)
{
   if (SVGA)
   {

   }
   else
   {
      _asm	      /* clear to black */
	  {
	       PUSH   ES
	       PUSH   DI
	       PUSH   DX
	       PUSH   CX
	       MOV    DX,03c4h
	       MOV    ax,00f02h
	       out    dx,ax
	       XOR    ax,ax
	       mov    al,80
	       mov    cx,320   /* words */
	       mul    cx
	       mov    cx,ax
	       XOR    ax,ax
	       les    di,Vio
	       cld
	       rep    stosw
	       POP    CX
	       POP    DX
	       POP    DI
	       POP    ES
	  }

      _asm
	  {
	       PUSH   ES
	       PUSH   DI
	       PUSH   DX
	       PUSH   CX
	       MOV    DX,03c4h
	       MOV    ah,color
	       MOV    al,002h
	       out    dx,ax
	       XOR    ax,ax
	       mov    al,80
	       mov    cx,320   /* words */
	       mul    cx
	       mov    cx,ax
	       mov    ax,0ffffh
	       les    di,Vio
	       cld
	       rep    stosw
	       POP    CX
	       POP    DX
	       POP    DI
	       POP    ES
	  }
   }
}

void ClearBitMap( void )
{
union REGS regs;

   regs.h.ah = 0x00;
   regs.h.al = 3;
   int86(0x10,&regs,&regs);
}

void PaintBitMap(int PaddedWidth, int Padding, int Masker , int xPos, int yPos)
{
 int j;

   Vio = 0xa0000000L + ((int)(header.Height+ yPos) * 80L ) + xPos/8 ;

   for(j=0;j<(int)header.Height/2;j++)	// For each row in the file
   {
      Vio_1 = ( Vio + ( header.Width /8)  );
      Color = aBlue[j];
      DrawIt(0x0102,(int)header.Width/8);
      if ( Padding )
      {
	 aBlue[j][(PaddedWidth/8)-1]&=Masker;
	 DrawBit(0x0102,Padding,aBlue[j][(PaddedWidth/8)-1],0,0x01);
	 gotoxy(25,30);
	 gotoxy(25,30);
      }
      Color = aGreen[j];
      DrawIt(0x0202,(int)header.Width/8);
      if ( Padding )
      {
	 aGreen[j][(PaddedWidth/8)-1]&=Masker;
	 DrawBit(0x0202,Padding,aGreen[j][(PaddedWidth/8)-1],1,0x02);
	 gotoxy(26,30);
	 gotoxy(26,30);
      }
      Color = aRed[j];
      DrawIt(0x0402,(int)header.Width/8);
      if ( Padding )
      {
	 aRed[j][(PaddedWidth/8)-1]&=Masker;
	 DrawBit(0x0402,Padding,aRed[j][(PaddedWidth/8)-1],2,0x04);
	 gotoxy(27,30);
	 gotoxy(27,30);
      }
      Color = aIntens[j];
      DrawIt(0x0802,(int)header.Width/8);
      if ( Padding )
      {
	 aIntens[j][(PaddedWidth/8)-1]&=Masker;
	 DrawBit(0x0802,Padding,aIntens[j][(PaddedWidth/8)-1],3,0x08);
	 gotoxy(28,30);
	 gotoxy(28,30);
      }
      Vio -=  80 ;
   }
}

char *ReadScreenMapData (int mode, char *Where , int len)
{
   char data[32];
   char *data1;
   data1 = &data[0];
   _asm
   {
      push   ES
      push   DS
      push   DI
      push   SI

      MOV    DX,03ceh		  // Read mode
      MOV    ax,mode
      OUT    dx,ax

      mov    cx,len
      LES    di,data1
      LDS    si,Where
      cld
      rep movsb

      POP    SI
      POP    DI
      POP    DS
      POP    ES
   }

   return (data);
}

void   ReadScreenData ( int xPos , int yPos)
{
 int j;
   Vio = 0xa0000000L + ((int)(header.Height+ yPos) * 80L ) + xPos/8 ;
   for(j=0;j<(int)header.Height/2;j++)	// For each row in the file
   {
      memcpy(bRed[j]	 , ReadScreenMapData (0x0404,Vio , (int)header.Width/8), (int)header.Width/8);
      memcpy(bGreen[j]	 , ReadScreenMapData (0x0204,Vio , (int)header.Width/8), (int)header.Width/8);
      memcpy(bBlue[j]	 , ReadScreenMapData (0x0104,Vio , (int)header.Width/8), (int)header.Width/8);
      memcpy(bIntens[j]  , ReadScreenMapData (0x0804,Vio , (int)header.Width/8), (int)header.Width/8);
      Vio -=  80 ;
   }
}

void   XORData ( void)
{
 int j,i,k;

   for(j=0;j<(int)header.Height/2;j++)	// For each row in the file
   {
       for(i=0;i<header.Width/8;i++)  // For byte in the row
       {
	  for(k = 0x80;k > 0;k=k>>1)  // For bit in the byte
	  {
	     if(XORBits[j][i/8] & k)
	     {
		aRed[j][i]	|= bRed[j][i];
		aGreen[j][i]	|= bGreen[j][i];
		aBlue[j][i]	|= bBlue[j][i];
		aIntens[j][i]	|= bIntens[j][i];
	     }
	     else
	     {
		aRed[j][i]	^= bRed[j][i];
		aGreen[j][i]	^= bGreen[j][i];
		aBlue[j][i]	^= bBlue[j][i];
		aIntens[j][i]	^= bIntens[j][i];
	     }
	  }
       }
   }

}

void DrawBitMap(char *FileName, int xPos , int yPos)
{
union REGS regs;
  int first = 1;
  int i,j,col,row,numpals,k,l,count=1,point;
  long size;
  int PaddedWidth;
  int Padding;
  int Masker;


   if((f = fopen(FileName,"rb")) == NULL)
      {
	 printf("error opening %s\n",FileName);
         exit(0);
      }

   if(fread(&fileheader,sizeof(fileheader),1,f) == 0)
      {
	 printf("invalid file\n",FileName);
         exit(0);
      }
   fseek(f, fileheader.HeaderSize ,SEEK_SET);
   if(fread(&header,sizeof(header),1,f) == 0)
      {
	 printf("invalid file\n",FileName);
         exit(0);
      }
    printf("header.BitCount = %d",header.BitCount);
   switch(header.BitCount)
      {
         case 1 : numpals = 0;   break;
         case 4 : numpals = 16;  break;
         case 8 : numpals = 256; break;
         case 24: numpals = 256; break;
         default:
		     printf("invalid ICO file\n",FileName);
                     exit(0);
      }
   for(i=0;i<numpals;i++)
      if(fread(&Colors[i],sizeof(Colors[i]),1,f) == 0)
         {
	    printf("invalid file\n",FileName);
            exit(0);
         }

  //size = ftell(f);
  //if(size != fileheader.Offset_To_Start)
  //  {
  //	printf("error in ICO file size = %ld  -> %ld\n",size,fileheader.Offset_To_Start);
  //	exit(0);
  //  }

   Which_Vga();
   if(Init_graph(0x12))
      exit(0);

   Vio = 0xa0000000L + ((int)(header.Height+ yPos) * 80L ) + xPos/8 ;
   //Setpaletteregisters();
   //ClearScreen(1);

   PaddedWidth =(int) header.Width;
   Padding     = 0;
   if(header.BitCount == 4)
   {
      if ( ( header.Width % 8 ) )
      {
	 PaddedWidth=(int)header.Width + ( 8 - ((int)header.Width % 8 ));
	 Padding    = ( 8 - ((int)header.Width % 8 ));
	 switch ( Padding )
	 {
	    case 1: Masker =0xfe; break;
	    case 2: Masker =0xfc; break;
	    case 3: Masker =0xf8; break;
	    case 4: Masker =0xf0; break;
	    case 5: Masker =0xe0; break;
	    case 6: Masker =0xc0; break;
	    case 7: Masker =0x80; break;
	 } /* endswitch */
      }

      k = 0;
      for(j=0;j<(int)header.Height/2;j++)  // For each row in the file
      {
	 l=0;
	 fread(buf,1,(unsigned int)PaddedWidth/2,f);

	 memset (aRed	[j]  ,0,32);
	 memset (aGreen [j]  ,0,32);
	 memset (aBlue	[j]  ,0,32);
	 memset (aIntens[j]  ,0,32);
	 point = 0;

	 for(k=0,first=1;k<PaddedWidth;k+=2,l++)

	 {
	    /*	if width odd pad to %8/2 */

	    if(first)
	       first = 0;
	    else
	       if(k%8 == 0) point++;

	    switch(buf[l] & 0xf0)    // 0000 0000
	    {			     // ÄÄÄÄ
	       case 0x00  : break;
	       case 0x10  : aRed[j][point]    |= count << (7 - (k%8)) ; break;
	       case 0x20  : aGreen[j][point]  |= count << (7 - (k%8)) ; break;
	       case 0x30  : aRed[j][point]    |= count << (7 - (k%8)) ; aGreen[j][point]  |= count << (7 - (k%8)) ; break;
	       case 0x40  : aBlue[j][point]   |= count << (7 - (k%8)) ; break;
	       case 0x50  : aRed[j][point]    |= count << (7 - (k%8)) ; aBlue[j][point]   |= count << (7 - (k%8)) ; break;
	       case 0x60  : aGreen[j][point]  |= count << (7 - (k%8)) ; aBlue[j][point]   |= count << (7 - (k%8)) ; break;
/*    DONT ASK ? case 7 & 8 are switched  (bug in bitmaps)  */
	       case 0x70  : aIntens[j][ point] |= count << (7 - (k%8)) ; break;
	       case 0x80  : aGreen[j][point]   |= count << (7 - (k%8)) ; aBlue[j][point]   |= count << (7 - (k%8)) ; aRed[j][point]    |= count << (7 - (k%8)) ; break;
	       case 0x90  : aIntens[j][ point] |= count << (7 - (k%8)) ; aRed[j][point]    |= count << (7 - (k%8)) ; break;
	       case 0xa0  : aIntens[j][ point] |= count << (7 - (k%8)) ; aGreen[j][point]  |= count << (7 - (k%8)) ; break;
	       case 0xb0  : aIntens[j][ point] |= count << (7 - (k%8)) ; aRed[j][point]    |= count << (7 - (k%8)) ; aGreen[j][point]  |= count << (7 - (k%8)) ; break;
	       case 0xc0  : aIntens[j][ point] |= count << (7 - (k%8)) ; aBlue[j][point]   |= count << (7 - (k%8)) ; break;
	       case 0xd0  : aIntens[j][ point] |= count << (7 - (k%8)) ; aRed[j][point]    |= count << (7 - (k%8)) ; aBlue[j][point]   |= count << (7 - (k%8)) ; break;
	       case 0xe0  : aIntens[j][ point] |= count << (7 - (k%8)) ; aGreen[j][point]  |= count << (7 - (k%8)) ; aBlue[j][point]   |= count << (7 - (k%8)) ; break;
	       case 0xf0  : aIntens[j][ point] |= count << (7 - (k%8)) ; aRed[j][point]    |= count << (7 - (k%8)) ; aGreen[j][point]  |= count << (7 - (k%8)) ; aBlue[j][point]   |= count << (7 - (k%8)) ; break;
	    }
	    switch(buf[l] & 0x0f)
	    {
	       case 0x00  : break;
	       case 0x01  : aRed[j][point]    |= count << (6 - (k%8)) ;break;
	       case 0x02  : aGreen[j][point]  |= count << (6 - (k%8)) ;break;
	       case 0x03  : aRed[j][point]    |= count << (6 - (k%8)) ;aGreen[j][point] |= count << (6 - (k%8)) ;break;
	       case 0x04  : aBlue[j][point]   |= count << (6 - (k%8)) ;break;
	       case 0x05  : aRed[j][point]    |= count << (6 - (k%8)) ;aBlue[j][point]	|= count << (6 - (k%8)) ;break;
	       case 0x06  : aGreen[j][point]  |= count << (6 - (k%8)) ;aBlue[j][point]	|= count << (6 - (k%8)) ;break;
/*    DONT ASK ? case 7 & 8 are switched  (bug in bitmaps)   */
	       case 0x07  : aIntens[j][ point]|= count << (6 - (k%8)) ;break;
	       case 0x08  : aGreen[j][point]  |= count << (6 - (k%8)) ;aBlue[j][point]	|= count << (6 - (k%8)) ;aRed[j][point] |= count << (6 - (k%8)) ;break;
	       case 0x09  : aIntens[j][ point] |= count << (6 - (k%8)) ;aRed[j][point]	 |= count << (6 - (k%8)) ;break;
	       case 0x0a  : aIntens[j][ point] |= count << (6 - (k%8)) ;aGreen[j][point] |= count << (6 - (k%8)) ;break;
	       case 0x0b  : aIntens[j][ point] |= count << (6 - (k%8)) ;aRed[j][point]	 |= count << (6 - (k%8)) ;aGreen[j][point] |= count << (6 - (k%8)) ;break;
	       case 0x0c  : aIntens[j][ point] |= count << (6 - (k%8)) ;aBlue[j][point]  |= count << (6 - (k%8)) ;break;
	       case 0x0d  : aIntens[j][ point] |= count << (6 - (k%8)) ;aRed[j][point]	 |= count << (6 - (k%8)) ;aBlue[j][point] |= count << (6 - (k%8)) ;break;
	       case 0x0e  : aIntens[j][ point] |= count << (6 - (k%8)) ;aGreen[j][point] |= count << (6 - (k%8)) ;aBlue[j][point] |= count << (6 - (k%8)) ;break;
	       case 0x0f  : aIntens[j][ point] |= count << (6 - (k%8)) ;aRed[j][point]	 |= count << (6 - (k%8)) ;aGreen[j][point] |= count << (6 - (k%8)) ;aBlue[j][point] |= count << (6 - (k%8)) ;break;
	    }
	 }
      }
   }
   fread(XORBits,1,sizeof(XORBits),f);
   ReadScreenData ((640-header.Width)/2,(480-header.Height/2)/2);
   XORData ();
   while(!kbhit())
     PaintBitMap(PaddedWidth, Padding, Masker,rand()%(640-header.Width),rand()%(480-header.Height/2));
     //PaintBitMap(PaddedWidth, Padding, Masker,(640-header.Width)/2,(480-header.Height/2)/2);

  regs.x.ax = 0x0f;
  regs.x.cx = 8;
  regs.x.dx = 8;
  int86(0x33,&regs,&regs);
  regs.x.ax = 1;
  int86(0x33,&regs,&regs);

while(!kbhit())
  {
     regs.x.ax = 0x0003;
     int86(0x33,&regs,&regs);
     col = (regs.x.cx) ;
     row = (regs.x.dx) ;
     if  (regs.x.bx & 0x0001)
        {
	  //gotoxy(29,1);
	  //printf("%3d   %3d",row,col);
        }
  }
  fclose (f);
}
int main (int argc , char **argv)
{
  char far *Image;

     DrawBitMap(argv[1], 10 , 10);
     getch();

     ClearBitMap();
}

