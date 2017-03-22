#include <stdlib.h>    /* for NULL              */
#include <dos.h>       /* for int86             */

int   BTRV(int OP,char *POS_BLK,char *DATA_BUF,int *DATA_LEN,char *KEY_BUF,int KEY_NUM);
#define B_STOP      25

void main(void)
{
   BTRV(B_STOP,NULL,NULL,NULL,NULL,0);
}

/*
ÛßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßÛ
Û               Novell's Btrieve Interface starts here                        Û
Û                                                                             Û
ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
*/

/*                                                                      */
/*           MicroSoft C Version 3.0 interface to the                   */
/*              Btrieve Record Manager, version 4                       */
/*                                                                      */
/*      Note: if compiling for the "D" or "L" memory model, remove      */
/*            the comments surrounding the following definition.        */
/*                                                                      */
/*                                  LMODEL means 32-bit pointers in use
#define LMODEL 1
*/


/*         make sure this constant is not commented if
                 compiling for MS windows
#define WINDOWS 1
*/

#define BTR_ERR     20                    /* record manager not started */
#define BTR_INT     0x7B                    /* Btrieve interrupt vector */
#define BTR2_INT    0x2F                 /* multi-user interrupt vector */
#define BTR_VECTOR  BTR_INT * 4                 /* offset for interrupt */
#define BTR_OFFSET  0x33               /* Btrieve offset within segment */
#define VARIABLE_ID 0x6176     /* id for variable length records - 'va' */
#define _2FCODE     0xAB00       /* function code for int 2F to btrieve */

/* ProcId is used for communicating with the Multi Tasking Version of  */
/* Btrieve. It contains the process id returned from BMulti and should */
/* not be changed once it has been set.                                */
/*                                                                     */

static unsigned ProcId = 0;             /* initialize to no process id */
static char MULTI = 0;                /* flag set to true if MultiUser */
static char VSet = 0;           /* flag set to true if checked version */

BTRV (OP, POS_BLK, DATA_BUF, DATA_LEN, KEY_BUF, KEY_NUM)
  int  OP;
  char POS_BLK[];
  char DATA_BUF[];
  int  *DATA_LEN;
  char KEY_BUF[];
  int  KEY_NUM;

