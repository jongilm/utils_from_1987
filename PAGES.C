#include <dos.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
/*#include <time.h>*/

/* Stack and pointer checking off */
#pragma check_stack( off )
#pragma check_pointer( off )
#pragma intrinsic( _enable, _disable )

#define BEEP() _asm { \
                        _asm     sub  bx, bx    /* Page 0          */ \
                        _asm     mov  ax, 0E07h /* TTY bell        */ \
                        _asm     int  10h       /* BIOS 10         */ \
                    }

enum BOOLEAN { FALSE, TRUE };

void (_interrupt _far *oldkbd)( void );
void _interrupt _far newkbd( unsigned _es, unsigned _ds, unsigned _di,
                             unsigned _si, unsigned _bp, unsigned _sp,
                             unsigned _bx, unsigned _dx, unsigned _cx,
                             unsigned _ax, unsigned _ip, unsigned _cs,
                             unsigned _flags );

/* Variables that will be accessed inside TSR must be global. */
int  ftimesup = FALSE, finvideo = FALSE;

/* Huge pointers force compiler to do segment arithmetic for us. */
char _huge *tsrstack;
char _huge *appstack;
char _huge *tsrbottom;

void main( int argc, char **argv )
{
    unsigned tsrsize;

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


    /* Replace existing routines with ours. */
    oldkbd = _dos_getvect(0x16);
    _dos_setvect(0x16,newkbd);

    /* Free the PSP segment and terminate with program resident. */
    _dos_freemem( _psp );
    _dos_keep( 0, tsrsize );
}

void _interrupt _far newkbd( unsigned _es, unsigned _ds, unsigned _di,
                             unsigned _si, unsigned _bp, unsigned _sp,
                             unsigned _bx, unsigned _dx, unsigned _cx,
                             unsigned _ax, unsigned _ip, unsigned _cs,
                             unsigned _flags )
{
    if ((_ax && 0xFF00) != 0x00)
        _chain_intr(oldkbd);
    else
    {
        /* First, execute the original timer interrupt. */
        (*oldkbd)();

        _asm mov _ax, ax
        /* Activate if two conditions are met: First, it's past time for
         * the alarm. Second, we are not in a video interrupt. Checking
         * the video interrupt prevents the rare but potentially
         * dangerous case of calling INT 10 to beep while INT 10 is
         * already running.
         */
        if (_ax == 0x1900)
        {
            /* Save current stack of application, and set old stack of TSR.
             * This is for safety since we don't know the state of the
             * application stack, but we do know the state of our own stack.
             * Turn off interrupts during the stack switch.
             */
            _disable();
            _asm
            {
                mov  WORD PTR appstack[0], sp   ; Save current stack
                mov  WORD PTR appstack[2], ss
                mov  sp, WORD PTR tsrstack[0]   ; Load new stack
                mov  ss, WORD PTR tsrstack[2]
            }
            _enable();

            _asm
            {
                mov ah, 0x0F
                int 0x10
                mov al, 0x00
                cmp bh, 0x01
                jz  pageis1
                mov al, 0x01
      pageis1:  mov ah, 0x05
                int 0x10
            }

            /* Restore application stack. */
            _disable();
            _asm
            {
                mov  sp, WORD PTR appstack[0]
                mov  ss, WORD PTR appstack[2]
            }
            _enable();
        }
    }
}
