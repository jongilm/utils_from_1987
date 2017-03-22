#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]);
void prn_INITIALISE(void);
void prn_NLQ(int NLQ);
void prn_ELITE(int ELITE);
void prn_EMPHASIZED(int EMPHASIZED);
void prn_DOUBLESTRIKE(int DOUBLESTRIKE);
void prn_UNIDIRECTIONAL(int UNIDIRECTIONAL);
void prn_FORMFEED(void);
void prn_COMPRESSED(void);

FILE *stream;
FILE *file;

int main(int argc, char *argv[])
{
   int ch;
   char device[129] = "prn";

   if (argc < 2) return(1);
   if (argc == 3) strcpy(device,argv[2]);

   if ((stream = fopen(device,"wb")) != NULL)
   {
      prn_INITIALISE();
      prn_NLQ(1);
      prn_ELITE(1);
      prn_EMPHASIZED(1);
      prn_DOUBLESTRIKE(1);
      prn_UNIDIRECTIONAL(1);
      prn_COMPRESSED();
      fflush(stream);

      if ((file = fopen(argv[1],"rb")) != NULL)
      {
         while (!feof(file))
         {
            if ((ch = fgetc(file)) != EOF) fputc(ch,stream);
         }
      }
      else
      {
         printf("Cannot open file %s\n",argv[1]);
      }

      prn_FORMFEED();
      prn_INITIALISE();
      fflush(stream);
      fclose(stream);
      return(0);
   }
   else
   {
      perror("Printer Doodoo");
      return(1);
   }
}

void prn_INITIALISE(void)
{
   fputc(0x1B,stream);
   fputc(0x40,stream);
}

void prn_COMPRESSED(void)
{
   fputc(0x0F,stream);
}

void prn_NLQ(int NLQ)
{
   fputc(0x1B,stream);
   fputc('x',stream);
   fputc(NLQ ? 1:0,stream);
}

void prn_ELITE(int ELITE)
{
   fputc(0x1B,stream);
   fputc('M',stream);
   fputc(ELITE ? 1:0,stream);
}

void prn_EMPHASIZED(int EMPHASIZED)
{
   fputc(0x1B,stream);
   fputc(EMPHASIZED ? 'E':'F',stream);
}

void prn_DOUBLESTRIKE(int DOUBLESTRIKE)
{
   fputc(0x1B,stream);
   fputc(DOUBLESTRIKE ? 'G':'H',stream);
}

void prn_UNIDIRECTIONAL(int UNIDIRECTIONAL)
{
   fputc(0x1B,stream);
   fputc('U',stream);
   fputc(UNIDIRECTIONAL ? 1:0,stream);
}

void prn_FORMFEED(void)
{
   fputc(0x0C,stream);
}
