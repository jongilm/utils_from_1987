#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <bios.h>
#include <dos.h>
#include <process.h>
#include <ctype.h>

#include "comms.h"

#include <tools.h>


#include "qcom.h"
#include "qcomutl.h"

extern int colour1;
extern int colour2;
extern int colour3;
extern int colour4;
extern int colour5;
extern int colour6;
extern int colour7;
extern int colour8;
extern int colour9;
extern int colour10;
extern int colour11;
extern int colour12;

extern COM *cp;

/***********************************************************/
/**                     UTILITIES                         **/
/***********************************************************/

void shell(void)
{
   char far *scrptr;
   int scrx,scry;

   scrptr = pushscreen();
   scrx   = wherex();
   scry   = wherey();

   clrscr(0x07);
   gotoxy(1,1);
   spawnlp(P_WAIT,getenv("COMSPEC"),getenv("COMSPEC"),NULL);

   gotoxy(scrx,scry);
   popscreen(scrptr);
   title();
}

void ports(void)
{
   unsigned int far *sports;
   unsigned int far *pports;
   char far *scrptr;
   int scrx,scry;
   char str[81];

   scrptr = pushscreen();
   scrx   = wherex();
   scry   = wherey();

   FP_SEG(sports) = 0x0000;
   FP_OFF(sports) = 0x0400;
   FP_SEG(pports) = 0x0000;
   FP_OFF(pports) = 0x0408;

   draw_box();

   _put_text(15+3,wherey(),colour6,"PORT ADDRESSES REPORTED BY DOS"); curs_d(cp);
   curs_d(cp);
   sprintf(str,"     This PC       QCom"                          ); _put_text(15+3,wherey(),colour12                       ,str); curs_d(cp);
   sprintf(str,"COM1:%4.4X         4/03F8"            ,*(sports+0)); _put_text(15+3,wherey(),*(sports+0) ? colour3 : colour6,str); curs_d(cp);
   sprintf(str,"COM2:%4.4X         3/02F8"            ,*(sports+1)); _put_text(15+3,wherey(),*(sports+1) ? colour3 : colour6,str); curs_d(cp);
   sprintf(str,"COM3:%4.4X         4/03E8 (or 2/0338)",*(sports+2)); _put_text(15+3,wherey(),*(sports+2) ? colour3 : colour6,str); curs_d(cp);
   sprintf(str,"COM4:%4.4X         3/02E8 (or 5/0238)",*(sports+3)); _put_text(15+3,wherey(),*(sports+3) ? colour3 : colour6,str); curs_d(cp);
   curs_d(cp);
   sprintf(str,"N.B. Please refer to hardware"          ); _put_text(15+3,wherey(),colour12,str); curs_d(cp);
   sprintf(str,"documentation for specs on COM3 & COM4."); _put_text(15+3,wherey(),colour12,str); curs_d(cp);
   curs_d(cp);
   sprintf(str,"LPT1:%4.4X",*(pports+0)); _put_text(15+3+3,wherey(),*(pports+0) ? colour3 : colour6,str); curs_d(cp);
   sprintf(str,"LPT2:%4.4X",*(pports+1)); _put_text(15+3+3,wherey(),*(pports+1) ? colour3 : colour6,str); curs_d(cp);
   sprintf(str,"LPT3:%4.4X",*(pports+2)); _put_text(15+3+3,wherey(),*(pports+2) ? colour3 : colour6,str); curs_d(cp);
   sprintf(str,"LPT4:%4.4X",*(pports+3)); _put_text(15+3+3,wherey(),*(pports+3) ? colour3 : colour6,str); curs_d(cp);

   _put_text(26,21,colour6,"Press any key to continue");
   gotoxy(1,26);
   fflush(stdin);
   getch();

   gotoxy(scrx,scry);
   popscreen(scrptr);
   title();
}

