#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <conio.h>
#include <string.h>
#include <dos.h>
#include <fcntl.h>
#include <malloc.h>
#include <zip.h>

#ifndef _MSC_VER
#define   _ffree(ptr)     farfree(ptr)
#define   _fmalloc(size)  farmalloc(size)
#endif

#define DIRTAILSIG 0x06054B50

static long GetSigOffset ( int fd, long Signature );
static int FindSig ( char far *buf, int len, long Signature );
static DIRENTRY far *ReadDirectory ( char *Filename, int fd, DIRTAIL *pTail, HEADERFUNCTION *HeaderFunction );
static void DumpFileNames ( DIRENTRY far *pEntry, int numfiles, ENTRYFUNCTION *EntryFunction );


int ZipList ( char *Filename, HEADERFUNCTION *HeaderFunction, ENTRYFUNCTION *EntryFunction )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   int fd;
   DIRTAIL Tail;
   DIRENTRY far *Directory;

   fd = open ( Filename, O_RDONLY | O_BINARY );
   if (fd == -1) return(-1);

   Directory = ReadDirectory ( Filename, fd, &Tail, HeaderFunction );
   if (Tail.DirSize == 0) return(-2);
   if (Directory == NULL) return(-3);

   DumpFileNames ( Directory, Tail.TotEntries, EntryFunction );

   _ffree(Directory);
   close ( fd );

   return(0);
}

char *ZipMethodString ( WORD CompressMethod, char *String )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   switch (CompressMethod)
   {
      case 1: strcpy(String,"Shrunk");  break;
      case 6: strcpy(String,"Implode"); break;
      default: itoa(CompressMethod,String,10);
   }
   return(String);
}

ULONG ZipRatio ( ULONG OriginalSize, ULONG CompressedSize )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   return( 100L - (CompressedSize*100L / OriginalSize ));
}

static DIRENTRY far *ReadDirectory ( char *Filename, int fd, DIRTAIL *pTail, HEADERFUNCTION *HeaderFunction )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   long tailpos;
   WORD bytes;
   WORD nread;
   char far *Directory = NULL;
   char far *TempPtr;
   ULONG TempSize;

   memset ( pTail, 0, sizeof(DIRTAIL) );

   tailpos = GetSigOffset ( fd, DIRTAILSIG );
   if ( tailpos >= 0 )
   {
      lseek ( fd, tailpos, 0 );
      read ( fd, pTail, sizeof(DIRTAIL) );

      (*HeaderFunction)(Filename, pTail);

      lseek ( fd, pTail->DirOffset, 0 );

      Directory = (char far *) _fmalloc((WORD)pTail->DirSize);
      if (Directory == NULL) return(NULL);

      TempSize = pTail->DirSize;
      TempPtr  = Directory;
      while ( TempSize > 0 )
      {
         bytes = ( TempSize > 65535L ) ? (WORD)65535 : (WORD)TempSize;
         _dos_read ( fd, TempPtr, bytes, &nread );
         TempSize -= bytes;
         TempPtr += bytes;
      }
   }
   return ( (DIRENTRY far *)Directory );
}

static long GetSigOffset ( int fd, long Signature )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   int bp;
   int bytesread;
   long fs;
   long pos;
   char buf[512];

   fs = filelength ( fd );
   pos = ( fs <= sizeof(buf)) ? 0L : fs - (long)sizeof(buf);
   lseek ( fd, pos, 0 );

   read ( fd, buf, sizeof(buf) );
   bp = FindSig ( buf, sizeof(buf), Signature );

   while ( bp < 0 && pos > 0 )
   {
      memcpy ( &buf[sizeof(buf) - 4],buf, 4 );
      pos -= sizeof(buf) - 4;
      if (pos < 0) pos = 0;
      lseek ( fd, pos, 0 );
      bytesread = read ( fd, buf, sizeof(buf) - 4 );
      if ( bytesread < sizeof(buf) - 4 )
         memcpy ( &buf[bytesread], &buf[sizeof(buf) - 4], 4 );

      if ( bytesread > 0 )
      {
         bytesread += 4;
         bp = FindSig ( buf, bytesread, Signature );
      }
   }
   if (bp < 0) return ( -1 );
   else        return ( pos + bp );
}

static int FindSig ( char far *buf, int len, long Signature )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   int pos = -1;

   _asm cld
   _asm les di,buf
   _asm mov cx,len
   _asm shr cx,1
   _asm jcxz L3
L1:
   _asm mov ax, word ptr Signature
   _asm repne scasw
   _asm jcxz L3
   _asm mov ax, word ptr Signature + 2
   _asm repne scasw
   _asm je L2
   _asm inc cx
   _asm sub di,2
   _asm jmp L1
L2:
   _asm shl cx,1
   _asm neg cx
   _asm add cx,len
   _asm sub cx,4
   _asm mov pos,cx
L3:
   return ( pos );
}

static void DumpFileNames ( DIRENTRY far *pEntry, int numfiles, ENTRYFUNCTION *EntryFunction )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   char Filename[256];
   WORD delta;

   while ( numfiles-- )
   {
      _fmemcpy(Filename, pEntry+1, pEntry->FileNameLen);
      Filename [pEntry->FileNameLen] = '\0';

      (*EntryFunction)(Filename, pEntry);

      delta = pEntry->FileNameLen + pEntry->ExtraLen + pEntry->CommentLen + sizeof(DIRENTRY);
      pEntry = (DIRENTRY far *)(delta + (char far *)pEntry );
   }
}
