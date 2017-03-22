
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

#define EOI     0x20  /* End of interrupt value */

#define IMR1    0x21  /* interrupt mask reg */
#define PIC1    0x20  /* port for EOI */

#define IMR2    0xA1  /* interrupt mask reg */
#define PIC2    0xA0  /* port for EOI */

void printvector(int i);
void PIC1_SendEOI ( void );
int  PIC1_EnabledIRQ ( int IRQ );
void PIC1_EnableIRQ  ( int IRQ );
void PIC1_DisableIRQ ( int IRQ );

void PIC2_SendEOI ( void );
int  PIC2_EnabledIRQ ( int IRQ );
void PIC2_EnableIRQ  ( int IRQ );
void PIC2_DisableIRQ ( int IRQ );

char *intdescrip[] =
{
   "00 Divide by zero",
   "01 Single step",
   "02 Non maskable interrupt (NMI)",
   "03 Breakpoint",
   "04 Overflow",
   "05 Print screen",
   "06 80286 illegal opcode",
   "07 coprocessor",
   "08 8259A IRQ0 System timer (18.2/sec chan0 of 8254)",
   "09 8259A IRQ1 Keyboard data ready",
   "0A 8259A IRQ2 Cascaded from IRQ9 (AT), Vert retrace (VGA/EGA)",
   "0B 8259A IRQ3 UART two, COM2/COM4",
   "0C 8259A IRQ4 UART one, COM1/COM3",
   "0D 8259A IRQ5 Fixed disk (XT), LPT2 ready (AT), reservd (PS/2)",
   "0E 8259A IRQ6 Diskette op. complete (def hndlr F000:EF57)",
   "0F 8259A IRQ7 LPT1 ready (unreliable), 8259 error condition",

   "10 BIOS - Video and screen services",
   "11 BIOS - Read equipment list",
   "12 BIOS - Report memory size",
   "13 BIOS - Disk I/O services",
   "14 BIOS - Serial I/O services",
   "15 BIOS - Cassette and extended services",
   "16 BIOS - Keyboard I/O services",
   "17 BIOS - Printer I/O services",
   "18 BIOS - Basic loader service",
   "19 BIOS - Bootstrap loader service",
   "1A BIOS - System timer and clock services",
   "1B BIOS - Keyboard break",
   "1C BIOS - User timer tick",
   "1D BIOS - video init",
   "1E BIOS - diskette param",
   "1F BIOS - video characters",

   "20 DOS  - Terminate program",
   "21 DOS  - Service calls",
   "22 DOS  - Terminate address",
   "23 DOS  - Break address",
   "24 DOS  - Critical error handler address",
   "25 DOS  - Absolute disk read",
   "26 DOS  - Absolute disk write",
   "27 DOS  - Terminate and stay resident",
   "28 DOS  - scheduler",
   "29 DOS  - fast putchar",
   "2A DOS  - Microsoft network",
   "2B DOS  - reserved",
   "2C DOS  - reserved",
   "2D DOS  - reserved",
   "2E DOS  - execute command",
   "2F DOS  - Multiplex interrupt",

   "30 DOS  - jump",
   "31 DOS  - jump",
   "32 DOS  - reserved",
   "33 DOS  - Microsoft mouse",
   "34 DOS  - reserved",
   "35 DOS  - reserved",
   "36 DOS  - reserved",
   "37 DOS  - reserved",
   "38 DOS  - reserved",
   "39 DOS  - reserved",
   "3A DOS  - reserved",
   "3B DOS  - reserved",
   "3C DOS  - reserved",
   "3D DOS  - reserved",
   "3E DOS  - reserved",
   "3F DOS  - reserved",

   "40 BIOS - diskette",
   "41 BIOS - ptr fixed disk 1",
   "42 BIOS - EGA entry",
   "43 BIOS - ptr EGA init",
   "44 BIOS - ptr EGA graphics",
   "45 BIOS - reserved",
   "46 BIOS - ptr fixed disk 2",
   "47 BIOS - reserved",
   "48 BIOS - PC Jr keyboard",
   "49 BIOS - PC Jr scan code",
   "4A BIOS - timer alarm",
   "4B BIOS - reserved",
   "4C BIOS - reserved",
   "4D BIOS - reserved",
   "4E BIOS - reserved",
   "4F BIOS - reserved",

   "50 BIOS - reserved",
   "51 BIOS - reserved",
   "52 BIOS - reserved",
   "53 BIOS - reserved",
   "54 BIOS - reserved",
   "55 BIOS - reserved",
   "56 BIOS - reserved",
   "57 BIOS - reserved",
   "58 BIOS - reserved",
   "59 BIOS - reserved",
   "5A BIOS - cluster-adapter",
   "5B BIOS - reserved",
   "5C BIOS - network",
   "5D BIOS - reserved",
   "5E BIOS - reserved",
   "5F BIOS - reserved",

   "60 user interrupt",
   "61 user interrupt",
   "62 user interrupt",
   "63 user interrupt",
   "64 user interrupt",
   "65 user interrupt",
   "66 user interrupt",
   "67 extended memory",
   "68 not used",
   "69 not used",
   "6A not used",
   "6B not used",
   "6C system resume",
   "6D not used",
   "6E not used",
   "6F not used",

   "70 8259A IRQ8 CMOS real time clock 1024/sec (many disbaled)",
   "71 8259A IRQ9 Cascade IRQ 9/2 (most invoke INT 0A)",
   "72 8259A IRQ10 Reserved by IBM - not used on any IBM models",
   "73 8259A IRQ11 Reserved by IBM - not used on any IBM models",
   "74 8259A IRQ12 Mouse on PS/2",
   "75 8259A IRQ13 Math coprocessor exception (not on some clones)",
   "76 8259A IRQ14 Hard disk controller - completion of activity",
   "77 8259A IRQ15 Reserved by IBM - not used on any IBM models",
   "78 not used",
   "79 not used",
   "7A not used",
   "7B not used",
   "7C not used",
   "7D not used",
   "7E not used",
   "7F not used",

   "80 BASIC reserved",
   "81 BASIC reserved",
   "82 BASIC reserved",
   "83 BASIC reserved",
   "84 BASIC reserved",
   "85 BASIC reserved",
   "86 BASIC",
   "87 BASIC",
   "88 BASIC",
   "89 BASIC",
   "8a BASIC",
   "8b BASIC",
   "8c BASIC",
   "8d BASIC",
   "8e BASIC",
   "8f BASIC",

   "90 BASIC",
   "91 BASIC",
   "92 BASIC",
   "93 BASIC",
   "94 BASIC",
   "95 BASIC",
   "96 BASIC",
   "97 BASIC",
   "98 BASIC",
   "99 BASIC",
   "9a BASIC",
   "9b BASIC",
   "9c BASIC",
   "9d BASIC",
   "9e BASIC",
   "9f BASIC",

   "a0 BASIC",
   "a1 BASIC",
   "a2 BASIC",
   "a3 BASIC",
   "a4 BASIC",
   "a5 BASIC",
   "a6 BASIC",
   "a7 BASIC",
   "a8 BASIC",
   "a9 BASIC",
   "aa BASIC",
   "ab BASIC",
   "ac BASIC",
   "ad BASIC",
   "ae BASIC",
   "af BASIC",

   "b0 BASIC",
   "b1 BASIC",
   "b2 BASIC",
   "b3 BASIC",
   "b4 BASIC",
   "b5 BASIC",
   "b6 BASIC",
   "b7 BASIC",
   "b8 BASIC",
   "b9 BASIC",
   "ba BASIC",
   "bb BASIC",
   "bc BASIC",
   "bd BASIC",
   "be BASIC",
   "bf BASIC",

   "c0 BASIC",
   "c1 BASIC",
   "c2 BASIC",
   "c3 BASIC",
   "c4 BASIC",
   "c5 BASIC",
   "c6 BASIC",
   "c7 BASIC",
   "c8 BASIC",
   "c9 BASIC",
   "ca BASIC",
   "cb BASIC",
   "cc BASIC",
   "cd BASIC",
   "ce BASIC",
   "cf BASIC",

   "d0 BASIC",
   "d1 BASIC",
   "d2 BASIC",
   "d3 BASIC",
   "d4 BASIC",
   "d5 BASIC",
   "d6 BASIC",
   "d7 BASIC",
   "d8 BASIC",
   "d9 BASIC",
   "da BASIC",
   "db BASIC",
   "dc BASIC",
   "dd BASIC",
   "de BASIC",
   "df BASIC",

   "e0 BASIC",
   "e1 BASIC",
   "e2 BASIC",
   "e3 BASIC",
   "e4 BASIC",
   "e5 BASIC",
   "e6 BASIC",
   "e7 BASIC",
   "e8 BASIC",
   "e9 BASIC",
   "ea BASIC",
   "eb BASIC",
   "ec BASIC",
   "ed BASIC",
   "ee BASIC",
   "ef BASIC",

   "f0 BASIC",
   "f1 not used",
   "f2 not used",
   "f3 not used",
   "f4 not used",
   "f5 not used",
   "f6 not used",
   "f7 not used",
   "f8 not used",
   "f9 not used",
   "fa not used",
   "fb not used",
   "fc not used",
   "fd not used",
   "fe not used",
   "ff not used"
};

