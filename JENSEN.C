#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>
#include <tools.h>

#define COLOUR(b,f) (b*16+f)
#define OFS(x,y) (2*(((y-1)*80)+(x-1)))

#define SYSTEM     " T E C O "
#define COPYRIGHT  "(c) 1989 Computer Lab Jhb"

int          option;
int          next;
int          opt;
int          set[256];
int          i;
int          nnn;
char         *scrnbuf;
int          xx,yy;
unsigned int screen;

int      main(void);
int      one(void);
int      two(void);
int      three(void);
void     setup_screen_type(void);
char     *push_scr(void);
void     pop_scr(char*);
void     putstring(int,int,char*,int);

main()
/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
�                                      �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸
*/
{
   setup_screen_type();
   setbuf(stdprn,NULL);

   scrnbuf = push_scr();
   xx = wherex();
   yy = wherey();
   gotoxy(1,26);
   clrscr(COLOUR(6,7));
   box(1,1,80,25,COLOUR(6,7),2);
   putstring(-1, 2,SYSTEM       ,COLOUR(6,15));
   putstring(53,25,COPYRIGHT    ,COLOUR(6,7));
   putstring(-1,24,"<ESC> Exit       <SPACE> Swap       </> Test",COLOUR(6,15));
/* if (!getch()) getch(); */

   next = 1;
   do
   {
      for (i=0;i<256;i++) set[i] = 0;
      clrwin(2,3,getvcols()-1,23,COLOUR(6,7));
      switch (next)
      {
         case 1 : one();   break;
         case 2 : two();   break;
         case 3 : three(); break;
      }
   } while (next != 0);

   gotoxy(xx,yy);
   pop_scr(scrnbuf);
   return(0);
}

one()
{
   putstring(-1, 3,"ONE",COLOUR(6,15));

   putstring( 5, 6,"A - Elite                          ",COLOUR(6,7));
   putstring( 5, 7,"B - NLQ                            ",COLOUR(6,7));
   putstring( 5, 8,"C - Proportional spacing           ",COLOUR(6,7));
   putstring( 5, 9,"D - Enlarged                       ",COLOUR(6,7));
   putstring( 5,10,"E - Enlarged for one line          ",COLOUR(6,7));
   putstring( 5,11,"F - Condensed (DRAFT)              ",COLOUR(6,7));
   putstring( 5,12,"G - Superscript                    ",COLOUR(6,7));
   putstring( 5,13,"H - Subscript                      ",COLOUR(6,7));
   putstring( 5,14,"I - Emphasized                     ",COLOUR(6,7));
   putstring( 5,15,"J - Double strike (DRAFT)          ",COLOUR(6,7));

   do
   {
      option = getch();

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
         case '/': fprintf(stdprn,"This is a Test\r\n"); break;
         case ' ': next = 2; break;
         case 27 : next = 0; break;
         case '1': next = 1; break;
         case '2': next = 2; break;
         case '3': next = 3; break;
      }

      opt = option - 91;

      if ((option >= 'a') && (option <= 'j'))
      {

         if (set[opt])
         {
            set[opt]=0;
            putstring(3,opt," ",COLOUR(6,7));
         }
         else
         {
            set[opt]=1;
            putstring(3,opt,"�",COLOUR(6,7));
         }
      }

   } while (next == 1);
   return(option);
}