{
/*struct REGVAL { int AX, BX, CX, DX, SI, DI, CY; } REGS;*/
/*struct SEGREG { short ES, CS, SS, DS; } sregs;*/
union REGS regs;
struct SREGS sregs;



struct BTRIEVE_PARMS      /* structure passed to Btrieve Record Manager */
 {
   char *BUF_OFFSET;                      /* callers data buffer offset */
#ifndef LMODEL
   int   BUF_SEG;                        /* callers data buffer segment */
#endif
   int   BUF_LEN;                              /* length of data buffer */
   char *CUR_OFFSET;                      /* user position block offset */
#ifndef LMODEL
   int   CUR_SEG;                        /* user position block segment */
#endif
   char *FCB_OFFSET;                              /* offset of disk FCB */
#ifndef LMODEL
   int   FCB_SEG;                                /* segment of disk FCB */
#endif
   int   FUNCTION;                                /* requested function */
   char *KEY_OFFSET;                     /* offset of user's key buffer */
#ifndef LMODEL
   int   KEY_SEG;                       /* segment of user's key buffer */
#endif
   unsigned char  KEY_LENGTH;                     /* length of user's key buffer */
   char  KEY_NUMBER;                    /* key of reference for request */
   int  *STAT_OFFSET;                          /* offset of status word */
#ifndef LMODEL
   int   STAT_SEG;                            /* segment of status word */
#endif
   int   XFACE_ID;                               /* language identifier */
 } XDATA;

int STAT = 0;                                 /* status of Btrieve call */

/*                                                                      */
/*  Check to see that the Btrieve Record Manager has been started.      */
/*                                                                      */

if (!VSet)                   /* if we don't know version of Btrieve yet */
 {
  VSet = 1;
  regs.x.ax = 0x3000;                              /* check dos version */
#ifdef WINDOWS
  int86 (0x21, &regs, &regs);
#else
  int86x (0x21, &regs, &regs, &sregs);
#endif
  if ((regs.x.ax & 0x00FF) >= 3)           /* if DOS version 3 or later */
   {
    regs.x.ax = _2FCODE;
#ifdef WINDOWS
    int86 (BTR2_INT, &regs, &regs);                /* is bmulti loaded? */
#else
    int86x (BTR2_INT, &regs, &regs, &sregs);
#endif
    MULTI = (char)((regs.x.ax & 0xFF) == 'M'); /* if al is M, bmulti is loaded */
   }
 }

if (!MULTI)
 {                                              /* if bmulti not loaded */
  regs.x.ax = 0x3500 + BTR_INT;
#ifdef WINDOWS
  int86 (0x21, &regs, &regs);         /* check for btrieve being loaded */
#else
  int86x (0x21, &regs, &regs, &sregs);
#endif
  if (regs.x.bx != BTR_OFFSET)
     return (BTR_ERR);
 }

/*  Read segment registers and initialize segment part of addresses to  */
/*  user's data segment.                                                */
/*                                                                      */

segread (&sregs);
#ifndef LMODEL
XDATA.BUF_SEG = XDATA.CUR_SEG = XDATA.FCB_SEG =
  XDATA.KEY_SEG = XDATA.STAT_SEG = sregs.ss;
#endif

/*                                                                      */
/*  Move user parameters to XDATA, the block where Btrieve expects them.*/
/*                                                                      */

XDATA.FUNCTION    = OP;
XDATA.STAT_OFFSET = &STAT;
XDATA.FCB_OFFSET  = POS_BLK;
XDATA.CUR_OFFSET  = POS_BLK + 38;
XDATA.BUF_OFFSET  = DATA_BUF;
XDATA.BUF_LEN     = *DATA_LEN;
XDATA.KEY_OFFSET  = KEY_BUF;
XDATA.KEY_LENGTH  = 255;                 /* use max since we don't know */
XDATA.KEY_NUMBER  = (char)KEY_NUM;
XDATA.XFACE_ID    = VARIABLE_ID;

/*                                                                      */
/*  Make call to the Btrieve Record Manager.                            */
/*                                                                      */

regs.x.dx = (int) &XDATA;      /* parameter block is expected to be in DX */
sregs.ds = sregs.ss;
if (!MULTI)
#ifdef WINDOWS                    /* if bmulti not loaded, call Btrieve */
  int86 (BTR_INT, &regs, &regs);
#else
  int86x (BTR_INT, &regs, &regs, &sregs);
#endif
else
 {                                                       /* call bmulti */
  while (1)
   {
    regs.x.ax = 1;                   /*  assume no proc id obtained yet */
    if ((regs.x.bx = ProcId) != 0)              /* if we have a proc id */
      regs.x.ax = 2;                                /* tell bmulti that */
    regs.x.ax += _2FCODE;
#ifdef WINDOWS
    int86 (BTR2_INT, &regs, &regs);                      /* call bmulti */
#else
    int86x (BTR2_INT, &regs, &regs, &sregs);
#endif
    if ((regs.x.ax & 0x00FF) == 0) break;      /* if call was processed */
#ifndef WINDOWS                                /* by bmulti, leave loop */
    regs.x.ax = 0x0200;     /* if multilink advanced is loaded, it will */
    int86x (0x7F, &regs, &regs, &sregs);    /* it will switch processes */
#endif
   }
  if (ProcId == 0) ProcId = regs.x.bx;
 }

*DATA_LEN = XDATA.BUF_LEN;
return (STAT);                                         /* return status */
}

/*
ÛßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßÛ
Û               Novell's Btrieve Interface ends here                          Û
Û                                                                             Û
ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
*/
