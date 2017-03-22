#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <dos.h>

typedef enum { FALSE, TRUE } BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long ULONG;
typedef void far *FP;

#ifndef MK_FP
#define MK_FP(seg,ofs)  ((FP)(((ULONG)(seg) << 16) | (ofs)))
#endif

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


BOOL belongs(void (_interrupt _far *vec)(), unsigned start, unsigned size);
void display(MCB far *mcb);
char far *env(MCB far *mcb);
void display_vectors(MCB far *mcb);

MCB far *get_mcb(void);
void walk(MCB far *mcb);
char *name_fm_mcb(MCB far *mcb);


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

main(int argc, char *argv[])
{
   printf("WALK - Memory Map Utility (c) 1991 Computer Lab\n\n");

   if (argc < 2)
       walk(get_mcb());            /* walk "normal" MCB chain */
   else
   {
       unsigned seg;
       sscanf(argv[1], "%04X", &seg);
       walk(MK_FP(seg, 0));       /* walk arbitrary MCB chain */
   }

   return 0;
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

   if (_osmajor >= 3)
   {
      inregs.h.ah = 0x62;
      intdos(&inregs,&outregs);
      myself = outregs.x.bx;
   }

   printf("MCB  Owner  Size Env  Name         Cmdline    Hooked vectors\n");
   printf("컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�\n");

   for (;;)
       switch (mcb->type)
       {
           case 'M' : /* Mark : belongs to MCB chain */
               display(mcb);
               mcb = MK_FP(FP_SEG(mcb) + mcb->size + 1, 0);
               break;
           case 'Z' : /* Zbikowski : end of MCB chain */
               display(mcb);
               return;
           default :
               fail("error in MCB chain");
       }
}

void display(MCB far *mcb)
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
      size = mcb->size;
      if (mcb->type == 'Z') size += my_size;

      printf("%04X %04X %6lu ", FP_SEG(mcb), mcb->owner, (long)size << 4);
      printf("%4s "  , env_string);
      printf("%-23s ", name_string);
      /*if (mcb->owner != 0)*/ display_vectors(mcb);

      printf("\n");
   }
   else
      my_size += mcb->size + 1;
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

void display_vectors(MCB far *mcb)
{
    static void (_interrupt _far **vec)() = (void far **) 0;
    int i;
    int count = 0;

    if (! vec)
    {
        if (! ((char*)vec = calloc(256, sizeof(void far *))))
            fail("insufficient memory");
        for (i=0; i<256; i++)
            vec[i] = _dos_getvect(i);
    }

    for (i=0; i<256; i++)
        if (vec[i] && belongs(vec[i], FP_SEG(mcb), mcb->size))
        {
            if (count && (!(count%10))) { printf("\n                                              "); }
            printf("%02X ", i);
            count++;
            vec[i] = 0;
        }
}
