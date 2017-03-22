#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <bios.h>
#include <dos.h>
#include <process.h>
#include <ctype.h>

#include "comms.h"
#include "commsbuf.h"

#include <tools.h>

#include "qcom.h"
#include "qcomutl.h"

char *sdup[]  = {"FULL","HALF"};
int  dupl     = FULL;
char *vers     = "2.21";
char *versdate = "15 May 1991";
char hstat[9];
char termstr[6];
static char filename[80];
static char paramfile[129];

int port;
int commsmode;
int baud;
int parity;
int databits;
int stopbits;
int duplex;
char emulation[11];
char handshake[11];

int xport;
int xcommsmode;
int xbaud;
int xparity;
int xdatabits;
int xstopbits;
int xduplex;
char xemulation[11];
char xhandshake[11];

int colour1  = 0x07;
int colour2  = 0x1F;
int colour3  = 0x74;
int colour4  = 0x71;
int colour5  = 0x70;
int colour6  = 0x70;
int colour7  = 0x9F;
int colour8  = 0x08;
int colour9  = 0x71;
int colour10 = 0x17;
int colour11 = 0x3F;
int colour12 = 0x7F;

COM *cp;

static char *errstr[] =
{
   "",
   "Bad argument, \'i\' or \'p\' expected.",
   "Device number too large (n>7).",
   "Device is already open",
   "Device not present"
};

int main(int argc, char *argv[])
{
   char far *scrptr;
   int scrx,scry;
   char *envvar;

   printf("QCOM - Quick Communications Version %-4.4s - %s\n",vers,versdate);
   printf("Copyright (c) 1989 Computer Lab.  All rights reserved.\n");

   if (getvdev()==1 || getvgamono())
   {
      colour1  = 0x07;
      colour2  = 0x0F;
      colour3  = 0x70;
      colour4  = 0x70;
      colour7  = 0x8F;
      colour9  = 0x70;
      colour10 = 0x07;
      colour11 = 0x7F;
   }

   port = 1;
   commsmode = 'i';

   envvar = getenv("INIT");
   strcpy(paramfile,(envvar!=NULL) ? envvar:"");
   strcat(paramfile,"\\QCOM.INI");
   load_parameters();

   if (argc > 1)
   {
      port = atoi(argv[1]);

      if (port < 1 || (port > _numports))
      {
         printf("\nInvalid serial port argument (%i)\n\n",port);
         printf("USAGE : QCOM [port]\n");
         printf("        <port> = serial port number (1..%i)\n",_numports);
         printf("   eg : QCOM 1\n");
         exit(1);
      }
   }

   if ((cp = sopen(port-1,(char)commsmode)) == NULL)
   {
      printf("Cannot open com%d:%s\n",port,errstr[comerr]);
      beep ( 1000, 100 );
      exit ( comerr );
   }

   scrptr = pushscreen ();
   scrx   = wherex ();
   scry   = wherey ();

   setscreen ();
   scrcol ( cp, 7, 1 );
   clrscr ( colour1 );
   gotoxy ( 1, 1 );

   modem ( cp, NEWMOD, NULL );
   my_init ();
   set_parameters ();
   setmon ( cp, t_mon );
   term ();
   save_parameters ();

   endscreen();
   gotoxy(scrx,scry);
   popscreen(scrptr);

   if (sclose(cp) != OK) printf("Unable to close com%d\n\n",port);

   return (0);
}

void press_any(void)
{
   int scrx,scry;

   scrx   = wherex();
   scry   = wherey();

   _put_text(26,25,colour7,"Press any key to continue");
   gotoxy(1,26);
   beep(1000,100);
   fflush(stdin);
   getch();

   clrl(25,colour1);
   gotoxy(scrx,scry);
}

