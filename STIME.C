#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

#include <dodtype.h>
#include <dosdate.h>
#include <taskfun.h>
#include <biosvid.h>
#include <gen.h>
#include <convdos.h>
#include <dally.h>
#include <iaconv.h>
#include <ini.h>

#ifndef MK_FP
#define MK_FP(seg,ofs) ((void far *)(((unsigned long)(seg) << 16) | (ofs)))
#endif

struct ts_tag
{
   BYTE Hour;
   BYTE Min;
   BYTE Sec;
   BYTE HSec;
   BYTE Day;
   BYTE Month;
   WORD Year;
   WORD Zone;
   BYTE DayOfWeek;
} ts;

int         mainprocessStack [500]={0};
WORD PASCAL CESXQQ;
PBYTEFILECTL pIniFile;
struct
{
   unsigned int  Hour;
   unsigned int  Min;
   unsigned int Str[128];
} Data[8];

char DOW[8][4] = {"Sun",
                  "Mon",
                  "Tue",
                  "Wed",
                  "Thu",
                  "Fri",
                  "Sat",
                  "All"};


/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
char far *SetVioAddress (void)
{
   union REGS regs;

   regs.h.ah = 0x0f;
   int86(0x10,&regs,&regs);

   if (regs.h.al == 7)
      return ( MK_FP(0xb000,0x0090) );
   else
      return ( MK_FP(0xb800,0x0090) );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void PASCAL mainprocess ( void )
{
   int i;
   char timestr[10];
   unsigned char BufferFull;
   char far *x;

   x = SetVioAddress();

   NameProcess ( "STime", 5 );
   EnableStackChecking ( 1000 );

   for(;;)
   {
      Delay (70);
      GetDateAndTime ((GPBYTE)&ts);
      /* geoff */
      itoa (ts.Hour , timestr, 10);
      timestr[2] = ':';
      itoa (ts.Min , &timestr[3], 10);
      timestr[5] = ':';
      if(ts.Sec < 10)
      {
	 timestr[6] = 0x30;
	 itoa (ts.Sec , &timestr[7], 10);
      }
      else
	 itoa (ts.Sec , &timestr[6], 10);
      timestr[9] = 0;
      for(i=0;i<8;i++)
      {
	 *(x+(i*2)) = timestr[i];
	 *(x+(i*2)+1) = 0x1f;
      }
      /* to here */
      //if (ts.Sec == 0) putch('\a');

      if ((Data[7].Hour == ts.Hour) &&
          (Data[7].Min  == ts.Min ) )
         ts.DayOfWeek = 7;

      if ((Data[ts.DayOfWeek].Hour == ts.Hour) &&
          (Data[ts.DayOfWeek].Min  == ts.Min ) )
      {
         i = 0;
         while(Data[ts.DayOfWeek].Str[i])
         {
            do
            {
               unsigned char h,l;
               h = (unsigned char)(Data[ts.DayOfWeek].Str[i] / 256);
               l = (unsigned char)(Data[ts.DayOfWeek].Str[i] % 256);

               _asm
               {
                  mov AH,05h
                  mov CH,h
                  mov CL,l
                  int 16h
                  mov BufferFull,AL
               }
               Delay(10);
            } while (BufferFull);

            i++;
         }
         Delay(4368); /* delay 1 minute */
      }

   }
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
int main (int argc,char **argv)
{
   PCBTYPE   PCB;
   char far *x;
   int       Erc;
   int i,j;
   int TempInt;
   char TempStr[128];
   char *p;



   STKHQQ = 0;

   OpenIniFile ("STime.INI",9,&pIniFile);

   for (i=0;i<8;i++)
   {
      ReadIniFile (pIniFile,NULL, 0, DOW[i], 3, TempStr, &TempInt);
      p = TempStr;

      p = strtok(TempStr,":");
      Data[i].Hour = atoi(p);
      p = strtok(NULL,",");
      Data[i].Min  = atoi(p);

      j = 0;
      p = strtok( NULL, " ");   /* Find next token         */
      while( p != NULL )
      {
         Data[i].Str[j++] = (unsigned int)Hex2Long(p);
         Data[i].Str[j]   = 0;
         p = strtok( NULL, " ");   /* Find next token         */
      }
   }
   CloseIniFile (pIniFile);
   for (i=0;i<8;i++)
   {
      printf("%s = %d:%d,",DOW[i],Data[i].Hour,Data[i].Min);
      j=0;
      while(Data[i].Str[j])
      {
         unsigned char h,l;
         h = (unsigned char)(Data[i].Str[j] / 256);
         l = (unsigned char)(Data[i].Str[j] % 256);
         printf("%4.4X(%c%c) ",Data[i].Str[j],h,l);
         j++;
      }
      printf("\n");
   }

   Erc  = ActivateConvergentDos ("\0\r");
   if(Erc)
     {
       printf("ActivateConvergentDos Failed %d",Erc);
       exit(0);
     }

   PCB.Priority        = 0;
   PCB.fSys            = 0;
   PCB.DefRespEx       = 0;
   PCB.fDebug          = 0;
   PCB.DS              = GetDS();
   PCB.ES              = GetES();
   PCB.SS              = GetSS();
   x                   = (char far *)&(mainprocessStack [499]);
   PCB.SP              = FP_OFF (x);
   PCB.CSIP            = mainprocess;

   Erc = CreateProcess ((GPBYTE)&PCB);

   BiosTurnOnCursor();

   ExecuteProgram ("c:\\command.com", 14, NULL, 0);

   BiosTurnOnCursor();
   StopProcess();
   Delay   ( 72 );
   DeactivateConvergentDos ();
   return(0);
}
