
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

#include <dodtype.h>
#include <times.h>


char Mask[_MAX_PATH];
SHORT CC;
SHORT YY;
SHORT MM;
SHORT DD;
char sCC[3];
char sYY[3];
char sMM[3];
char sDD[3];

int main ( int argc, char *argv[] )
{
   int i;
   char str[13];

   if (argc != 2)
   {
      printf("MD_DATE - Make a directory using a date mask and change to it.\n");
      printf("Usage: MD_DATE <CaseSensitivePathMask>\n");
      printf("   eg: md_date CEyymmdd.bak            -> mkdir CE961231.bak\n");
      printf("   or: md_date A:\\BACKUPS.cc\\ddmmxx.yy -> mkdir A:\\BACKUPS.19\\3112xx.96\n");
      printf("   Only 'cc','yy','mm' & 'dd' are subsituted.\n");
      printf("   If directory already exists, cd to it and return 0.\n");
      printf("   Return values: 0=successful, 1=param error, 2=md failed, 3=cd failed.\n");
      return 2;
   }

   GetDate ( &YY, &MM, &DD );
   CC = YY/100;
   YY = YY%100;
   sprintf(sCC,"%2.2d", CC );
   sprintf(sYY,"%2.2d", YY );
   sprintf(sMM,"%2.2d", MM );
   sprintf(sDD,"%2.2d", DD );

   strcpy(Mask,argv[1]);

   for (i=0;i<strlen(Mask)-1;i++)
   {
      if (memcmp(&Mask[i],"cc",2)==0) memcpy(&Mask[i],sCC,2);
      if (memcmp(&Mask[i],"yy",2)==0) memcpy(&Mask[i],sYY,2);
      if (memcmp(&Mask[i],"mm",2)==0) memcpy(&Mask[i],sMM,2);
      if (memcmp(&Mask[i],"dd",2)==0) memcpy(&Mask[i],sDD,2);
   }
   if (_chdir( Mask ) == 0)
      return 0;

   if (_mkdir( Mask ) != 0)
   {
      printf ("md \"%s\" failed.\n",Mask);
      return 2;
   }
   if (_chdir( Mask ) != 0)
   {
      printf ("cd \"%s\" failed.\n",Mask);
      return 3;
   }
   //printf ("md & cd \"%s\" successful.\n",Mask);
   return 0;
}
