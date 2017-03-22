#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <tools.h>

int option;
int opt;
int set[256];
int i;
int nnn;
unsigned int screentype;
int main(void);
int fonts(void);
int actions(void);

main()
/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³                                      ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
{
   do
   {
      option = fonts();
      if (option != 27) option = actions();
   } while (option != 27);
   return(0);
}

fonts()
{
   for (i=0;i<256;i++) set[i] = 0;
   clrscr(0,0,24,80,0);
/* lowvideo();*/
   gotoxy( 3, 1); cprintf("A - Elite                          ");
   gotoxy( 3, 2); cprintf("B - NLQ                            ");
   gotoxy( 3, 3); cprintf("C - Proportional spacing           ");
   gotoxy( 3, 4); cprintf("D - Enlarged                       ");
   gotoxy( 3, 5); cprintf("E - Enlarged for one line          ");
   gotoxy( 3, 6); cprintf("F - Condensed (DRAFT)              ");
   gotoxy( 3, 7); cprintf("G - Superscript                    ");
   gotoxy( 3, 8); cprintf("H - Subscript                      ");
   gotoxy( 3, 9); cprintf("I - Emphasized                     ");
   gotoxy( 3,10); cprintf("J - Double strike (DRAFT)          ");
   gotoxy( 3,11); cprintf("K - Italic (TECO DRAFT)            ");
   gotoxy( 3,12); cprintf("L - Underline                      ");
   gotoxy( 3,13); cprintf("M - Print ASCII 128-159 (TECO)     ");
   gotoxy( 3,14); cprintf("N - World Trade character set (IBM)");
   gotoxy( 3,15); cprintf("O - Print control codes            ");
   gotoxy( 3,16); cprintf("P - Paper end detecter             ");
   gotoxy( 3,17); cprintf("Q - Select printer (On Line)       ");
   gotoxy( 3,18); cprintf("R - Set MSB to 1                   ");
   gotoxy( 3,19); cprintf("S - Set MSB to 0                   ");
   gotoxy( 3,20); cprintf("T - Automatic line feed (IBM)      ");
   gotoxy( 3,21); cprintf("U - Unidirectional                 ");
   gotoxy( 3,22); cprintf("V - Half speed                     ");
   gotoxy( 3,23); cprintf("W - Incremental and view           ");
/* highvideo();*/
   gotoxy( 3,24);
   cprintf("Enter option (<esc>:exit 0:swap <space>:Test) ===>  ");
/* lowvideo();*/
   do
   {
      gotoxy(55,24);
      printf(" ");
      option = getche();

      switch (option)
      {
         case 'a': if (!set[ 1]) fprintf(stdprn,"%c%c"  , 27, 77    );
                   else          fprintf(stdprn,"%c%c"  , 27, 80    ); break;
         case 'b': if (!set[ 2]) fprintf(stdprn,"%c%c%c", 27,120,  1);
                   else          fprintf(stdprn,"%c%c%c", 27,120,  0); break;
         case 'c': if (!set[ 3]) fprintf(stdprn,"%c%c%c", 27,112,  1);
                   else          fprintf(stdprn,"%c%c%c", 27,112,  0); break;
         case 'd': if (!set[ 4]) fprintf(stdprn,"%c%c%c", 27, 87,  1);
                   else          fprintf(stdprn,"%c%c%c", 27, 87,  0); break;
         case 'e': if (!set[ 5]) fprintf(stdprn,"%c"    , 14        );
                   else          fprintf(stdprn,"%c"    , 20        ); break;
         case 'f': if (!set[ 6]) fprintf(stdprn,"%c"    , 15        );
                   else          fprintf(stdprn,"%c"    , 18        ); break;
         case 'g': if (!set[ 7]) fprintf(stdprn,"%c%c%c", 27, 83,  1);
                   else          fprintf(stdprn,"%c%c"  , 27, 84    ); break;
         case 'h': if (!set[ 8]) fprintf(stdprn,"%c%c%c", 27, 83,  0);
                   else          fprintf(stdprn,"%c%c"  , 27, 84    ); break;
         case 'i': if (!set[ 9]) fprintf(stdprn,"%c%c"  , 27, 69    );
                   else          fprintf(stdprn,"%c%c"  , 27, 70    ); break;
         case 'j': if (!set[10]) fprintf(stdprn,"%c%c"  , 27, 71    );
                   else          fprintf(stdprn,"%c%c"  , 27, 72    ); break;
         case 'k': if (!set[11]) fprintf(stdprn,"%c%c"  , 27, 52    );
                   else          fprintf(stdprn,"%c%c"  , 27, 53    ); break;
         case 'l': if (!set[12]) fprintf(stdprn,"%c%c%c", 27, 45,  1);
                   else          fprintf(stdprn,"%c%c%c", 27, 45,  0); break;
         case 'm': if (!set[13]) fprintf(stdprn,"%c%c"  , 27, 54    );
                   else          fprintf(stdprn,"%c%c"  , 27, 55    ); break;
         case 'n': if (!set[14]) fprintf(stdprn,"%c%c"  , 27, 54    );
                   else          fprintf(stdprn,"%c%c"  , 27, 55    ); break;
         case 'o': if (!set[15]) fprintf(stdprn,"%c%c%c", 27, 73,  1);
                   else          fprintf(stdprn,"%c%c%c", 27, 73,  0); break;
         case 'p': if (!set[16]) fprintf(stdprn,"%c%c"  , 27, 57    );
                   else          fprintf(stdprn,"%c%c"  , 27, 56    ); break;
         case 'q': if (!set[17]) fprintf(stdprn,"%c"    , 17        );
                   else          fprintf(stdprn,"%c"    , 19        ); break;
         case 'r': if (!set[18]) fprintf(stdprn,"%c%c"  , 27, 62    );
                   else          fprintf(stdprn,"%c%c"  , 27, 35    ); break;
         case 's': if (!set[19]) fprintf(stdprn,"%c%c"  , 27, 61    );
                   else          fprintf(stdprn,"%c%c"  , 27, 35    ); break;
         case 't': if (!set[20]) fprintf(stdprn,"%c%c%c", 27, 53,  1);
                   else          fprintf(stdprn,"%c%c%c", 27, 53,  0); break;
         case 'u': if (!set[21]) fprintf(stdprn,"%c%c%c", 27, 85,  1);
                   else          fprintf(stdprn,"%c%c%c", 27, 85,  0); break;
         case 'v': if (!set[22]) fprintf(stdprn,"%c%c%c", 27,115,  1);
                   else          fprintf(stdprn,"%c%c%c", 27,115,  0); break;
         case 'w': if (!set[23]) fprintf(stdprn,"%c%c%c", 27,105,  1);
                   else          fprintf(stdprn,"%c%c%c", 27,105,  0); break;
         case ' ': fprintf(stdprn,"This is a Test\r\n"); break;
      }

      opt = option - 96;

      if ((option >= 'a') && (option <= 'w'))
      {
         gotoxy(1,opt);
         if (set[opt])
         {
            set[opt]=0;
            cprintf(" ");
         }
         else
         {
            set[opt]=1;
            cprintf("Û");
         }
      }

   } while ((option != 27) && (option != '0'));
   return(option);
}

