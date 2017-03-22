#define DEBUGGING 0

#include <stdlib.h>    /* most prototypes and #defines            */
#include <stdio.h>     /* required for printf()                   */
#include <string.h>    /* required for string handling functions  */
#include <conio.h>     /* required for getch()                    */

void Prefix_String ( char *Str, int Char );
void DecodePacket ( int Word );

/* 1 to 9 */

char *a[] = { "",
              "one",
              "two",
              "three",
              "four",
              "five",
              "six",
              "seven",
              "eight",
              "nine"
             };

/* 10 to 19 */

char *b[] = { "ten",
              "eleven",
              "twelve",
              "thirteen",
              "fourteen",
              "fifteen",
              "sixteen",
              "seventeen",
              "eighteen",
              "ninteen"
            };

/* 20 to ninty */

char *c[] = { "",
              "",
              "twenty",
              "thirty",
              "forty",
              "fifty",
              "sixty",
              "seventy",
              "eighty",
              "ninty"
            };

/* 100 to 1 000 000 000 000 */

char *d[] = { "",
              "thousand",
              "million",
              "billion",
              "trillion",
              "zillion"
            };

char *int_ptr = NULL;                      // pointer to integer portion
char *dec_ptr = NULL;                      // pointer to decimal portion
int  StrPos,
     PktPos,
     Packet,
     i,
     Count,
     Word;
char pkt[6][4];                            // array of 6 four byte strings to hold numeric packets
char wrd[6][81];                           // array of 6 81 byte strings to hold alphanumeric packets
char Bigword[512];                         // main string to hold result

int main ( int argc, char *argv[] )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ MAIN : argc = argument count including program name in argv[0] ³*/
/*³        argv = array of pointers; one for each argument         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   if (argc < 2)                           // if no arguments
   {
      printf("USAGE : %s <number>",argv[0]); // print usage message
      exit(1);                             // and exit to OS
   }

   int_ptr = argv[1];                      // integer portion starts at the
                                           // beginning of argv[1]

   dec_ptr = strchr(argv[1],'.');          // Search for decimal point.
   if (dec_ptr != NULL)                    // If found ?
   {
      *dec_ptr = 0;                        // Replace it with a NULL marking
                                           // the end of the integer portion.
      dec_ptr++;                           // Decimal portion starts at next
                                           // byte
   }
   else
      dec_ptr = argv[1] + strlen(argv[1]); // set pointer for decimal portion
                                           // at the terminating NULL of the
                                           // argument

#if DEBUGGING
   puts(int_ptr);
   puts(dec_ptr);
   getch();
#endif

   StrPos = strlen(int_ptr);               // Number of digits to work with
   Packet = 1;                             // Counter tracking number of
                                           // packets made

   while (StrPos >= 1)                     // Reverse Loop
   {
      PktPos = 1;                          // Counter tracking position in packet
      strcpy(pkt[Packet-1],"");            // Initialize Packet
      while (PktPos <= 3 && StrPos >= 1)   // Loop through Packet
      {
         Prefix_String(pkt[Packet-1],int_ptr[StrPos-1]); // Add digits to packet
         StrPos--;                         // Decrease number of digits remaining
         PktPos++;                         // Increase packet size counter
      }

      Packet++;                            // Declare packet # "cc"
   }
   Packet--;                               // Correct for number of packets made

#if DEBUGGING
   for (i = 0; i<Packet; i++)
   {
      puts(pkt[i]);
      getch();
   }
#endif

   Count = Packet;                         // Set counter to number of packets
                                           // made

   printf("Number = %s.%s\n", int_ptr, dec_ptr );
                                           // Display the number

   strcpy( Bigword, "" );                  // Initialize main string variable

   Word = Packet;

   while (Count>=1)                        // Reverse loop to build name LHS. to RHS.
   {
      strcpy(wrd[Word-1],"");              // Initialize packet's word containing variable

      DecodePacket(Word-1);                // Call analysing function

      if (strlen(wrd[Word-1]) > 0)         // If the packet was none "000"
      {
         strcat(Bigword," ");
         strcat(Bigword,wrd[Word-1]);      // Concatenate to form bigger sentence
      }

#if DEBUGGING
      puts("---------");
      puts(wrd[Word-1]);
      puts(Bigword);
      getch();
#endif

      Count--;                             // Decrease the number of packets remaining
      Word--;
   }

   printf("String = %s\n", Bigword );      // Display the sentence

   return(0);
}
#pragma optimize("lge",off)

void Prefix_String ( char *Str, int Char )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Prefix zstring Str with character Char                  ³*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   register int len;

   len = strlen(Str);
   strrev(Str);
   Str[len]   = (char)Char;
   Str[len+1] = 0;
   strrev(Str);
}

#pragma optimize("",on)  /* restore previous values */

