
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

#include <tools.h>

#define ERC_OK               0
#define ERC_BANNER_NOMEM     1
#define ERC_BANNER_NOT_FOUND 2

int DoddleTypeBanner ( char *name )
/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*�                                                              �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
{
   FILE *banner_file;
   char *buffer;
   int  x1,y1,x2,y2,bytes,y;
   #pragma pack(1)
   struct
   {
      char filler;
      int  startpos;
      int  endpos;
   } header;
   #pragma pack()

   buffer = calloc(1,2000);
   if (buffer==NULL) return (ERC_BANNER_NOMEM);

   banner_file=fopen(name,"rb");
   if (banner_file==NULL) return (ERC_BANNER_NOT_FOUND);

   fread(&header,5, 1, banner_file);
   x1    = (header.startpos % 80) + 1;
   y1    = (header.startpos / 80) + 1;
   x2    = (header.endpos   % 80) + 1;
   y2    = (header.endpos   / 80) + 1;
   bytes = (x2-x1+1) * (y2-y1+1);

   fread(buffer, 1, bytes, banner_file);
   for (y=y1;y<=y2;y++)
      _put_chars(x1, y, x2-x1+1, buffer+((x2-x1+1)*(y-y1)));

   fread(buffer, 1, bytes, banner_file);
   for (y=y1;y<=y2;y++)
      _put_attrs(x1, y, x2-x1+1, buffer+((x2-x1+1)*(y-y1)));

   fclose(banner_file);

   free(buffer);
   return(ERC_OK);
}

void main ( int argc, char **argv )
{
   if (argc==2)
   {
      switch (DoddleTypeBanner(argv[1]))
      {
         case ERC_BANNER_NOMEM     : puts("Insufficient memory");
         case ERC_BANNER_NOT_FOUND : puts("File not found");
      }
   }
   else
      puts("USAGE : DODSCR <doddle screen file>");
}