int main ( int argc, char *argv[] )
{
   int i;
   int FirstVector = 0;
   int LastVector  = 0xFF;

   if (argc == 2)
   {
      sscanf(argv[1],"%x",&FirstVector);
      LastVector  = FirstVector;
   }
   else if (argc == 3)
   {
      sscanf(argv[1],"%x",&FirstVector);
      sscanf(argv[2],"%x",&LastVector);
   }
   for (i=FirstVector;i<=LastVector;i++) printvector(i);
   printf("\n");

   printf("IMR1 (port 21h) = 0x%2.2X      IRQ : 7 6 5 4 3 2 1 0 (Software int 08-0F)\n",inp(IMR1));
   printf("                         Masked :");
   for (i=7;i>=0;i--) printf(" %c",PIC1_EnabledIRQ(i)?' ':'X');
   printf("\n");
   printf("                        Enabled :");
   for (i=7;i>=0;i--) printf(" %c",PIC1_EnabledIRQ(i)?'X':' ');
   printf("\n");
   printf("\n");
   printf("IMR2 (port A1h) = 0x%2.2X      IRQ : F E D C B A 9 8 (Software int 70-77)\n",inp(IMR2));
   printf("                         Masked :");
   for (i=7;i>=0;i--) printf(" %c",PIC2_EnabledIRQ(i)?' ':'X');
   printf("\n");
   printf("                        Enabled :");
   for (i=7;i>=0;i--) printf(" %c",PIC2_EnabledIRQ(i)?'X':' ');
   printf("\n");

   return(0);
}

void printvector(int i)
{
   printf("%2.2X = %Fp : %s\n",i,_dos_getvect(i),intdescrip[i]);
}

void PIC1_SendEOI ( void )
{
   outp( PIC1, EOI );
}

int  PIC1_EnabledIRQ ( int IRQ )
{
   return((inp(IMR1) & (1<<IRQ)) ? 0:1 );
}

void PIC1_EnableIRQ  ( int IRQ )
{
   outp( IMR1, inp(IMR1) & ~(1<<IRQ) );
}

void PIC1_DisableIRQ ( int IRQ )
{
   outp( IMR1, inp(IMR1) |  (1<<IRQ) );
}

void PIC2_SendEOI ( void )
{
   outp( PIC2, EOI );
}

int  PIC2_EnabledIRQ ( int IRQ )
{
   return((inp(IMR2) & (1<<IRQ)) ? 0:1 );
}

void PIC2_EnableIRQ  ( int IRQ )
{
   outp( IMR2, inp(IMR2) & ~(1<<IRQ) );
}

void PIC2_DisableIRQ ( int IRQ )
{
   outp( IMR2, inp(IMR2) |  (1<<IRQ) );
}
