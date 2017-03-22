typedef unsigned int  WORD;
typedef unsigned long ULONG;

typedef struct DIRTAIL
{
   ULONG Signature;
   WORD ThisDisk;
   WORD DirDisk;
   WORD NumEntries;
   WORD TotEntries;
   ULONG DirSize;
   ULONG DirOffset;
   WORD BannerLength;
} DIRTAIL;

typedef struct DIRENTRY
{
   ULONG Signature;
   WORD CreatorVersion;
   WORD ExtractorVersion;
   WORD GenBits;
   WORD CompressMethod;
   WORD ModTime;
   WORD ModDate;
   ULONG Crc32;
   ULONG CompressedSize;
   ULONG OriginalSize;
   WORD FileNameLen;
   WORD ExtraLen;
   WORD CommentLen;
   WORD DiskNumStart;
   WORD FileAttribs;
   ULONG FileFlags;
   ULONG HeaderPos;
} DIRENTRY;

typedef void HEADERFUNCTION( char far *Filename, DIRTAIL far *pTail   );
typedef void ENTRYFUNCTION ( char far *Filename, DIRENTRY far *pEntry );

extern int ZipList ( char *Filename, HEADERFUNCTION *HeaderFunction, ENTRYFUNCTION *EntryFunction );
extern char *ZipMethodString ( WORD CompressMethod, char *String );
extern ULONG ZipRatio ( ULONG OriginalSize, ULONG CompressedSize );

/*** Function definitions ********************************************

   void HeaderFunction( char far *Filename, DIRTAIL far *pTail   )
   void EntryFunction ( char far *Filename, DIRENTRY far *pEntry )

**********************************************************************/
