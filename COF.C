#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <io.h>
#include <search.h>
#include <sys\types.h>
#include <sys\utime.h>
#include <sys\stat.h>

/* Prototypes */
void checkerc(void);
int compare(char *filename1, char *filename2);

/* Variables */
int erc = 0;
char errstr[257];
char errmsg[257];

void main(int argc,char *argv[])
/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³                                                                            ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
{
   printf("COF - COmpare File Utility (c) 1991 Computer Lab\n");

   if (argc != 3) erc = 1;      checkerc();
   compare(argv[1], argv[2]);   checkerc();
   erc = -1;                    checkerc();
}

void checkerc(void)
/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³                                                                            ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
{
   switch (erc)
   {
      case -1 : exit(0);
      case  0 : return;
      case  1 : sprintf(errmsg,"SYNTAX: COF <file1> <file2"); break;
      case  2 : sprintf(errmsg,"ERROR %2d: OPEN %s",erc,errstr); break;
      case  3 : sprintf(errmsg,"ERROR %2d: OPEN %s",erc,errstr); break;
      case  4 : sprintf(errmsg,"ERROR %2d: EOF  %s",erc,errstr); break;
      case  5 : sprintf(errmsg,"ERROR %2d: EOF  %s",erc,errstr); break;
      default : sprintf(errmsg,"ERROR %2d: Unknown error code",erc);
   }
   printf("\n%s\n",errmsg);
   exit(1);
}

int compare(char *filename1, char *filename2)
/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³                                                                            ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
{
   FILE *file1;
   FILE *file2;
   int ch1 = 0;
   int ch2 = 0;
   int readfile1 = 1;
   int readfile2 = 1;
   long file1pos = 0L;
   long file2pos = 0L;
   long offset   = 0L;

   file1 = fopen(filename1,"rb");
   if (!file1) {erc = 2; strcpy(errstr,filename1); return(0);}
   file2 = fopen(filename2,"rb");
   if (!file2) {erc = 3; strcpy(errstr,filename2); return(0);}



   while (1)
   {
      if (feof(file1)) readfile1 = 0;
      if (feof(file2)) readfile2 = 0;
      if (readfile1) ch1 = fgetc(file1);
      if (readfile2) ch2 = fgetc(file2);
      if (feof(file1) && feof(file2)) break;
      readfile1 = 0;
      readfile2 = 0;
      if (isspace(ch1)) readfile1 = 1;
      if (isspace(ch2)) readfile2 = 1;
      if (ch1 == ch2)   readfile1 = 1;
      if (ch1 == ch2)   readfile2 = 1;

/*      if (ch1 == ch2)   putchar(ch1);*/
/*      else              putchar('±');*/
      if (readfile1 || readfile2) continue;


      if (!offset)
      {
         file1pos = ftell(file1);
         file2pos = ftell(file2);
         printf("\nDIFFERENCES FOUND\n");
      }

      readfile1 = 1;
   }

   fclose(file2);
   fclose(file1);
   return(1);
}

