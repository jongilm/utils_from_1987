
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

#define MAXMOD 220

#define DATA  0
#define CONST 1
#define BSS   2

struct
{
   int  size[3];
   char name[256];
} x[MAXMOD];

FILE        *f;
char         buf[256];
char         name[256];
int          i;
int          j;
char        *psize;
unsigned int size;

void SkipUntil ( char *str )
{
   for(;;)
   {
      fgets(buf,256,f);
      if (feof(f)) break;
      //printf("%s\n",buf);
      if (strstr(buf,str)) break;
   }
   if (!strstr(buf,str))
   {
      printf("%s\n",buf);
      printf("Cannot find \"%s\" in file.\n",str);
      printf("Relink with Microsoft Linker Version 5.30 using the /MAP:FULL option.\n");
      exit(0);
   }
}

void LoadInto ( int which )
{
   while (!feof(f))
   {
      fgets(buf,256,f);
      if (feof(f)) break;

      if (!strstr(buf,"at offset"))
      {
         printf("%d Done.\n",which);
         break;
      }
      psize = buf+32;

      if (sscanf(psize,"%XH bytes from %[^\0\r\n]",&size,name) != 2)
      {
         printf("invalid line");
         exit(0);
      }

      for (i=0;i<MAXMOD;i++)
      {
         if (x[i].name[0])
         {
            if (!strcmp(x[i].name,name))
            {
               x[i].size[which] = size;
               break;
            }
         }
         else
         {
            strcpy(x[i].name,name);
            x[i].size[which] = size;
            break;
         }
      }
   }
}

void Report ( void )
{
   unsigned long int size_total[4];
   size_total[0] = 0L;
   size_total[1] = 0L;
   size_total[2] = 0L;
   size_total[3] = 0L;

   for (j=0,i=0;i<MAXMOD;i++)
   {
      if (strlen(x[i].name) > j) j = strlen(x[i].name);
   }

   printf("Ö%*.*sÒ%6sÂ%6sÂ%6sÒ%6s·\n",j,j,"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ");
   printf("º%-*.*sº%6s³%6s³%6sº%6sº\n",j,j,"MODULE's DGROUP Usage","DATA","CONST","BSS","TOTAL");
   printf("Ç%*.*s×%6sÅ%6sÅ%6s×%6s¶\n",j,j,"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ");
   for (i=0;i<MAXMOD;i++)
   {
      unsigned long int total;
      if (!x[i].name[0]) break;
      total = x[i].size[0] + x[i].size[1] + x[i].size[2];
      printf("º%-*.*sº%6u³%6u³%6uº%6luº\n",j,j,x[i].name,x[i].size[0],x[i].size[1],x[i].size[2],total);
      size_total[0] += x[i].size[0];
      size_total[1] += x[i].size[1];
      size_total[2] += x[i].size[2];
      size_total[3] += total;
   }
   printf("Ç%*.*s×%6sÅ%6sÅ%6s×%6s¶\n",j,j,"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ");
   printf("º%-*.*sº%6lu³%6lu³%6luº%6luº\n",j,j,"TOTAL",size_total[0],size_total[1],size_total[2],size_total[3]);
   printf("Ó%*.*sĞ%6sÁ%6sÁ%6sĞ%6s½\n",j,j,"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ");
}

int main ( int argc, char *argv[] )
{
   for (i=0;i<MAXMOD;i++)
      memset(&x[i],0,sizeof(x[i]));

   if (argc<2)
   {
      printf("Link with Microsoft Linker Version 5.30 using the /MAP:FULL option.\n");
      printf("Usage: MAPFULL <mapfilename>\n");
      exit(0);
   }

   f=fopen(argv[1],"rt");
   if (!f)
   {
      printf("File not found \"%s\"\n",argv[1]);
      exit(0);
   }

   SkipUntil ( "_DATA                  DATA 16-bit"  );
   LoadInto ( 0 );
   //fgets(buf,256,f);

   SkipUntil ( "CONST                  CONST 16-bit" );
   LoadInto ( 1 );
   //fgets(buf,256,f);

   SkipUntil ( "_BSS                   BSS 16-bit"   );
   LoadInto ( 2 );

   fclose(f);

   Report();

   return(0);
}
