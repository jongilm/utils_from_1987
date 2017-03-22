
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

#define MAXMOD 220
struct
{
   int data;
   int Const;
   int bss;
   char name[256];
} x[MAXMOD];

FILE        *f;
char         buf[256];
char         name[256];
int          i;
int          j;
char        *psize;
unsigned int size;


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

   for(;;)
   {
      fgets(buf,256,f);
      if (feof(f)) break;
      //printf("%s\n",buf);
      if (strstr(buf,"_DATA")) break;
   }
   if (!strstr(buf,"_DATA"))
   {
      printf("%s\n",buf);
      printf("Cannot find _DATA in file.\n");
      printf("Relink with Microsoft Linker Version 5.30 using the /MAP:FULL option.\n");
      exit(0);
   }
   /* DATA */
   i = 0;
   while (!feof(f))
   {
      fgets(buf,256,f);
      if (feof(f)) break;

      //printf("%5d %s\n",i,buf);

      if (!strstr(buf,"at offset"))
      {
         printf("DATA Done.\n");
         break;
      }
      psize = buf+32;

      if (sscanf(psize,"%XH bytes from %[^\0\r\n]",&size,name) != 2)
      {
         printf("invalid line");
         exit(0);
      }
      x[i].data = size;
      strcpy(x[i].name,name);
      i++;
   }
   //goto print;
   fgets(buf,256,f);

   /* Const */
   while (!feof(f))
   {
      fgets(buf,256,f);
      if (feof(f)) break;
/*
               at offset 00000H 00028H bytes from doddle.lib (\doddle\sources\library\compact.C)
*/
      if (!strstr(buf,"at offset"))
      {
         printf("CONST Done.\n");
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
               x[i].Const = size;
               break;
            }
         }
         else
         {
            strcpy(x[i].name,name);
            x[i].Const = size;
            break;
         }
      }

   }

   fgets(buf,256,f);

   /* BSS */
   while (!feof(f))
   {
      fgets(buf,256,f);
      if (feof(f)) break;
/*
               at offset 00000H 00028H bytes from doddle.lib (\doddle\sources\library\compact.C)
*/
      if (!strstr(buf,"at offset"))
      {
         printf("BSS Done.\n");
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
               x[i].bss = size;
               break;
            }
         }
         else
         {
            strcpy(x[i].name,name);
            x[i].bss = size;
            break;
         }
      }
   }
print:
   fclose(f);
{
   unsigned long int data_total  = 0L;
   unsigned long int const_total = 0L;
   unsigned long int bss_total   = 0L;
   unsigned long int total_total = 0L;

   for (j=0,i=0;i<MAXMOD;i++)
   {
      if (strlen(x[i].name) > j) j = strlen(x[i].name);
   }

   printf("Ö%*.*sÒ%6sÂ%6sÂ%6sÒ%6s·\n",j,j,"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ");
   printf("º%-*.*sº%6s³%6s³%6sº%6sº\n",j,j,"MODULE's DGROUP Usage","DATA","CONST","BSS","TOTAL");
   printf("Ç%*.*s×%6sÅ%6sÅ%6s×%6s¶\n",j,j,"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ");
   for (i=0;i<MAXMOD;i++)
   {
      unsigned long int total;
      if (!x[i].name[0]) break;
      total = x[i].data+x[i].Const+x[i].bss;
      printf("º%-*.*sº%6u³%6u³%6uº%6luº\n",j,j,x[i].name,x[i].data,x[i].Const,x[i].bss,total);
      data_total  += x[i].data;
      const_total += x[i].Const;
      bss_total   += x[i].bss;
      total_total += total;
   }
   printf("Ç%*.*s×%6sÅ%6sÅ%6s×%6s¶\n",j,j,"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ");
   printf("º%-*.*sº%6lu³%6lu³%6luº%6luº\n",j,j,"TOTAL",data_total,const_total,bss_total,total_total);
   printf("Ó%*.*sĞ%6sÁ%6sÁ%6sĞ%6s½\n",j,j,"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ","ÄÄÄÄÄÄ");
}
   return(0);
}