void equipment_list(void)
{
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
   char far          *scrptr;
   int               scrx,scry;
   char              str[81];
   char              video_mode[8];
   unsigned int      base_memory,extended_memory;
   unsigned int      major_version,minor_version;
   unsigned int      drive,drivecount;
   int               current_drive;
   unsigned long int available_memory,free_disk_space;
   unsigned          pstatus;

   scrptr = pushscreen();
   scrx   = wherex();
   scry   = wherey();

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

   inregs.h.ah = 0x48;
   inregs.x.bx = 0xFFFF;
   int86(0x21,&inregs,&outregs);
   available_memory = (long)outregs.x.bx * 16L;

   inregs.h.ah = 0x88;
   int86(0x15,&inregs,&outregs);
   extended_memory = outregs.x.ax;

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

   draw_box();

   _put_text(15+3,wherey(),colour6,"EQUIPMENT REPORTED BY DOS"); curs_d(cp);
   curs_d(cp);
   sprintf(str,"            DOS Version : %d.%2.2d",major_version,minor_version                                            ); _put_text(15+3,wherey(),colour6,str); curs_d(cp);
   sprintf(str,"    Base memory (total) : %uk"     ,base_memory                                                            ); _put_text(15+3,wherey(),colour6,str); curs_d(cp);
   sprintf(str,"        Diskette drives : %d"      ,equip.bits.any_diskettes_present ? equip.bits.number_of_diskettes+1 : 0); _put_text(15+3,wherey(),colour6,str); curs_d(cp);
   sprintf(str,"         Logical drives : %d"      ,drivecount                                                             ); _put_text(15+3,wherey(),colour6,str); curs_d(cp);
   sprintf(str,"          Current drive : %c:"     ,current_drive                                                          ); _put_text(15+3,wherey(),colour6,str); curs_d(cp);
   sprintf(str,"        Disk free space : %ld"     ,free_disk_space                                                        ); _put_text(15+3,wherey(),colour6,str); curs_d(cp);
   sprintf(str,"           Serial ports : %d"      ,equip.bits.number_of_RS232_ports                                       ); _put_text(15+3,wherey(),colour6,str); curs_d(cp);
   sprintf(str,"         Internal modem : %s"      ,equip.bits.internal_modem ? y:n                                        ); _put_text(15+3,wherey(),colour6,str); curs_d(cp);
   sprintf(str,"         Parallel ports : %d"      ,equip.bits.number_of_printers                                          ); _put_text(15+3,wherey(),colour6,str); curs_d(cp);
   sprintf(str,"      Printer available : %s"      ,pstatus ? y:n                                                          ); _put_text(15+3,wherey(),colour6,str); curs_d(cp);
   sprintf(str,"          Games adapter : %s"      ,equip.bits.game_adapter   ? y:n                                        ); _put_text(15+3,wherey(),colour6,str); curs_d(cp);
   sprintf(str,"     Initial video mode : %s"      ,video_mode                                                             ); _put_text(15+3,wherey(),colour6,str); curs_d(cp);
   sprintf(str,"           Co-processor : %s"      ,equip.bits.maths_coprocessor ? y:n                                     ); _put_text(15+3,wherey(),colour6,str); curs_d(cp);

/* sprintf(str,"                    DMA : %s"      ,equip.bits.dma_present ? n:y                                           ); _put_text(15+3,wherey(),colour6,str); curs_d(cp); */
/* sprintf(str,"Base memory (available) : %u"      ,available_memory                                                       ); _put_text(15+3,wherey(),colour6,str); curs_d(cp); */
/* sprintf(str,"Ext. memory (available) : %uk"     ,extended_memory                                                        ); _put_text(15+3,wherey(),colour6,str); curs_d(cp); */
/* sprintf(str,"              Special 1 : %d"      ,equip.bits.XTmemoryMSB_ATmouse                                         ); _put_text(15+3,wherey(),colour6,str); curs_d(cp); */
/* sprintf(str,"              Special 2 : %d"      ,equip.bits.XTmemoryLSB                                                 ); _put_text(15+3,wherey(),colour6,str); curs_d(cp); */
   curs_d(cp);

   _put_text(26,22,colour6,"Press any key to continue");

   gotoxy(1,26);
   fflush(stdin);
   getch();

   gotoxy(scrx,scry);
   popscreen(scrptr);
   title();
}

/***********************************************************/
/**                        WALK                           **/
/***********************************************************/

typedef enum { F, T } BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long ULONG;
typedef void far *FP;

//#define MK_FP(seg,ofs)  ((FP)(((ULONG)(seg) << 16) | (ofs)))

#pragma pack(1)

typedef struct
{
   BYTE type;          /* 'M'=in chain; 'Z'=at end */
   WORD owner;         /* PSP of the owner */
   WORD size;          /* in 16-byte paragraphs */
   BYTE unused[3];
   BYTE dos4[8];
} MCB;

#define MCB_FM_SEG(seg)     ((seg) - 1)
#define IS_PSP(mcb)         (FP_SEG(mcb) + 1 == (mcb)->owner)
#define ENV_FM_PSP(psp_seg) (*((WORD far *) MK_FP(psp_seg, 0x2c)))