void term ( void )
{
   int c;
   int carryon = TRUE;

   panel ( colour1 );

   while ( carryon )
   {
      display_status ();
      a_sgetc ( cp );
      if ( ( c = inkey () ) != NOKEY )
      {
         if (c < 256)
            a_sputc ( cp, (char)c );
         else
            switch (c%256)
            {
               case qF1  : if ( menu () ) carryon = FALSE; break;
               case qF2  : shell ();                       break;
               case qF3  : carryon = FALSE;                break;
               case qAF1 : dtr  ( ON  );                   break;
               case qAF2 : dtr  ( OFF );                   break;
               case qAF3 : dsr  ( ON  );                   break;
               case qAF4 : dsr  ( OFF );                   break;
               case qAF5 : rts  ( ON  );                   break;
               case qAF6 : rts  ( OFF );                   break;
               case qAF7 : cts  ( ON  );                   break;
               case qAF8 : cts  ( OFF );                   break;
               case qCF1 : out1 ( ON  );                   break;
               case qCF2 : out1 ( OFF );                   break;
               case qCF3 : out2 ( ON  );                   break;
               case qCF4 : out2 ( OFF );                   break;
            }
      }
   }
}

void title(void)
{
   char str[101];


   sprintf(str," QCom V%-4.4s    %c",vers,commsmode); _put_text(1,1,colour3,str);
   _put_text(17,1,colour4,"�");
   sprintf(str,"COM%d:%5d,%c,%d,%d",port, getbaud(cp), par_str()[0], getdatalen(cp), getstop(cp)); _put_text(18,1,colour5,str);
   _put_text(34,1,colour4,"�");
   sprintf(str,"%-s", sdup[dupl]); _put_text(35,1,colour5,str);
   _put_text(39,1,colour4,"�");
   sprintf(str,"%-8.8s", hstat);   _put_text(40,1,colour5,str);
   _put_text(48,1,colour4,"�");
   sprintf(str,"%-5.5s", termstr); _put_text(49,1,colour5,str);
   _put_text(54,1,colour4,"�");
   sprintf(str," F1=Menu F2=Shell F3=Quit ");   _put_text(55,1,colour3,str);
}

void panel ( int colour )
{
   clrscr(colour);
   title();
   gotoxy(1,2);
}

typedef enum {LBRK='B',CONFIGU='C',EQUIP='E',FILEXFER='F',MEMMAP='M',PORTS='P'} CHOICES1;

int menu(void)
{
   char c;
   int transtxt(), retval = 0;
   char far *scrptr;
   int scrx,scry;

   static char *mymenu[] =
   {
      "MAIN MENU",
      " ",
      "C. Configuration",
      "B. Transmit break signal",
      "F. File transfer",
      "E. Equipment list",
      "M. Memory map",
      "P. Port Addresses",
      ""
   };

   char **menup;

   scrptr = pushscreen();
   scrx   = wherex();
   scry   = wherey();

   c = TRUE;
   while (TRUE)
   {
      draw_box();
      for (menup = mymenu; **menup != 0; menup++)
      {
         _put_text(15+3,wherey(),colour6,*menup);
         curs_d(cp);
      }

      _put_text(15+3,23-2,colour6,"Enter selection (<ESC> to return) :  ");
      gotoxy(54,23-2);

      fflush(stdout);
      c = (char)getche();
      if (c == 0) continue;
      if (c == ESC) break;
      if (c%256 == qF1) break;
      c = (char)toupper(c);

      switch (c)
      {
         case CONFIGU:
            setup();
            break;
         case LBRK:
            rsbreak(cp);
            break;
         case PORTS:
            ports();
            break;
         case EQUIP:
            equipment_list();
            break;
         case FILEXFER:
            file_transfer();
            break;
         case MEMMAP:
            memory_map();
            break;
         default:
            bad_choice();
            break;
      }
      title();
   }

   gotoxy(scrx,scry);
   popscreen(scrptr);
   title();

   return (retval);
}

void bad_choice(void)
{
   beep(4000,100);
}

typedef enum {UPLOAD='1',DNLOAD,XSEND,XRECV,KSEND,KRECV} CHOICES2;

