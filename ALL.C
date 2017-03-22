#include <stdlib.h>
#include <stdio.h>
#include <bios.h>
#include <dos.h>
#include <process.h>
#include <direct.h>

long get_disk_size(int disk)
{
   union REGS inregs,outregs;
   struct SREGS segregs;
   inregs.h.ah = 0x1C;
   inregs.h.dl = (unsigned char)disk;
   segread(&segregs);
   int86x(0x21,&inregs,&outregs,&segregs);

   /* sectors_per_cluster = outregs.h.al; */
   /* bytes_per_sectors   = outregs.x.cx; */
   /* clusters_per_disk   = outregs.x.dx; */

   return((long)outregs.x.dx * (long)outregs.h.al * (long)outregs.x.cx);
}

void main(int argc, char *argv[])
{
   unsigned int origdrive;
   char origpath[_MAX_PATH];
   struct diskfree_t drvinfo;
   unsigned drive, drivecount,i;
   long free,total;
   char freestr[50];
   char totalstr[50];

   _dos_getdrive( &drive );
   _dos_setdrive( drive, &drivecount );

   printf( "\n");
   printf( "DRIVE     TOTAL           FREE\n");
   printf( "----------------------------------------\n");
   for (i=3;i<=drivecount;i++)
   {
      if (_dos_getdiskfree( i, &drvinfo ) == 0)
      {
         printf("  %c:  ",'A'+i-1);
         free = (long)drvinfo.avail_clusters *drvinfo.sectors_per_cluster *drvinfo.bytes_per_sector;
         sprintf(freestr,"%9ld",free);
         total = get_disk_size(i);
         sprintf(totalstr,"%9ld",total);
         printf("%3.3s %3.3s %3.3s    ",totalstr,totalstr+3,totalstr+6);
         printf("%3.3s %3.3s %3.3s ",freestr,freestr+3,freestr+6);
         printf("%6.2f%%  ",(1.0-(float)free/(float)total)*100.0);

         if (argc > 1)
         {
            _dos_getdrive(&origdrive);
            _dos_setdrive(i,&drivecount);
            getcwd(origpath,_MAX_PATH);
            chdir("\\");

            printf("   Executing : %s from %c:\\\n",argv[1],'A'+i-1);
            spawnvp(P_WAIT,argv[1],&argv[1]);

            chdir(origpath);
            _dos_setdrive(origdrive,&drivecount);
         }
         printf( "\n");
      }
   }
}