BOOL     belongs(void (_interrupt _far *vec)(), unsigned start, unsigned size);
void     get_details(MCB far *mcb,char *entry);
char far *env(MCB far *mcb);
MCB far  *get_mcb(void);
void     walk(MCB far *mcb);
char     *name_fm_mcb(MCB far *mcb);


char drive[_MAX_DRIVE],
     dir[_MAX_DIR],
     fname[_MAX_FNAME],
     ext[_MAX_EXT],
     full[_MAX_PATH];

char     env_string[7];
char     name_string[129];
char     cmdline_string[129];
WORD     myself  = 0xFFFF;
WORD     my_size = 0;


void memory_map(void)
{
   char far *scrptr;
   int scrx,scry;

   scrptr = pushscreen();
   scrx   = wherex();
   scry   = wherey();

   draw_box();

   _put_text(15+3,wherey(),colour6,"MEMORY MAP"); curs_d(cp);
   curs_d(cp);

   walk(get_mcb());            /* walk "normal" MCB chain */

   _put_text(26,21,colour6,"Press any key to continue");
   gotoxy(1,26);
   fflush(stdin);
   getch();

   gotoxy(scrx,scry);
   popscreen(scrptr);
   title();
}

void fail(char *s)
{
   puts(s);
   exit(1);
}

MCB far *get_mcb(void)
{
   _asm mov ah, 52h
   _asm int 21h
   _asm mov dx, es:[bx-2]
   _asm xor ax, ax
   /* far* returned in DX:AX */
}

mcb_chk(MCB far *mcb)
{
    for (;;)
        if (mcb->type == 'M')
            mcb = MK_FP(FP_SEG(mcb) + mcb->size + 1, 0);
        else
            return (mcb->type == 'Z');
}

void walk(MCB far *mcb)
{
   union REGS inregs,outregs;
   char entry[40];
   int count;

   if (_osmajor >= 3)
   {
      inregs.h.ah = 0x62;
      intdos(&inregs,&outregs);
      myself = outregs.x.bx;
   }
   my_size = 0;


   count = 0;
   for (;;)
   {
      switch (mcb->type)
      {
          case 'M' : /* Mark : belongs to MCB chain */
              get_details(mcb,entry);
              if (entry[0])
              {
                 if (IS_PSP(mcb)) {_put_text(15+3,wherey(),colour3,entry); curs_d(cp);}
                 else             {_put_text(15+3,wherey(),colour6,entry); curs_d(cp);}
                 count++;
              }
              mcb = MK_FP(FP_SEG(mcb) + mcb->size + 1, 0);
              break;
          case 'Z' : /* Zbikowski : end of MCB chain */
              get_details(mcb,entry);
              if (entry[0])
              {
                 if (IS_PSP(mcb)) {_put_text(15+3,wherey(),colour3,entry); curs_d(cp);}
                 else             {_put_text(15+3,wherey(),colour6,entry); curs_d(cp);}
                 count++;
              }
              return;
          default :
              fail("error in MCB chain");
      }
      if (count >= 12)
      {
         _put_text(26,21,colour6,"Press any key for more");
         gotoxy(1,26);
         fflush(stdin);
         getch();
         draw_box();
         _put_text(15+3,wherey(),colour6,"MEMORY MAP"); curs_d(cp);
         curs_d(cp);
         count = 0;
      }
   }
}

