#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <conio.h>
#include <string.h>
#include <dos.h>
#include <fcntl.h>
#include <malloc.h>
#include <zip.h>

HEADERFUNCTION HeaderFunction;
ENTRYFUNCTION  EntryFunction;
static char *timestr( unsigned t, char *buf );
static char *datestr( unsigned d, char *buf );

static ULONG TotalCompressedSize = 0L;
static ULONG TotalOriginalSize = 0L;
static ULONG Filecount = 0L;


int main ( int argc, char *argv[] )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   int Erc;

   if (argc != 2)
   {
      printf("usage : ZIPLIST zipfile\n");
      exit(1);
   }

   Erc = ZipList ( argv[1], HeaderFunction, EntryFunction );

   switch (Erc)
   {
      case 0:
         printf("--------------------------------------------------------\n");
         printf("%6lu           %6lu  %3ld%%                       %-lu\n",
                TotalOriginalSize,
                TotalCompressedSize,
                ZipRatio ( TotalOriginalSize, TotalCompressedSize ),
                Filecount);
         break;
      case -1:
         printf("File '%s' not found\n", argv[1] );
         break;
      case -2:
         printf("ZIP file invalid\n" );
         break;
      case -3:
         printf("Out of memory\n" );
         break;
   }

   return(0);
}

void HeaderFunction( char far *Filename, DIRTAIL far *pTail   )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   printf("Searching ZIP:%s\n\n",Filename);
   printf ( "number of files found in archive = %d\n\n", pTail->TotEntries );
   printf("Length  Method   Size    Ratio  Date      Time      Name\n");
   printf("--------------------------------------------------------\n");
}

void EntryFunction ( char far *Filename, DIRENTRY far *pEntry )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   char TempStr[11];
   char date[10];
   char time[10];

   TotalCompressedSize += pEntry->CompressedSize;
   TotalOriginalSize   += pEntry->OriginalSize;
   Filecount++;

   printf("%6lu  %7.7s  %6lu  %3ld%%   %8s  %8s  %s\n",
          pEntry->OriginalSize,
          ZipMethodString ( pEntry->CompressMethod, TempStr ),
          pEntry->CompressedSize,
          ZipRatio ( pEntry->OriginalSize, pEntry->CompressedSize ),
          datestr(pEntry->ModDate,date),
          timestr(pEntry->ModTime,time),
          Filename
          );
}


static char *timestr( unsigned t, char *buf )
/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³ Takes unsigned time in the format:               fedcba9876543210          ³
³ s=2 sec incr, m=0-59, h=23                       hhhhhmmmmmmsssss          ³
³ Changes to a 9-byte string:                      hh:mm:ss                  ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
{
    int hrs  = (t >> 11) & 0x1f;
    int mins = (t >>  5) & 0x3f;
    int secs = t         & 0x1f;

    strcpy( buf,"??:??:??");
    sprintf( buf, "%2.2d:%2.2d:%2.2d",hrs,mins,secs);
    return buf;
}

static char *datestr( unsigned d, char *buf )
/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³ Takes unsigned date in the format:               fedcba9876543210          ³
³ d=1-31, m=1-12, y=0-119 (1980-2099)              yyyyyyymmmmddddd          ³
³ Changes to a 9-byte string:                      yy-mm-dd                  ³
³                                                                            ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
{
    int year  = ((d >> 9) & 0x7f) + 80;
    int month = (d >> 5) & 0x0f;
    int day   = d        & 0x1f;

    strcpy( buf,"??-??-??");
    sprintf( buf, "%02.2d-%02.2d-%02.2d",year ,month, day);
    return buf;
}