void file_transfer(void)
{
   char c;
   int transtxt(), retval = 0;
   char far *scrptr;
   int scrx,scry;

   static char *mymenu[] =
   {
      "FILE TRANSFER MENU",
      " ",
      "1. ASCII file upload",
      "2. ASCII file download",
      "3. Xmodem file upload",
      "4. Xmodem file download",
      "5. Kermit file upload",
      "6. Kermit file download",
      ""
   };

   char **menup;

   scrptr = pushscreen();
   scrx   = wherex();
   scry   = wherey();

   c = TRUE;
   while (TRUE)
   {
      draw_box();
      for (menup = mymenu; **menup != 0; menup++)
      {
         _put_text(15+3,wherey(),colour6,*menup);
         curs_d(cp);
      }

      _put_text(15+3,23-2,colour6,"Enter selection (<ESC> to return) :  ");
      gotoxy(54,23-2);

      fflush(stdout);
      c = (char)getche();
      if (c == 0) continue;
      if (c == ESC) break;
      if (c%256 == qF1) break;
      c = (char)toupper(c);


      switch (c)
      {
         case UPLOAD:
            settrans(cp,transtxt);
            panel(colour1);
            transfer(cp,"S",get_fnm("Ascii upload"));
            press_any();
            break;
         case DNLOAD:
            settrans(cp,transtxt);
            panel(colour1);
            transfer(cp,"R",get_fnm("Ascii download"));
            press_any();
            break;
         case XSEND:
            settrans(cp,xmodem);
            panel(colour1);
            transfer(cp,"S",get_fnm("Xmodem upload"));
            press_any();
            break;
         case XRECV:
            settrans(cp,xmodem);
            panel(colour1);
            transfer(cp,"R",get_fnm("Xmodem download"));
            press_any();
            break;
         case KSEND:
            settrans(cp,kermit);
            panel(colour1);
            transfer(cp,"S",get_fnm("Kermit upload"));
            press_any();
            break;
         case KRECV:
            settrans(cp,kermit);
            panel(colour1);
            printf("\nKermit download.\n\n");
            transfer(cp,"R",NULL);
            press_any();
            break;
         default:
            bad_choice();
            break;
      }
      title();
   }

   gotoxy(scrx,scry);
   popscreen(scrptr);
   title();
}

