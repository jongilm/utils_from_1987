#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

typedef struct
{
   unsigned es, ds, di, si, bp, sp;
   unsigned bx, dx, cx, ax, ip, cs, flags;
} INTERRUPT_REGS;


#define EOI     0x20  /* End of interrupt value */
#define IMR     0x21  /* interrupt mask reg */
#define PIC     0x20  /* port for EOI */

#define DHR     0
#define IER     1
#define IIR     2
#define LCR     3
#define MCR     4
#define LSR     5
#define MSR     6

unsigned int UART_PortAddress = 0x03F8;
unsigned char UART_IRQ   = 4;
unsigned int UART_Interrupt_Counter;

void interrupt far new_int ( INTERRUPT_REGS r )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   r = r;

   _asm
   {
      sti

   again:
      inc UART_Interrupt_Counter

      mov dx,UART_PortAddress
      add dx,LSR
      in al, dx

      mov dx,UART_PortAddress
      add dx,DHR
      in al, dx

      mov dx,UART_PortAddress
      add dx,MSR
      in al, dx

      mov dx,UART_PortAddress
      add dx,IIR
      in al, dx

      cmp al, 00000001b
      jne again

   not_again:
      cli
      mov al, EOI
      out PIC, al
   }
}

unsigned int ResetPort( unsigned char IRQ, unsigned int PortAddress )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   unsigned int i;
   int old_int_seg;
   int old_int_ofs;
   unsigned int SaveCount;

   UART_IRQ               = IRQ;
   UART_PortAddress       = PortAddress;
   UART_Interrupt_Counter = 0;

   _asm
   {
// Set the IER to 0 ie No interrupts enabled
      mov dx,PortAddress
      add dx,IER
      mov al, 0
      out dx, al

// Save the old interrupt handler
      mov al, IRQ
      add al, 8
      mov ah, 0x35
      int 0x21                         // get int vector
      mov old_int_seg, es
      mov old_int_ofs, bx

// Setup our new interrupt handler
      push ds
      mov  al, IRQ
      add  al, 8
      push ax
      mov  ax, cs
      mov  ds, ax
      mov  dx, offset new_int
      pop  ax
      mov  ah, 0x25
      int  0x21                        // set int vector
      pop  ds

// Enable the PIC
      in al,IMR                       // get the current value of the IMR
      mov bl,al                       // save it in bx
      mov al,1                        // create a mask by which to alter the IMR
      mov cl, IRQ
      shl al,cl
      not al                          // eg for IRQ4  11101111
      and al,bl                       // alter the value of the IMR
      mov dx,IMR
      out dx, al                      // Do it.

   }

   for (;;)
   {
      SaveCount = UART_Interrupt_Counter;
      for (i=1;i<65500;i++)
      {
         if ( UART_Interrupt_Counter !=  SaveCount) break;
      }
      if ( UART_Interrupt_Counter ==  SaveCount) break;
   }

   _asm
   {

// Disable the PIC
      in al,IMR                       // get the current value of the IMR
      mov bl,al                       // save it in bx
      mov al,1                        // create a mask by which to alter the IMR
      mov cl,IRQ
      shl al,cl                       // eg for IRQ4  00010000
      or  al,bl                       // alter the value of the IMR
      mov dx,IMR
      out dx, al                      // Do it.

// Replace the old interrupt handler
      push ds
      mov al, IRQ
      add al, 8
      push ax
      mov ds, old_int_seg
      mov dx, old_int_ofs
      pop ax
      mov ah, 0x25
      int 0x21                         // set int vector
      pop  ds
   }
   return(UART_Interrupt_Counter);
}

int main ( int argc, char *argv[] )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   unsigned int IRQ;
   unsigned int PortAddress;
   unsigned int Count;

   printf("ResetCOM:  (c) Softnet Computer Services (Pty) Ltd\n");

   if (argc<2)
   {
      puts ("");
      puts ("Usage : ResetCOM <IRQ> <PortAddress>");
      puts ("     eg ResetCOM 4 3F8");
      puts ("");
      puts ("RESETCOM will clear any pending interrupts on the specified UART port");
      return ( 1 );
   }

   sscanf(argv[1],"%X",&IRQ);
   sscanf(argv[2],"%X",&PortAddress);


   Count = ResetPort ( (unsigned char)IRQ, PortAddress );

   printf("Interrupt(s) processed : %d\n", Count);

   return ( 0 );
}