void get_details(MCB far *mcb, char *entry)
{
    unsigned env_seg;
    int      cmdline_len;
    char far *cmdline_ptr;
    char far *penv;
    WORD     size;

    strcpy(env_string    ,"");
    strcpy(name_string   ,"");
    strcpy(cmdline_string,"");

/* Environment */
    if (IS_PSP(mcb))
    {
       penv = env(mcb);         /* MSC wants lvalue */
       env_seg = FP_SEG(penv);
       if (env_seg)
          sprintf(env_string,"%04X", env_seg);
       else
          strcpy(env_string,"none");
    }

/* Name */
   strcpy(name_string,name_fm_mcb(mcb));
   if (name_string[0] == '\0')
      if (mcb->owner == 0)
         strcpy(name_string,"(free)");
      else
      {
         strcpy(name_string,name_fm_mcb( MK_FP(mcb->owner-1,0) ));
         if (name_string[0] == '\0')
         {
            strcpy(name_string,"(unknown)");
         }
         else
         {
            strlwr(name_string);
            penv = env( MK_FP(mcb->owner-1,0) );
            env_seg = FP_SEG(penv);
            if (FP_SEG(penv) == FP_SEG(mcb)+1)
               strcat(name_string," environment");
            else
               strcat(name_string," data");
         }
      }

/* Command Line */
   /* presence command line is independent of program name */
   if (IS_PSP(mcb))
   {
      /*
          psp         := MCB owner
          cmdline_len := psp[80h]
          cmdline     := psp[81h]
      */
      cmdline_len = *((BYTE far *) MK_FP(mcb->owner, 0x80));
      cmdline_ptr = MK_FP(mcb->owner, 0x81);

      memcpy(cmdline_string,cmdline_ptr,cmdline_len);
      cmdline_string[cmdline_len] = '\0';
      strcat(name_string," ");
      strcat(name_string,cmdline_string);
   }

   if ((mcb->owner != myself) || (_osmajor < 3))
   {
      size = mcb->size + ((mcb->type == 'Z') ? my_size : 0);
      sprintf(entry,"%6lu %-32.32s", (long)size << 4, name_string);
   }
   else
   {
      my_size += mcb->size + 1;
      entry[0] = '\0';
   }
}

char far *env(MCB far *mcb)
{
    char far *e;
    unsigned env_mcb;
    unsigned env_owner;

    /*
        if the MCB owner is one more than the MCB segment then
            psp := MCB owner
            env_seg := make_far_pointer(psp, 2Ch)
            e := make_far_pointer(env_seg, 0)
        else
            return NULL
    */
    if (IS_PSP(mcb))
        e = MK_FP(ENV_FM_PSP(mcb->owner), 0);
    else
        return (char far *) 0;

    /*
       Does this environment really belong to this PSP? An
       environment is just another memory block, so its MCB is
       located in the preceding paragraph. Make sure the env
       MCB's owner is equal to the PSP whose environment this
       supposedly is!
       This is a good example of the sort of consistency check
       one must do when working with undocumented DOS.
    */
    env_mcb = MCB_FM_SEG(FP_SEG(e));
    env_owner = ((MCB far *) MK_FP(env_mcb, 0))->owner;
    return (env_owner == mcb->owner) ? e : (char far *) 0;
}

BOOL belongs(void (_interrupt _far *vec)(), unsigned start, unsigned size)
{
    unsigned seg = FP_SEG(vec) + (FP_OFF(vec) >> 4); /* normalize */
    return (seg >= start) && (seg <= (start + size));
}

char *name_fm_mcb(MCB far *mcb)
{
   static char name_string[13];
   char far *penv;
   unsigned envvar_len;

   name_string[0] = '\0';

   if (IS_PSP(mcb))
   {
      if (belongs(_dos_getvect(0x2e), FP_SEG(mcb), mcb->size)) /* INT 2Eh belongs to master COMMAND.COM (or other shell) */
      {
         if ( _fullpath( full, getenv("COMSPEC"), _MAX_PATH ) != NULL )
         {
            _splitpath( full, drive, dir, fname, ext );
            strcpy(name_string,fname);
            strcat(name_string,ext);
         }
      }
      else
      {
         /* is there an environment? */
         if (penv = env(mcb))
         {
            /* program name only available in DOS 3+ */
            if (_osmajor >= 3)
            {
               /* skip past environment variables */
               do penv += (envvar_len = strlen(penv)) + 1;
               while (envvar_len);

               /*
                   penv now points to WORD containing number of strings following
                   environment; check for reasonable value: signed because
                   could be FFFFh; will normally be 1
               */
               if ((*((signed far *) penv) >= 1) && (*((signed far *) penv) < 10))
               {
                  penv += sizeof(signed);
                  if (isalpha(*penv))
                  {
                     if ( _fullpath( full, penv, _MAX_PATH ) != NULL )
                     {
                        _splitpath( full, drive, dir, fname, ext );
                        strcpy(name_string,fname);
                        strcat(name_string,ext);
                     }
                  }
               }
            }
         }
      }

      if (name_string[0] == '\0')
      {
         if (_osmajor >= 4)
         {
            memcpy(name_string,mcb->dos4,8);
            cmdline_string[9] = '\0';
         }
      }
   }
   else if (mcb->owner == 8) /* 0008 is not really a PSP; belongs to CONFIG.SYS */
      strcpy(name_string,"config");


   return(name_string);
}