actions()
{
   nnn = 1;
   clrscr(0,0,24,80,0);
/* lowvideo();*/
   gotoxy( 3, 1); cprintf("A - Initialize                            ");
   gotoxy( 3, 2); cprintf("B - Dummy command                         ");
   gotoxy( 3, 3); cprintf("C - Bell                                  ");
   gotoxy( 3, 4); cprintf("D - Back space                            ");
   gotoxy( 3, 5); cprintf("E - Horizontal tab                        ");
   gotoxy( 3, 6); cprintf("F - Line feed                             ");
   gotoxy( 3, 7); cprintf("G - Execute nnn/216 inch line feed        ");
   gotoxy( 3, 8); cprintf("H - Execute nnn/216 inch reverse line feed");
   gotoxy( 3, 9); cprintf("I - Vertical tab                          ");
   gotoxy( 3,10); cprintf("J - Form feed                             ");
   gotoxy( 3,11); cprintf("K - Carriage return                       ");
   gotoxy( 3,12); cprintf("L - Cancel line                           ");
   gotoxy( 3,13); cprintf("M - Delete character                      ");
/* highvideo();*/
   gotoxy( 3,24);
   cprintf("Enter option (<esc>:exit 0:swap <space>:Test) ===>  ");
/* lowvideo();*/
   do
   {
      gotoxy(55,24);
      option = getche();

      switch (option)
      {
         case 'a': fprintf(stdprn,"%c%c"  , 27, 64    ); break;
         case 'b': fprintf(stdprn,"%c%c%c", 27, 25,nnn); break;
         case 'c': fprintf(stdprn,"%c"    ,  7        ); break;
         case 'd': fprintf(stdprn,"%c"    ,  8        ); break;
         case 'e': fprintf(stdprn,"%c"    ,  9        ); break;
         case 'f': fprintf(stdprn,"%c"    , 10        ); break;
         case 'g': fprintf(stdprn,"%c%c%c", 27, 74,nnn); break;
         case 'h': fprintf(stdprn,"%c%c%c", 27,106,nnn); break;
         case 'i': fprintf(stdprn,"%c"    , 11        ); break;
         case 'j': fprintf(stdprn,"%c"    , 12        ); break;
         case 'k': fprintf(stdprn,"%c"    , 13        ); break;
         case 'l': fprintf(stdprn,"%c"    , 24        ); break;
         case 'm': fprintf(stdprn,"%c"    ,127        ); break;
         case ' ': fprintf(stdprn,"This is a Test"); break;
      }

   } while ((option != 27) && (option != '0'));
   return(option);
}
/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³Description                                 ³                         ³                                              ³
ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
³Initialize                                  ³ 027 064                 ³                                              ³
³Dummy command                               ³ 027 025 nnn             ³                                              ³
³Bell                                        ³ 007                     ³                                              ³
³Back space                                  ³ 008                     ³                                              ³
³Horizontal tab                              ³ 009                     ³                                              ³
³Line feed                                   ³ 010                     ³                                              ³
³Execute nnn/216 inch line feed              ³ 027 074 nnn             ³                                              ³
³Execute nnn/216 inch reverse line feed      ³ 027 106 nnn             ³ 0<=nnn<=255                                  ³
³Vertical tab                                ³ 011                     ³                                              ³
³Form feed                                   ³ 012                     ³                                              ³
³Carriage return                             ³ 013                     ³                                              ³
³Cancel line                                 ³ 024                     ³                                              ³
³Delete character                            ³ 127                     ³                                              ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³Description                                 ³                         ³                                              ³
ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
³                                            ³                         ³                                              ³
³Select print mode                           ³ 027 033 nnn             ³ 0<=nnn<=255                                  ³
³Select international character set          ³ 027 082 nnn             ³ 1<=nnn<=10                                   ³
³Justification left                          ³ 027 097 000             ³                                              ³
³Justification centre                        ³ 027 097 001             ³                                              ³
³Justification right                         ³ 027 097 002             ³                                              ³
³Justification full                          ³ 027 097 003             ³                                              ³
³                                            ³                         ³                                              ³
³Set form length in lines                    ³ 027 067 nnn             ³ 1<=nnn<=127                                  ³
³Set form length in inches                   ³ 027 067 nnn 000         ³ 1<=nnn<=127                                  ³
³Set skip perforation length                 ³ 027 078 nnn             ³ 1<=nnn<=127                                  ³
³Cancel skip perforation                     ³ 027 079                 ³                                              ³
³                                            ³                         ³                                              ³
³Set inter-character space                   ³ 027 032 nnn             ³ 0<=nnn<=127                                  ³
³                                            ³                         ³                                              ³
³Select 7/72  inch line spacing = 21/216     ³ 027 049                 ³                                              ³
³Select 1/8   inch line spacing = 27/216     ³ 027 048                 ³                                              ³
³Select 1/6   inch line spacing = 36/216     ³ 027 050                 ³                                              ³
³Select n/72  inch line spacing = 3n/216     ³ 027 065 nnn             ³ 0<=nnn<=85 for (IBM) must send 027 050 first ³
³Select n/216 inch line spacing =  n/216     ³ 027 051 nnn             ³                                              ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³Description                                 ³ On             ³ Off            ³
ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
³Elite (off=Pica)                            ³ 027 077        ³ 027 080        ³
³NLQ                                         ³ 027 120 001    ³ 027 120 000    ³
³Proportional spacing                        ³ 027 112 001    ³ 027 112 000    ³
³Enlarged                                    ³ 027 087 001    ³ 027 087 000    ³
³Enlarged for one line                       ³ 014 or 027 014 ³ 020            ³
³Condensed                            (DRAFT)³ 015 or 027 015 ³ 018            ³
³Superscript                                 ³ 027 083 001    ³ 027 084        ³
³Subscript                                   ³ 027 083 000    ³ 027 084        ³
³Emphasized                                  ³ 027 069        ³ 027 070        ³
³Double strike                        (DRAFT)³ 027 071        ³ 027 072        ³
³Italic                          (TECO DRAFT)³ 027 052        ³ 027 053        ³
³Underline                                   ³ 027 045 001    ³ 027 045 000    ³
³Enable printing of ASCII 128-159      (TECO)³ 027 054        ³ 027 055        ³
³World Trade character set off=normal   (IBM)³ 027 054        ³ 027 055        ³
³Enable printing of control codes            ³ 027 073 001    ³ 027 073 000    ³
³                                            ³                ³                ³
³Paper end detecter                          ³ 027 057        ³ 027 056        ³
³Select printer (On Line)                    ³ 017            ³ 019            ³
³Set MSB to 1                                ³ 027 062        ³ 027 035        ³
³Set MSB to 0                                ³ 027 061        ³ 027 035        ³
³Automatic line feed                    (IBM)³ 027 053 001    ³ 027 053 000    ³
³Unidirectional                              ³ 027 085 001    ³ 027 085 000    ³
³Half speed                                  ³ 027 115 001    ³ 027 115 000    ³
³Incremental and view                        ³ 027 105 001    ³ 027 105 000    ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
/*
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Graphics
027 060                      Return print head to home
027 036 aaa bbb              Set absolute dot position 0<=aaa<=255 0<=bbb<=255 (aaa+bbb*256)
027 042 mmm aaa bbb ..data.. 8 bit image mode selection 0<=mmm<=6 0<=aaa,bbb<=255
027 063 nnn mmm              Re-assign 8-pin image commands nnn=0075,  76, 89, 90 0<=mmm<=6
027 075 aaa bbb ..data..     Set single density bit image mode
027 076 aaa bbb ..data..     Set double density bit image mode
027 089 aaa bbb ..data..     Set high speed double density bit image mode
027 090 aaa bbb ..data..     Set quadruple density bit image mode
027 092 mmm aaa bbb ..data.. 9 bit image mode selection mmm=000:single mmm=001:double 0<=aaa,bbb<=255


Character download
027 037 nnn 000                  Turn download CG on/off nnn=000:ROM nnn=001:download
027 038 000 nnn mmm aaa pp1..ppn Download character definition 0<=nnn,mmm,aaa,ppp<=255
027 058 000 000 000              Copy ROM CG to download CG

Tabs and Margins
027 047 nnn                  Select vertical tab channel  0<=nnn<=7
027 066 n00 n01..n015 000    Set vertical tabs            1<=nnn<=255
027 068 n00 n01..n031        Set horizontal tabs
027 098 ccc n00 n01..n15 000 Set vertical tabs in channel 0<=ccc<=7 0<=nnn<=255
027 082                      Restore Default tab setting
027 081 nnn                  Set right margin             1<=nnn<=255
027 108 nnn                  Set left margin              0<=nnn<=255
027 088 aaa bbb              Set left and right margins   0<=aaa,bbb<=255
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
*/
/*
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
007                              Bell
008                              Back space
009                              Horizontal tab
010                              Line feed
011                              Vertical tab
012                              Form feed
013                              Carriage return
014                              Select enlarged printing for one line
015                              Select condensed printing
017                              Select printer
018                              Cancel condensed printing (DRAFT)
019                              Deselect printer
020                              Cancel enlarged printing
024                              Cancel line
127                              Delete character
027 014                          Select enlarged printing for one line
027 015                          Select condensed printing (DRAFT)
027 025 nnn                      Dummy command
027 032 nnn                      Set inter-character space 0<=nnn<=127
027 033 nnn                      Print mode selection 0<=nnn<=255
027 035                          Cancel MSB control
027 036 aaa bbb                  Set absolute dot position 0<=aaa<=255 0<=bbb<=255 (aaa+bbb*256)
027 037 nnn 000                  Turn download CG on/off nnn=000:ROM nnn=001:download
027 038 000 nnn mmm aaa pp1..ppn Download character definition 0<=nnn,mmm,aaa,ppp<=255
027 042 mmm aaa bbb ..data..     8 bit image mode selection 0<=mmm<=6 0<=aaa,bbb<=255
027 045 nnn                      Turn underlining on/off nnn=000:off nnn=001:on
027 047 nnn                      Select vertical tab channel 0<=nnn<=7
027 048                          Select 1/8   inch line spacing
027 049                          Select 7/72  inch line spacing
027 050                          Select 1/6   inch line spacing (TECO)
027 050                          Select 1/6   inch line spacing or programmable with 027 065 nnn (IBM)
027 051 nnn                      Select n/216 inch line spacing
027 052                          Select italic characters (TECO DRAFT)
027 053                          Cancel italic characters (TECO DRAFT)
027 053 nnn                      Turn automatic line feed on/off nnn=000:off nnn=001:on (IBM)
027 054                          Permit printing of ASCII codes 128 to 159 (TECO)
027 054                          Select world trade character set (IBM)
027 055                          Prevent printing of ASCII codes 128 to 159 (TECO)
027 055                          Select normal character set (IBM)
027 056                          Disable paper end detecter
027 057                          Enable paper end detecter
027 058 000 000 000              Copy ROM CG to download CG
027 060                          Return print head to home
027 061                          Clear MSB of incoming data
027 062                          Set MSB of incoming data
027 063 nnn mmm                  Re-assign 8-pin image commands nnn= 075, 076,089,090 0<=mmm<=6
027 064                          Initialize printer
027 065 nnn                      Select n/72 inch line spacing 0<=nnn<=85 (TECO)
027 065 nnn                      Set programmable line spacing to n/72 inch 0<=nnn<=85 (IBM)
027 066 n00 n01..n015 000        Set vertical tabs 1<=nnn<=255
027 067 nnn                      Set form length in lines 1<=nnn<=127
027 067 nnn 000                  Set form length in inches 1<=nnn<=22 ????
027 068 n00 n01..n031            Set horizontal tabs
027 069                          Select emphasized printing
027 070                          Cancel emphasized printing
027 071                          Select double strike printing (DRAFT)
027 072                          Cancel double strike printing (DRAFT)
027 073 nnn                      Set ability to print control codes on/off nnn=000:on nnn=001:off
027 074 nnn                      Execute n/216 inch line feed
027 075 aaa bbb ..data..         Set single density bit image mode
027 076 aaa bbb ..data..         Set double density bit image mode
027 077                          Select elite characters (DRAFT)
027 078 nnn                      Set skip perforation length 1<=nnn<=127
027 079                          Cancel skip perforation
027 080                          Select pica characters
027 081 nnn                      Set right margin 1<=nnn<=255
027 082 nnn                      Select international character set 1<=nnn<=10
027 082                          Restore Default tab setting
027 083 nnn                      Select superscipt/subscript mode nnn=000:subscript nnn=001:superscipt
027 084                          Cancel superscipt/subscript mode
027 085 nnn                      Set Unidirectional printing on/off nnn=000:off nnn=001:on
027 087 nnn                      Set enlarged printing off/on nnn=000:off nnn=001:on
027 088 aaa bbb                  Set left and right margins 0<=aaa,bbb<=255
027 089 aaa bbb ..data..         Set high speed double density bit image mode
027 090 aaa bbb ..data..         Set quadruple density bit image mode
027 092 mmm aaa bbb ..data..     9 bit image mode selection mmm=000:single mmm=001:double 0<=aaa,bbb<=255
027 097 nnn                      Set justification mode nnn=000:left 001:centre 002:right 003:full
027 098 ccc n00 n01..n15 000     Set vertical tabs in channel 0<=ccc<=7 0<=nnn<=255
027 105 nnn                      Set incremental and view print mode on/off nnn=000:off nnn=001:on
027 106 nnn                      Reverse line feed n/216 inches 0<=nnn<=255
027 108 nnn                      Set left margin 0<=nnn<=255
027 112 nnn                      Set proportional spacing on off nnn=000:off nnn=001:on
027 115 nnn                      Set half speed printing on/off nnn=000:off nnn=001:on
027 120 nnn                      Set NLQ printing on/off nnn=000:off nnn=001:on
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
*/