two()
{
   putstring(-1, 3,"TWO",COLOUR(6,15));

   putstring( 5, 6,"A - Italic (TECO DRAFT)            ",COLOUR(6,7));
   putstring( 5, 7,"B - Underline                      ",COLOUR(6,7));
   putstring( 5, 8,"C - Print ASCII 128-159 (TECO)     ",COLOUR(6,7));
   putstring( 5, 9,"D - World Trade character set (IBM)",COLOUR(6,7));
   putstring( 5,10,"E - Print control codes            ",COLOUR(6,7));
   putstring( 5,11,"F - Paper end detecter             ",COLOUR(6,7));
   putstring( 5,12,"G - Select printer (On Line)       ",COLOUR(6,7));
   putstring( 5,13,"H - Set MSB to 1                   ",COLOUR(6,7));
   putstring( 5,14,"I - Set MSB to 0                   ",COLOUR(6,7));
   putstring( 5,15,"J - Automatic line feed (IBM)      ",COLOUR(6,7));
   putstring( 5,16,"K - Unidirectional                 ",COLOUR(6,7));
   putstring( 5,17,"L - Half speed                     ",COLOUR(6,7));
   putstring( 5,18,"M - Incremental and view           ",COLOUR(6,7));

   do
   {
      option = getch();

      switch (option)
      {
         case 'a': if (!set[ 1]) fprintf(stdprn,"%c%c"  , 27, 52    );
                   else          fprintf(stdprn,"%c%c"  , 27, 53    ); break;
         case 'b': if (!set[ 2]) fprintf(stdprn,"%c%c%c", 27, 45,  1);
                   else          fprintf(stdprn,"%c%c%c", 27, 45,  0); break;
         case 'c': if (!set[ 3]) fprintf(stdprn,"%c%c"  , 27, 54    );
                   else          fprintf(stdprn,"%c%c"  , 27, 55    ); break;
         case 'd': if (!set[ 4]) fprintf(stdprn,"%c%c"  , 27, 54    );
                   else          fprintf(stdprn,"%c%c"  , 27, 55    ); break;
         case 'e': if (!set[ 5]) fprintf(stdprn,"%c%c%c", 27, 73,  1);
                   else          fprintf(stdprn,"%c%c%c", 27, 73,  0); break;
         case 'f': if (!set[ 6]) fprintf(stdprn,"%c%c"  , 27, 57    );
                   else          fprintf(stdprn,"%c%c"  , 27, 56    ); break;
         case 'g': if (!set[ 7]) fprintf(stdprn,"%c"    , 17        );
                   else          fprintf(stdprn,"%c"    , 19        ); break;
         case 'h': if (!set[ 8]) fprintf(stdprn,"%c%c"  , 27, 62    );
                   else          fprintf(stdprn,"%c%c"  , 27, 35    ); break;
         case 'i': if (!set[ 9]) fprintf(stdprn,"%c%c"  , 27, 61    );
                   else          fprintf(stdprn,"%c%c"  , 27, 35    ); break;
         case 'j': if (!set[10]) fprintf(stdprn,"%c%c%c", 27, 53,  1);
                   else          fprintf(stdprn,"%c%c%c", 27, 53,  0); break;
         case 'k': if (!set[11]) fprintf(stdprn,"%c%c%c", 27, 85,  1);
                   else          fprintf(stdprn,"%c%c%c", 27, 85,  0); break;
         case 'l': if (!set[12]) fprintf(stdprn,"%c%c%c", 27,115,  1);
                   else          fprintf(stdprn,"%c%c%c", 27,115,  0); break;
         case 'm': if (!set[13]) fprintf(stdprn,"%c%c%c", 27,105,  1);
                   else          fprintf(stdprn,"%c%c%c", 27,105,  0); break;
         case '/': fprintf(stdprn,"This is a Test\r\n"); break;
         case ' ': next = 3; break;
         case 27 : next = 0; break;
         case '1': next = 1; break;
         case '2': next = 2; break;
         case '3': next = 3; break;
      }

      opt = option - 91;

      if ((option >= 'a') && (option <= 'm'))
      {

         if (set[opt])
         {
            set[opt]=0;
            putstring(3,opt," ",COLOUR(6,7));
         }
         else
         {
            set[opt]=1;
            putstring(3,opt,"�",COLOUR(6,7));
         }
      }

   } while (next == 2);
   return(option);
}