void DecodePacket ( int Which )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Convert each group of three digits into their word      ³*/
/*³ equivalents.                                            ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   switch( strlen(pkt[Which]) )
   {
      case 3 :
         if ( !strcmp(pkt[Which],"000") ) break;

         strcpy(wrd[Which],a[pkt[Which][0]-'0']);

         if ( pkt[Which][1] == '0' && pkt[Which][2] == '0' )
         {
            strcat(wrd[Which], " hundred");
         }
         else
         {
            if ( pkt[Which][0] != '0' )
               strcat(wrd[Which], " hundred and");
            else
               strcat(wrd[Which], "and");
         }

         if ( pkt[Which][1] != '0' )
         {
            if ( pkt[Which][1] != '1' )
            {
               strcat(wrd[Which]," ");
               strcat(wrd[Which],c[pkt[Which][1]-'0']);
               if ( pkt[Which][2] != '0' )
               {
                  strcat(wrd[Which]," ");
                  strcat(wrd[Which],a[pkt[Which][2]-'0']);
               }
            }
            else
            {
               strcat(wrd[Which]," ");
               strcat(wrd[Which],b[pkt[Which][2]-'0']);
            }
         }
         else
         {
            if ( pkt[Which][2] != '0' )
            {
               strcat(wrd[Which]," ");
               strcat(wrd[Which],a[pkt[Which][2]-'0']);
            }
         }
         strcat(wrd[Which]," ");
         strcat(wrd[Which],d[Count-1]);

         break;

      case 2 :
         if ( strlen(int_ptr)>3 )
         {
            if ( Count != Packet )
               strcat(wrd[Which]," and");
         }
         if ( pkt[Which][0] != '0' )
         {
            if ( pkt[Which][0] != '1' )
            {
               strcat(wrd[Which]," ");
               strcat(wrd[Which],c[pkt[Which][0]-'0']);
               if ( pkt[Which][1] != '0' )
               {
                  strcat(wrd[Which]," ");
                  strcat(wrd[Which],a[pkt[Which][1]-'0']);
               }
            }
            else
            {
               strcat(wrd[Which]," ");
               strcat(wrd[Which],b[pkt[Which][1]-'0']);
            }
         }
         else
         {
            if ( pkt[Which][1] != '0' )
            {
               strcat(wrd[Which]," ");
               strcat(wrd[Which],a[pkt[Which][1]-'0']);
            }
         }
         strcat(wrd[Which]," ");
         strcat(wrd[Which],d[Count-1]);
         break;

      case 1 :
         if ( strlen(int_ptr)>3 )
         {
            if ( Count != Packet )
               strcat(wrd[Which], " and");
         }
         if ( pkt[Which][0] != '0' )
         {
            strcat(wrd[Which]," ");
            strcat(wrd[Which],a[pkt[Which][0]-'0']);
            strcat(wrd[Which]," ");
            strcat(wrd[Which],d[Count-1]);
         }
         break;
   }

}