char *get_fnm(char *type)
{
   char *strcat(), prompt[61];
   int last_char;

   strcpy(prompt,"Enter the filename for ");
   strcat(prompt,type);
   strcat(prompt," : ");

   _put_text(2,3,colour2,prompt);
   gotoxy(strlen(prompt)+2,3);
   last_char = edline(filename,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",colour6);
   clreol(strlen(prompt)+2,3,colour1);
   _put_text(strlen(prompt)+2,3,colour2,filename);

   gotoxy(1,4);
   return (filename);
}

void setup(void)
{
   void new_baud(),new_data(),new_stop(),new_par(),new_hand(),new_dup(),new_term();

   int choice;
   char *par_str();
   char far *scrptr;
   static void (*funcp[])(void) =
   {
      new_port,
      new_baud,
      new_data,
      new_par,
      new_stop,
      new_dup,
      new_hand,
      new_term,
      new_mode
   };

   scrptr = pushscreen();

   while (TRUE)
   {
      draw_box();

      _put_text(15+3,wherey(),colour6,"CURRENT CONFIGURATION"); curs_d(cp);
      curs_d(cp);
      _put_text(15+3,wherey(),colour6,"1. Comms port"        ); curs_d(cp);
      _put_text(15+3,wherey(),colour6,"2. Baud rate"         ); curs_d(cp);
      _put_text(15+3,wherey(),colour6,"3. Data bits"         ); curs_d(cp);
      _put_text(15+3,wherey(),colour6,"4. Parity"            ); curs_d(cp);
      _put_text(15+3,wherey(),colour6,"5. Stop bits"         ); curs_d(cp);
      _put_text(15+3,wherey(),colour6,"6. Duplex"            ); curs_d(cp);
      _put_text(15+3,wherey(),colour6,"7. Handshake"         ); curs_d(cp);
      _put_text(15+3,wherey(),colour6,"8. Emulation"         ); curs_d(cp);
      _put_text(15+3,wherey(),colour6,"9. Comms mode"        ); curs_d(cp);

      if ((choice = choose(0,9)) == 0)
      {
         bad_choice();
         continue;
      }
      if (choice == -1) break;
      (*funcp[choice-1])();
   }
   popscreen(scrptr);
   title();
   save_parameters();
}

void new_dup(void)
{
   duplex = !duplex;
   set_parameters();
   title();
}

void new_port(void)
{
   int choice;
   char far *scrptr;

   scrptr = pushscreen();

   while (TRUE) {
           draw_box();

           _put_text(15+3,wherey(),colour6,"SET COMMS PORT"); curs_d(cp);
           curs_d(cp);
           curs_d(cp);
           _put_text(15+3,wherey(),colour6,"1. COM1 - IRQ:4 Port:03F8"); curs_d(cp);
           _put_text(15+3,wherey(),colour6,"2. COM2 - IRQ:3 Port:02F8"); curs_d(cp);
           _put_text(15+3,wherey(),colour6,"3. COM3 - IRQ:4 Port:03E8"); curs_d(cp);
           _put_text(15+3,wherey(),colour6,"4. COM4 - IRQ:3 Port:02E8"); curs_d(cp);

           if ((choice = choose(1,4)) == 0) {
                   bad_choice();
                   continue;
           }
           if (choice != -1)
           {
                   port = choice;
                   set_port();
           }
           break;
   }
   popscreen(scrptr);
   title();
}

void new_mode(void)
{
   static int newmode[] =
           { 'i','p' };
   int choice;
   char far *scrptr;

   scrptr = pushscreen();

   while (TRUE) {
           draw_box();

           _put_text(15+3,wherey(),colour6,"SET COMMS MODE"); curs_d(cp);
           curs_d(cp);
           _put_text(15+3,wherey(),colour6,"1. Interrupt driven"); curs_d(cp);
           _put_text(15+3,wherey(),colour6,"2. Polled");           curs_d(cp);

           if ((choice = choose(1,2)) == 0) {
                   bad_choice();
                   continue;
           }
           if (choice != -1)
           {
                   commsmode = newmode[choice-1];
                   set_port();
           }
           break;
   }
   popscreen(scrptr);
   title();
}

void new_baud(void)
{
        static int newrate[] =
                { 75,150,300,1200,2400,4800,9600,19200 };
        int choice;
        char far *scrptr;

        scrptr = pushscreen();

        while (TRUE) {
                draw_box();

                _put_text(15+3,wherey(),colour6,"SET BAUD RATE"); curs_d(cp);
                curs_d(cp);
                _put_text(15+3,wherey(),colour6,"1.    75 baud"); curs_d(cp);
                _put_text(15+3,wherey(),colour6,"2.   150 baud"); curs_d(cp);
                _put_text(15+3,wherey(),colour6,"3.   300 baud"); curs_d(cp);
                _put_text(15+3,wherey(),colour6,"4.  1200 baud"); curs_d(cp);
                _put_text(15+3,wherey(),colour6,"5.  2400 baud"); curs_d(cp);
                _put_text(15+3,wherey(),colour6,"6.  4800 baud"); curs_d(cp);
                _put_text(15+3,wherey(),colour6,"7.  9600 baud"); curs_d(cp);
                _put_text(15+3,wherey(),colour6,"8. 19200 baud"); curs_d(cp);

                if ((choice = choose(1,8)) == 0) {
                        bad_choice();
                        continue;
                }
                if (choice != -1)
                {
                        baud = newrate[choice-1];
                        set_parameters();
                }
                break;
        }
        popscreen(scrptr);
        title();
}

void new_par(void)
{
        static char newpar[] =
                { 'N','O','E','M','S' };
        int choice;
        char far *scrptr;

        scrptr = pushscreen();

        while (TRUE) {
                draw_box();

                _put_text(15+3,wherey(),colour6,"SET PARITY"); curs_d(cp);
                curs_d(cp);
                _put_text(15+3,wherey(),colour6,"1. No parity");    curs_d(cp);
                _put_text(15+3,wherey(),colour6,"2. Odd Parity");   curs_d(cp);
                _put_text(15+3,wherey(),colour6,"3. Even Parity");  curs_d(cp);
                _put_text(15+3,wherey(),colour6,"4. MARK Parity");  curs_d(cp);
                _put_text(15+3,wherey(),colour6,"5. SPACE Parity"); curs_d(cp);

                if ((choice = choose(1,5)) == 0) {
                        bad_choice();
                        continue;
                }
                if (choice != -1)
                {
                        parity = newpar[choice-1];
                        set_parameters();
                }
                break;
        }
        popscreen(scrptr);
        title();
}

void new_data(void)
{
        int choice;
        char far *scrptr;

        scrptr = pushscreen();

        while (TRUE) {
                draw_box();

                _put_text(15+3,wherey(),colour6,"SET DATA BITS"); curs_d(cp);
                curs_d(cp);
                _put_text(15+3,wherey(),colour6,"Possible settings are:"); curs_d(cp);
                _put_text(15+3,wherey(),colour6,"   5, 6, 7, 8");          curs_d(cp);

                if ((choice = choose(5,8)) == 0) {
                        bad_choice();
                        continue;
                }
                if (choice != -1)
                {
                        databits = choice;
                        set_parameters();
                }
                break;
        }
        popscreen(scrptr);
        title();
}

void new_stop(void)
{
        int choice;
        char far *scrptr;

        scrptr = pushscreen();

        while (TRUE) {
                draw_box();

                _put_text(15+3,wherey(),colour6,"SET STOP BITS"); curs_d(cp);
                curs_d(cp);
                _put_text(15+3,wherey(),colour6,"Possible settings are:"); curs_d(cp);
                _put_text(15+3,wherey(),colour6,"   1 or 2");              curs_d(cp);

                if ((choice = choose(1,2)) == 0) {
                        bad_choice();
                        continue;
                }
                if (choice != -1)
                {
                        stopbits = choice;
                        set_parameters();
                }
                break;
        }
        popscreen(scrptr);
        title();
}

void new_term(void)
{
        void ansi(), vt52(), vt100();

        static void (*newt[])() =
                {
                        dispchar,
                        ansi,
                        vt52,
                        vt100
                };
        int choice;
        char far *scrptr;

        scrptr = pushscreen();

        while (TRUE) {
                draw_box();

                _put_text(15+3,wherey(),colour6,"SET TERMINAL EMULATION"); curs_d(cp);
                curs_d(cp);
                _put_text(15+3,wherey(),colour6,"1. No emulation"); curs_d(cp);
                _put_text(15+3,wherey(),colour6,"2. Ansi-BBS");     curs_d(cp);
                _put_text(15+3,wherey(),colour6,"3. VT52");         curs_d(cp);
                _put_text(15+3,wherey(),colour6,"4. VT100");        curs_d(cp);

                if ((choice = choose(1,4)) == 0) {
                        bad_choice();
                        continue;
                }
                if (choice != -1)
                {
                   switch (choice)
                   {
                      case 1: strcpy(emulation,"NONE"); break;
                      case 2: strcpy(emulation,"ANSI"); break;
                      case 3: strcpy(emulation,"VT52"); break;
                      case 4: strcpy(emulation,"VT100"); break;
                   }
                   set_parameters();
                }
                break;
        }
        popscreen(scrptr);
        title();
}

void new_hand(void)
{
        static char *hmode[] = { "NONE","XON/XOFF","DTR/DSR","RTS/CTS"};
        int choice;
        char far *scrptr;

        scrptr = pushscreen();

        while (TRUE) {
                draw_box();

                _put_text(15+3,wherey(),colour6,"SET HANDSHAKING PROTOCOL"); curs_d(cp);
                curs_d(cp);
                _put_text(15+3,wherey(),colour6,"1. no handshake"); curs_d(cp);
                _put_text(15+3,wherey(),colour6,"2. use XON/XOFF"); curs_d(cp);
                _put_text(15+3,wherey(),colour6,"3. use DTR/DSR");  curs_d(cp);
                _put_text(15+3,wherey(),colour6,"4. use RTS/CTS");  curs_d(cp);

                if ((choice = choose(1,4)) == 0) {
                        bad_choice();
                        continue;
                }
                if (choice != -1)
                {
                        strcpy(handshake,hmode[choice-1]);
                        set_parameters();
                }
                break;
        }
        popscreen(scrptr);
        title();
}

int choose(int low, int high)
{
        int key;

        fflush(stdout);

        _put_text(15+3,23-2,colour6,"Enter selection (<ESC> to return) :  ");
        gotoxy(54,23-2);

        if ((key = getche()) == ESC)
                return (-1);
        key -= 48;
        if (key < low || key > high)
                return (0);
        return (key);
}

char *par_str(void)
{
        static char par[6];

        switch (getparity(cp)) {

                case 'O':       strcpy(par,"ODD");
                                        break;
                case 'E':       strcpy(par,"EVEN");
                                        break;
                case 'M':       strcpy(par,"MARK");
                                        break;
                case 'S':       strcpy(par,"SPACE");
                                        break;
                default:        strcpy(par,"NONE");
        }
        return (par);
}

/* File transfer error message strings */

char Xerr[][80] =
  { "File transfer completed OK.",
    "Transfer cancelled by user.",
    "Unable to open file.",
    "Error reading file.",
    "Error writing file.",
    "Error closing file.",
    "Remote not responding.",
    "Maximum retrys exceeded.",
    "Maximum errors exceeded.",
    "Timeout error.",
    "EOF not aknowledged.",
    "EOF not received from remote.",
    "End of Transfer not completed.",
    "Synchronisation error.",
    "Transfer cancelled by remote.",
    "Initialisation error.",
    "Packet NAK'ed by remote.",
    "Previous packet received",
    "Packet integrity error.",
    "File header error.",
  } ;

void t_mon(int status)           /* example monitor function */
{
        if (status == -1) {
                if (_errno == OK) {
                        if (_sending)
                                printf("Sending Packet no.%u                         \r",_packno);
                        else
                                printf("Packet no.%u Received OK                     \r",_packno);
                } else
                        printf("Error: %s - Total errors = %d\n",Xerr[_errno],_errors);
        } else if (status>0)
                printf("\nFile Transfer Error #%d: %s - Transfer aborted\n",status,Xerr[status]);
        else
                printf("\n%s\n",Xerr[0]);
        fflush(stdout);
}

void draw_box(void)
{
   int x1,y1,x2,y2,border,i;

   x1 = 15;
   y1 = 5;
   x2 = 60;
   y2 = 23;
   border = 1;

   box(x1,y1,x2,y2,colour6,border);
   clrwin(x1+1,y1+1,x2-1,y2-1,colour6);
   for (i=y1+1;i<=y2+1;i++) _put_attr(x2+1,i,2,colour8);
   _put_attr(x1+2,y2+1,x2-x1+1,colour8);

   gotoxy(x1+1,y1+1);

}

void draw_little_box(void)
{
   int x1,y1,x2,y2,border,i;

   x1 = 30;
   y1 = 13;
   x2 = 65;
   y2 = 18;
   border = 1;

   box(x1,y1,x2,y2,colour11,border);
   clrwin(x1+1,y1+1,x2-1,y2-1,colour11);
   for (i=y1+1;i<=y2+1;i++) _put_attr(x2+1,i,2,colour8);
   _put_attr(x1+2,y2+1,x2-x1+1,colour8);

   gotoxy(x1+1,y1+1);

}


void display_status(void)
{
   _put_text(50,2,getdtr(cp)  ? colour9:colour10,"DTR" );
   _put_text(54,2,getdsr(cp)  ? colour9:colour10,"DSR" );
   _put_text(58,2,getrts(cp)  ? colour9:colour10,"RTS" );
   _put_text(62,2,getcts(cp)  ? colour9:colour10,"CTS" );
   _put_text(66,2,getcd(cp)   ? colour9:colour10,"CD" );
   _put_text(69,2,getri(cp)   ? colour9:colour10,"RI"  );
   _put_text(72,2,getout1(cp) ? colour9:colour10,"OUT1");
   _put_text(77,2,getout2(cp) ? colour9:colour10,"OUT2");
}

void dtr(int which)
{
   if (setdtr(cp,which) != OK) beep(1000,100);
}

void dsr(int which)
{
   if (setdsr(cp,which) != OK) beep(1000,100);
}

void cts(int which)
{
   if (setcts(cp,which) != OK) beep(1000,100);
}

void rts(int which)
{
   if (setrts(cp,which) != OK) beep(1000,100);
}

void out1(int which)
{
   if (setout1(cp,which) != OK) beep(1000,100);
}

void out2(int which)
{
   if (setout2(cp,which) != OK) beep(1000,100);
}

void load_parameters(void)
{
   FILE *file;
   char far *scrptr;
   int scrx,scry;

   scrptr = pushscreen();
   scrx   = wherex();
   scry   = wherey();
   draw_little_box();
   _put_text(30+3,wherey(),colour11,"LOADING CONFIGURATION"); curs_d(cp);
   curs_d(cp);
   _put_text(30+3,wherey(),colour11,paramfile); curs_d(cp);
   gotoxy(1,26);

   port     = 1;
   duplex   = FULL;
   baud     = 2400;
   parity   = 'N';
   databits = 8;
   stopbits = 1;
   strcpy(emulation,"NONE");
   strcpy(handshake,"NONE");

   file = fopen(paramfile,"rt");
   if (file != NULL)
   {
      fscanf(file,"port=%d\n"     ,&port);
      fscanf(file,"mode=%c\n"     ,&commsmode);
      fscanf(file,"baud=%d\n"     ,&baud);
      fscanf(file,"parity=%c\n"   ,&parity);
      fscanf(file,"databits=%d\n" ,&databits);
      fscanf(file,"stopbits=%d\n" ,&stopbits);
      fscanf(file,"duplex=%d\n"   ,&duplex);
      fscanf(file,"emulation=%s\n",emulation);
      fscanf(file,"handshake=%s\n",handshake);
      fclose(file);
   }

   xport      = port;
   xcommsmode = commsmode;
   xbaud      = baud;
   xparity    = parity;
   xdatabits  = databits;
   xstopbits  = stopbits;
   xduplex    = duplex;
   strcpy(xemulation,emulation);
   strcpy(xhandshake,handshake);

/*
   printf("port=%d\n"     ,port);
   printf("mode=%c\n"     ,commsmode);
   printf("baud=%d\n"     ,baud);
   printf("parity=%c\n"   ,parity);
   printf("databits=%d\n" ,databits);
   printf("stopbits=%d\n" ,stopbits);
   printf("duplex=%d\n"   ,duplex);
   printf("emulation=%s\n",emulation);
   printf("handshake=%s\n",handshake);
   getch();
*/
   gotoxy(scrx,scry);
   popscreen(scrptr);
}

void save_parameters(void)
{
   FILE *file;
   char far *scrptr;
   int scrx,scry;

   if ( (xport      == port)            &&
        (xcommsmode == commsmode)       &&
        (xbaud      == baud)            &&
        (xparity    == parity)          &&
        (xdatabits  == databits)        &&
        (xstopbits  == stopbits)        &&
        (xduplex    == duplex)          &&
        (!strcmp(xemulation,emulation)) &&
        (!strcmp(xhandshake,handshake))    ) return;

   scrptr = pushscreen();
   scrx   = wherex();
   scry   = wherey();
   draw_little_box();
   _put_text(30+3,wherey(),colour11,"SAVING CONFIGURATION"); curs_d(cp);
   curs_d(cp);
   _put_text(30+3,wherey(),colour11,paramfile); curs_d(cp);
   gotoxy(1,26);

   file = fopen(paramfile,"wt");
   if (file != NULL)
   {
      fprintf(file,"port=%d\n"     ,port);
      fprintf(file,"mode=%c\n"     ,commsmode);
      fprintf(file,"baud=%d\n"     ,baud);
      fprintf(file,"parity=%c\n"   ,parity);
      fprintf(file,"databits=%d\n" ,databits);
      fprintf(file,"stopbits=%d\n" ,stopbits);
      fprintf(file,"duplex=%d\n"   ,duplex);
      fprintf(file,"emulation=%s\n",emulation);
      fprintf(file,"handshake=%s\n",handshake);
      fclose(file);

      xport      = port;
      xcommsmode = commsmode;
      xbaud      = baud;
      xparity    = parity;
      xdatabits  = databits;
      xstopbits  = stopbits;
      xduplex    = duplex;
      strcpy(xemulation,emulation);
      strcpy(xhandshake,handshake);
   }
   gotoxy(scrx,scry);
   popscreen(scrptr);
}

void my_init(void)
{
                                         /* Default       */
   set_form(cp,OUT,CONSOLE   ,RAW   );   /*   COOKED   X  */
   set_form(cp,OUT,CASE      ,CASOFF);   /*   CASOFF      */
   set_form(cp,OUT,EOL       ,EOLOFF);   /*   EOLOFF      */
   set_form(cp,OUT,PASS      ,OFF   );   /*   ON       X  */
   set_form(cp,OUT,TOASCII   ,OFF   );   /*   OFF         */
   set_form(cp,OUT,F_CTRL    ,OFF   );   /*   OFF         */
   set_form(cp,OUT,F_CHAR    ,OFF   );   /*   OFF         */
   set_form(cp,OUT,HANDSHAKE ,0     );   /*   XONXOFF  X  */
   set_form(cp,OUT,DUPLEX    ,FULL  );   /*   FULL        */
   set_form(cp,OUT,ECHO2CRT  ,TRUE  );   /*   FALSE    X  */
   set_form(cp,OUT,CHAR_DELAY,0     );   /*   0           */
   set_form(cp,OUT,LINE_DELAY,0     );   /*   0           */
   set_form(cp,OUT,LINE_CHAR ,LF    );   /*   LF          */
   set_form(cp,OUT,PAUSE_CHAR,'~'   );   /*   ~           */
   set_form(cp,OUT,PAUSE_TIME,500   );   /*   500         */
   set_form(cp,OUT,ABORT_KEY ,27    );   /*   27          */

   set_form(cp,IN ,CONSOLE   ,RAW   );   /*   COOKED   X  */
   set_form(cp,IN ,CASE      ,CASOFF);   /*   CASOFF      */
   set_form(cp,IN ,EOL       ,EOLOFF);   /*   EOLOFF      */
   set_form(cp,IN ,PASS      ,OFF   );   /*   OFF         */
   set_form(cp,IN ,TOASCII   ,OFF   );   /*   OFF         */
   set_form(cp,IN ,F_CTRL    ,OFF   );   /*   OFF         */
   set_form(cp,IN ,F_CHAR    ,OFF   );   /*   OFF         */
   set_form(cp,IN ,HANDSHAKE ,0     );   /*   XONXOFF  X  */
   set_form(cp,IN ,ECHOBACK  ,FALSE );   /*   FALSE       */
   set_form(cp,IN ,ECHO2CRT  ,TRUE  );   /*   TRUE        */
   set_form(cp,IN ,TIME_OUT  ,1     );   /*   0           */
   set_form(cp,IN ,ABORT_KEY ,27    );   /*   27          */

   strcpy(filename,"                                      ");
}

void set_parameters(void)
{
   void ansi(), vt52(), vt100();

   setbaud(cp,baud);
   setparity(cp,(char)parity);
   setdatalen(cp,databits);
   setstop(cp,stopbits);
   set_form(cp,OUT,DUPLEX,duplex);
   dupl   = duplex;

   strcpy(termstr,emulation);
   if      (!strcmp(emulation,"ANSI" )) setterm(cp,ansi);
   else if (!strcmp(emulation,"VT52" )) setterm(cp,vt52);
   else if (!strcmp(emulation,"VT100")) setterm(cp,vt100);
   else { setterm(cp,dispchar); strcpy(termstr,"NONE"); }

   strcpy(hstat,handshake);
   if      (!strcmp(handshake,"XON/XOFF")) set_form(cp,OUT,HANDSHAKE,1);
   else if (!strcmp(handshake,"DTR/DSR" )) set_form(cp,OUT,HANDSHAKE,2);
   else if (!strcmp(handshake,"RTS/CTS" )) set_form(cp,OUT,HANDSHAKE,3);
   else { set_form(cp,OUT,HANDSHAKE,0); strcpy(hstat,"NONE"); }
}

void set_port(void)
{
   if (sclose(cp) != OK) printf("Unable to close comms port\n\n");

   if ((cp = sopen(port-1,(char)commsmode)) == NULL)
   {
      printf("Cannot open com%d:%s\n",port,errstr[comerr]);
      beep(1000,100);
      exit (comerr);
   }
   set_parameters();
}