three()
{
   nnn = 1;
   putstring(-1, 3,"THREE",COLOUR(6,15));

   putstring( 5, 6,"A - Initialize                            ",COLOUR(6,7));
   putstring( 5, 7,"B - Dummy command                         ",COLOUR(6,7));
   putstring( 5, 8,"C - Bell                                  ",COLOUR(6,7));
   putstring( 5, 9,"D - Back space                            ",COLOUR(6,7));
   putstring( 5,10,"E - Horizontal tab                        ",COLOUR(6,7));
   putstring( 5,11,"F - Line feed                             ",COLOUR(6,7));
   putstring( 5,12,"G - Execute nnn/216 inch line feed        ",COLOUR(6,7));
   putstring( 5,13,"H - Execute nnn/216 inch reverse line feed",COLOUR(6,7));
   putstring( 5,14,"I - Vertical tab                          ",COLOUR(6,7));
   putstring( 5,15,"J - Form feed                             ",COLOUR(6,7));
   putstring( 5,16,"K - Carriage return                       ",COLOUR(6,7));
   putstring( 5,17,"L - Cancel line                           ",COLOUR(6,7));
   putstring( 5,18,"M - Delete character                      ",COLOUR(6,7));

   do
   {
      option = getch();

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
         case '/': fprintf(stdprn,"This is a Test"); break;
         case ' ': next = 1; break;
         case 27 : next = 0; break;
         case '1': next = 1; break;
         case '2': next = 2; break;
         case '3': next = 3; break;
      }

   } while (next == 3);
   return(option);
}
/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컫컴컴컴컴컴컴컴컴컴컴컴컴컫컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
쿏escription                                 �                         �                                              �
쳐컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컵컴컴컴컴컴컴컴컴컴컴컴컴컵컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
쿔nitialize                                  � 027 064                 �                                              �
쿏ummy command                               � 027 025 nnn             �                                              �
쿍ell                                        � 007                     �                                              �
쿍ack space                                  � 008                     �                                              �
쿓orizontal tab                              � 009                     �                                              �
쿗ine feed                                   � 010                     �                                              �
쿐xecute nnn/216 inch line feed              � 027 074 nnn             �                                              �
쿐xecute nnn/216 inch reverse line feed      � 027 106 nnn             � 0<=nnn<=255                                  �
쿣ertical tab                                � 011                     �                                              �
쿑orm feed                                   � 012                     �                                              �
쿎arriage return                             � 013                     �                                              �
쿎ancel line                                 � 024                     �                                              �
쿏elete character                            � 127                     �                                              �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컨컴컴컴컴컴컴컴컴컴컴컴컴컨컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컫컴컴컴컴컴컴컴컴컴컴컴컴컫컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
쿏escription                                 �                         �                                              �
쳐컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컵컴컴컴컴컴컴컴컴컴컴컴컴컵컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
�                                            �                         �                                              �
쿞elect print mode                           � 027 033 nnn             � 0<=nnn<=255                                  �
쿞elect international character set          � 027 082 nnn             � 1<=nnn<=10                                   �
쿕ustification left                          � 027 097 000             �                                              �
쿕ustification centre                        � 027 097 001             �                                              �
쿕ustification right                         � 027 097 002             �                                              �
쿕ustification full                          � 027 097 003             �                                              �
�                                            �                         �                                              �
쿞et form length in lines                    � 027 067 nnn             � 1<=nnn<=127                                  �
쿞et form length in inches                   � 027 067 nnn 000         � 1<=nnn<=127                                  �
쿞et skip perforation length                 � 027 078 nnn             � 1<=nnn<=127                                  �
쿎ancel skip perforation                     � 027 079                 �                                              �
�                                            �                         �                                              �
쿞et inter-character space                   � 027 032 nnn             � 0<=nnn<=127                                  �
�                                            �                         �                                              �
쿞elect 7/72  inch line spacing = 21/216     � 027 049                 �                                              �
쿞elect 1/8   inch line spacing = 27/216     � 027 048                 �                                              �
쿞elect 1/6   inch line spacing = 36/216     � 027 050                 �                                              �
쿞elect n/72  inch line spacing = 3n/216     � 027 065 nnn             � 0<=nnn<=85 for (IBM) must send 027 050 first �
쿞elect n/216 inch line spacing =  n/216     � 027 051 nnn             �                                              �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컨컴컴컴컴컴컴컴컴컴컴컴컴컨컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컫컴컴컴컴컴컴컴컴쩡컴컴컴컴컴컴컴커
쿏escription                                 � On             � Off            �
쳐컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컵컴컴컴컴컴컴컴컴탠컴컴컴컴컴컴컴캑
쿐lite (off=Pica)                            � 027 077        � 027 080        �
쿙LQ                                         � 027 120 001    � 027 120 000    �
쿛roportional spacing                        � 027 112 001    � 027 112 000    �
쿐nlarged                                    � 027 087 001    � 027 087 000    �
쿐nlarged for one line                       � 014 or 027 014 � 020            �
쿎ondensed                            (DRAFT)� 015 or 027 015 � 018            �
쿞uperscript                                 � 027 083 001    � 027 084        �
쿞ubscript                                   � 027 083 000    � 027 084        �
쿐mphasized                                  � 027 069        � 027 070        �
쿏ouble strike                        (DRAFT)� 027 071        � 027 072        �
쿔talic                          (TECO DRAFT)� 027 052        � 027 053        �
쿢nderline                                   � 027 045 001    � 027 045 000    �
쿐nable printing of ASCII 128-159      (TECO)� 027 054        � 027 055        �
쿥orld Trade character set off=normal   (IBM)� 027 054        � 027 055        �
쿐nable printing of control codes            � 027 073 001    � 027 073 000    �
�                                            �                �                �
쿛aper end detecter                          � 027 057        � 027 056        �
쿞elect printer (On Line)                    � 017            � 019            �
쿞et MSB to 1                                � 027 062        � 027 035        �
쿞et MSB to 0                                � 027 061        � 027 035        �
쿌utomatic line feed                    (IBM)� 027 053 001    � 027 053 000    �
쿢nidirectional                              � 027 085 001    � 027 085 000    �
쿓alf speed                                  � 027 115 001    � 027 115 000    �
쿔ncremental and view                        � 027 105 001    � 027 105 000    �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컨컴컴컴컴컴컴컴컴좔컴컴컴컴컴컴컴켸
*/
/*
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
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
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
*/
/*
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
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
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
*/

void setup_screen_type()
/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
�                                                          �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸
*/
{
   screen     = getvseg();
   if (getvmode() == 7) screen = 0xb000;
   return;
}

char *push_scr()
/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
� Save screen                                              �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸
*/
{
   char far *scrnptr;
   char *scrnbufptr;

   FP_SEG(scrnptr) = screen;
   FP_OFF(scrnptr) = 0;
   if (scrnbufptr = malloc(4000)) memcpy(scrnbufptr,scrnptr,4000);
   return(scrnbufptr);
}

void pop_scr(scrnbufptr)
char *scrnbufptr;
/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
� Restore screen                                           �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸
*/
{
   char far *scrnptr;

   FP_SEG(scrnptr) = screen;
   FP_OFF(scrnptr) = 0;
   memcpy(scrnptr,scrnbufptr,4000);
   free(scrnbufptr);
}

void putstring(x,y,line,colour)
int x;
int y;
char* line;
int colour;
/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
� Put string onto screen                                   �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸
*/
{
   if (x==-1) x = 41 - strlen(line)/2;
   _put_text(x,y,colour,line);
}
