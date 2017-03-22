#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char  Font[4096]={0};

FILE          *InFile;
FILE          *OutFile;

unsigned char  Hdr[25] = { 0x8c, 0xc8, 0x8e, 0xc0, 0x8d, 0x2e, 0x19, 0x01,
                           0xba, 0x00, 0x00, 0xb3, 0x00, 0xb7, 0x10, 0xb4,
                           0x11, 0xb0, 0x00, 0xb9, 0xff, 0x00, 0xcd, 0x10,
                           0xc3 };

int ValidComFileName ( char *ComFileName );
int CloseInFile      ( void );
int CloseOutFile     ( void );
int CreateOutFile    ( char *FileName, int COMFile );
void SetFont         ( char _far *Ptr );


int main( int argc, char *argv[] )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   if ( argc < 2 )
   {
      printf("USAGE: FONT <InFile>               :  Set font\n");
      printf("   or: FONT <InFile> <OutFile>     :  Create font file according to extension.\n");
      printf("\n");
      printf("       <InFile>  = (a) File containing 256 8x16 font bitmaps (4096 bytes).\n");
      printf("                or (b) Windows 256 8x16 .FNT file.\n");

      printf("      <OutFile>  = (a) File containing 256 8x16 font bitmaps (4096 bytes).\n");
      printf("                or (b) .COM file to set font.\n");

      return(1);
   }

   strupr(argv[1]);

   if ( (InFile = fopen(argv[1],"r+b" )) == NULL)
   {
      printf("Error: unable to open InFile : %s\n", argv[1]);
      return(1);
   }


   if (strstr(argv[1],".FNT"))
      if (fseek(InFile,1147,0))
      {
         printf("Error seeking data : %s\n", argv[1]);
         CloseInFile();
         return(0);
      }

   if (fread(Font,4096,1,InFile) != 1)
   {
      printf("Error reading data : %s\n", argv[1]);
      CloseInFile();
      return(0);
   }
   CloseInFile();

   if (argc > 2)
   {
      strupr(argv[2]);

      if (ValidComFileName ( argv[2] ))
         CreateOutFile ( argv[2], 1 );
      else
         CreateOutFile ( argv[2], 0 );
   }
   else
   {
      SetFont ( Font );
   }
   return(0);
}

int FileExists ( char *FileName )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   if ( (OutFile = fopen(FileName,"r+b" )) != NULL )
   {
      CloseOutFile();
      return(1);
   }

   return(0);
}

int ValidComFileName ( char *FileName )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   if (!strstr(FileName,".COM"))
      return(0);
   else
      return(1);
}

int CloseInFile ( void )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   if (fclose(InFile))
   {
      printf("Error closing InFile\n");
      return(1);
   }
   return(0);
}

int CloseOutFile ( void )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   if (fclose(OutFile))
   {
      printf("Error closing OutFile\n");
      return(1);
   }
   return(0);
}

int CreateOutFile ( char *FileName, int COMFile )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   if ( FileExists ( FileName ) )
   {
      printf("File already exists : %s\n", FileName);
      return(0);
   }

   if ( (OutFile = fopen(FileName,"w+b" )) == NULL)
   {
      printf("Error: unable to open OutFile : %s\n", FileName);
      return(0);
   }

   if (COMFile)
      if (fwrite(Hdr,25,1,OutFile) != 1)
      {
         printf("Error writing header : %s\n", FileName);
         CloseOutFile();
         return(0);
      }

   if (fwrite(Font,4096,1,OutFile) != 1)
   {
      printf("Error writing data : %s\n", FileName);
      CloseOutFile();
      return(0);
   }

   if (CloseOutFile())
      return(0);

   printf("Creating %s\n", FileName );
   return(1);
}

void SetFont (char _far *Ptr)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   char _far *Ptr1 = Ptr;
   _asm
   {
      push  bx
      push  cx
      push  dx
      push  es
      push  bp

      les   bp, Ptr1    // ES:BP : pointer to font table
      mov   ah, 0x11    // function    : Character generator
      mov   al, 0x00    // sub function: Load user defined font (text modes)
      mov   bh, 0x10    // bytes per character
      mov   bl, 0x00    // font block to load (0..3)
      mov   cx, 0xFF    // character count
      mov   dx, 0x00    // table character offset
      int   0x10        // interrupt: BIOS Video and screen services

      pop  bp
      pop  es
      pop  dx
      pop  cx
      pop  bx
   }
}
