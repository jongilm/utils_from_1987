#include <dos.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* Stack and pointer checking off */
#pragma check_stack( off )
#pragma check_pointer( off )
#pragma intrinsic( _enable, _disable )

/* In-line assembler macro to sound a bell. Note that comments in macros must
 * be in the C format, not the assembler format.
 */

/* Prototypes for interrupt functions */
void (_interrupt _far *oldtimer)( void );
void _interrupt _far newtimer( void );
void (_interrupt _far *oldprtscr)( void );
void _interrupt _far newprtscr( void );

/* Variables that will be accessed inside TSR must be global. */
char tempstr[34];
unsigned int i;
unsigned long secs,mins,hours;
int  _far *pvga     = (int  _far *)0xb8000000L;
long _far *pcurtick = (long _far *)0x0000046cL;

/* Huge pointers force compiler to do segment arithmetic for us. */
char _huge *tsrstack;
char _huge *appstack;
char _huge *tsrbottom;

unsigned int mypspseg;
unsigned int myenvseg;

#ifndef MK_FP
#define MK_FP(seg,ofs)  ((void far *)(((unsigned long)(seg) << 16) | (ofs)))
#endif

/* Save current stack of application, and set old stack of TSR. */
#define SAVE_STACK    _disable();                            \
                      _asm                                   \
                      {                                      \
                         _asm mov  WORD PTR appstack[0], sp  \
                         _asm mov  WORD PTR appstack[2], ss  \
                         _asm mov  sp, WORD PTR tsrstack[0]  \
                         _asm mov  ss, WORD PTR tsrstack[2]  \
                      }                                      \
                      _enable();

/* Restore application stack. */
#define RESTORE_STACK _disable();                            \
                      _asm                                   \
                      {                                      \
                          _asm mov  sp, WORD PTR appstack[0] \
                          _asm mov  ss, WORD PTR appstack[2] \
                      }                                      \
                      _enable();


void main(void)
{
   unsigned tsrsize;

   mypspseg = _psp;
   myenvseg = *((unsigned far *)(MK_FP(_psp,0x2c)));

   /* Initialize stack and bottom of program. */
   _asm mov  WORD PTR tsrstack[0], sp
   _asm mov  WORD PTR tsrstack[2], ss
   FP_SEG( tsrbottom ) = _psp;
   FP_OFF( tsrbottom ) = 0;

   /* Program size is:
    *     top of stack
    *   - bottom of program (converted to paragraphs)
    *   + one extra paragraph
    */
   tsrsize = ((tsrstack - tsrbottom) >> 4) + 1;

   /* Replace existing timer and prtscr routines with ours. */
   oldtimer = _dos_getvect( 0x1c );
   _dos_setvect( 0x1c, newtimer );
   oldprtscr = _dos_getvect( 0x05 );
   _dos_setvect( 0x05, newprtscr );

   /* Free the PSP segment and terminate with program resident. */
   _dos_freemem( _psp );
   _dos_keep( 0, tsrsize );
}

void _interrupt _far newtimer()
{
   (*oldtimer)(); /* First, execute the original timer interrupt. */

   SAVE_STACK;

   secs  = (*pcurtick % 1092L) / 18L; /* not quite correct */
   mins  = (*pcurtick / 1092L) % 60L; /* but what the hell */
   hours = *pcurtick / 65520L;

   ltoa(hours,tempstr,10);
   tempstr[2] = ':';
   ltoa(mins,tempstr+3,10);
   tempstr[5] = ':';
   ltoa(secs,tempstr+6,10);

   for (i = 0; i < strlen(tempstr); i++)
      *(pvga+i) = 0x1700 + tempstr[i];

   RESTORE_STACK;
}

void _interrupt _far newprtscr()
{
   SAVE_STACK;

   _dos_setvect(0x1c, oldtimer);
   _dos_setvect(0x05, oldprtscr);
   _dos_freemem(mypspseg);
   _dos_freemem(myenvseg);

   RESTORE_STACK;
}
