#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <bios.h>
#include <string.h>

#define LPT1 0

union
{                                        /* Access equiment either as:    */
   unsigned int all;                     /*   unsigned or                 */
   struct                                /*   bit fields                  */
   {
      unsigned int any_diskettes_present : 1;
      unsigned int maths_coprocessor     : 1;
      unsigned int XTmemoryMSB_ATmouse   : 1; /* AT: pointing device installed   */
      unsigned int XTmemoryLSB           : 1; /* XT: 00=16k 01=32k 10=48k 11=64k */
      unsigned int initial_video_mode    : 2; /* 00=reserved, 01=CO40, 10=CO80, 11=MONO */
      unsigned int number_of_diskettes   : 2;
      unsigned int dma_present           : 1;
      unsigned int number_of_RS232_ports : 3;
      unsigned int game_adapter          : 1;
      unsigned int internal_modem        : 1;
      unsigned int number_of_printers    : 2;
   } bits;
} equip;

union REGS        inregs,outregs;
struct diskfree_t drvinfo;
char              y[] = "YES", n[] = "NO ";
char              str[81];
char              video_mode[8];
unsigned int      base_memory;
unsigned int      major_version,minor_version;
unsigned int      drive,drivecount;
int               current_drive;
unsigned long int free_disk_space;
unsigned          pstatus;
unsigned int far  *sports;
unsigned int far  *pports;

int main(void)
{
   FP_SEG(sports) = 0x0000;
   FP_OFF(sports) = 0x0400;
   FP_SEG(pports) = 0x0000;
   FP_OFF(pports) = 0x0408;

   inregs.h.ah = 0x30;
   int86(0x21,&inregs,&outregs);
   major_version = outregs.h.al;
   minor_version = outregs.h.ah;

   int86(0x11,&inregs,&outregs);
   equip.all = outregs.x.ax;
   if      (equip.bits.initial_video_mode == 0) strcpy(video_mode,"EGA/VGA");
   else if (equip.bits.initial_video_mode == 1) strcpy(video_mode,"CO40   ");
   else if (equip.bits.initial_video_mode == 2) strcpy(video_mode,"CO80   ");
   else if (equip.bits.initial_video_mode == 3) strcpy(video_mode,"MONO   ");

   int86(0x12,&inregs,&outregs);
   base_memory = outregs.x.ax;

   _dos_getdrive( &drive );
   current_drive = 'A' + drive - 1;
   _dos_setdrive( drive, &drivecount );
   _dos_getdiskfree( drive, &drvinfo );
   free_disk_space = (long)drvinfo.avail_clusters * drvinfo.sectors_per_cluster * drvinfo.bytes_per_sector;
   _dos_getdrive( &drive );

   /* Fail if any error bit is on, or if either operation bit is off. */
   pstatus = _bios_printer( _PRINTER_STATUS, LPT1, 0 );
   if ((pstatus & 0x29) || !(pstatus & 0x80) || !(pstatus & 0x10)) pstatus = 0;
   else                                                            pstatus = 1;




   printf("           EQUIPMENT REPORTED BY DOS\n\n");

   printf("            DOS Version : %d.%2.2d\n",major_version,minor_version                                            );
   printf("    Base memory (total) : %uk\n"     ,base_memory                                                            );
   printf("        Diskette drives : %d \n"      ,equip.bits.any_diskettes_present ? equip.bits.number_of_diskettes+1 : 0);
   printf("         Logical drives : %d \n"      ,drivecount                                                             );
   printf("          Current drive : %c:\n"     ,current_drive                                                          );
   printf("        Disk free space : %ld\n"     ,free_disk_space                                                        );
   printf("           Serial ports : %d \n"      ,equip.bits.number_of_RS232_ports                                       );
   printf("  Serial port addresses : %4.4X %4.4X %4.4X %4.4X\n",*(sports+0),*(sports+1),*(sports+2),*(sports+3));
   printf("         Internal modem : %s \n"      ,equip.bits.internal_modem ? y:n                                        );
   printf("         Parallel ports : %d \n"      ,equip.bits.number_of_printers                                          );
   printf("Parallel port addresses : %4.4X %4.4X %4.4X %4.4X\n",*(pports+0),*(pports+1),*(pports+2),*(pports+3));
   printf("      Printer available : %s \n"      ,pstatus ? y:n                                                          );
   printf("          Games adapter : %s \n"      ,equip.bits.game_adapter   ? y:n                                        );
   printf("     Initial video mode : %s \n"      ,video_mode                                                             );
   printf("           Co-processor : %s \n"      ,equip.bits.maths_coprocessor ? y:n                                     );
   printf("                    DMA : %s \n"      ,equip.bits.dma_present ? n:y                                           );

   return (0);
}
