#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <doddle.h>

FILE *File[2];
char TextRecord[2][256];
int RecordCount[2];
char _far *pScreen;

unsigned char NormalColour1  = 0x07;
unsigned char BrightColour1  = 0x0F;
unsigned char NormalColour2  = 0x70;
unsigned char BrightColour2  = 0x7F;

int CaseSensitive     = TRUE;
int RemoveWhiteSpaces = FALSE;

static VOID NextRecord(int which)
{
   char TempStr[20];
   char buffer[256];
   char *Ptr;

   if (feof(File[which]))
   {
      Beep();
      return;
   }

   fgets(TextRecord[which],255,File[which]);

   if ( TextRecord[which][strlen(TextRecord[which])-1] == '\n')
      TextRecord[which][strlen(TextRecord[which])-1] = '\0';

   if (RemoveWhiteSpaces)
   {
      Ptr = strtok(TextRecord[which]," \r\n\f\b\t\v");
      if (Ptr)
      {
         strcpy(buffer,Ptr);
         while (Ptr=strtok(NULL," \r\n\f\b\t\v"))
         {
            strcat(buffer," ");
            strcat(buffer,Ptr);
         }
         strcpy(TextRecord[which],buffer);
      }
   }

   ScrollFrame ( which+1,0,0xFF, strlen(TextRecord[which])/80+1,1);

   if (feof(File[which]))
      PutFrameCharsAttr ( which+1,0,10," EOF ",5,BrightColour1);
   else
      PutFrameCharsAttr ( which+1,0,10,TextRecord[which],strlen(TextRecord[which]),NormalColour1);

   RecordCount[which]++;
   itoa(RecordCount[which],TempStr,10);
   PutFrameCharsAttr ( 0,75,which*2,TempStr,strlen(TempStr),BrightColour2 );
}

static BOOLEAN  KbdCommand (VOID)
{
   int ch;

   ReadKbd ( &ch );
   switch ( toupper(ch) )
   {
      case '1' : NextRecord(0); break;
      case '2' : NextRecord(1); break;
      case 'S' :
      case 'N' : NextRecord(0);
                 NextRecord(1); break;
      case 'C' :
      case 'G' : strcpy(TextRecord[0], "");
                 strcpy(TextRecord[1], "");
                 for(;;)
                 {
                    if (CaseSensitive) {if (strcmp (TextRecord[0],TextRecord[1])) break;}
                    else               {if (stricmp(TextRecord[0],TextRecord[1])) break;}
                    if ( feof(File[0]) || feof(File[1]) )
                    {
                       Beep();
                       return ( TRUE );
                    }
                    NextRecord(0);
                    NextRecord(1);
                 } break;
      case 27  :
      case 'Q' : InitVidFrame ( 0,0,0,80,25,0,0,0,FALSE,FALSE );
                 ResetFrame ( 0 );
                 FillFrame ( 0, ' ', 0x07 );
                 PosFrameCursor ( 0,0,0 );
                 return( FALSE );
   }
    return( TRUE );
}

void main(int argc, char *argv[])
{
   int i;

   if (argc<3)                           { printf( "Usage : COMPARE <filename> <filename> [/C] [/W]\n"); return; }
   if (!(File[0] = fopen(argv[1],"rt"))) { printf( "Cannot open %s\n",argv[1]);                return; }
   if (!(File[1] = fopen(argv[2],"rt"))) { printf( "Cannot open %s\n",argv[2]);                return; }

   for (i=3;i<argc;i++)
      switch(toupper(argv[i][1]))
      {
         case 'C' : CaseSensitive     = FALSE; break;
         case 'W' : RemoveWhiteSpaces = TRUE;  break;
      }

   ResetVideo ( 80,25,1,0x0720,pScreen );
   InitVidFrame ( 0,0,0,80,25,0,0,0,FALSE,FALSE );
   ResetFrame ( 0 );
   if (ColorVid)
   {
      NormalColour1  = 0x17;
      BrightColour1  = 0x1F;
      NormalColour2  = 0x57;
      BrightColour2  = 0x5F;
   }

   InitVidFrame ( 0,0,11,80, 3,0,0,0,FALSE,FALSE );
   InitVidFrame ( 1,0, 0,80,11,0,0,0,FALSE,FALSE );
   InitVidFrame ( 2,0,14,80,11,0,0,0,FALSE,FALSE );

   FillFrame ( 0, ' ', NormalColour2  );
   FillFrame ( 1, ' ', NormalColour1 );
   FillFrame ( 2, ' ', NormalColour1 );

   PosFrameCursor ( 0,255,255 );

   memset(TextRecord[0],'Ä',80);
   PutFrameCharsAttr ( 0,0,0,TextRecord[0],80,NormalColour2  );
   PutFrameCharsAttr ( 0,0,2,TextRecord[0],80,NormalColour2  );
   PutFrameCharsAttr ( 0,0,1," <1> Step1   <2> Step2   <G> Compare   <N> Step   <Q> Quit                     ",80, NormalColour2  );

   PutFrameCharsAttr ( 0,40-strlen(argv[1])/2,0,argv[1],strlen(argv[1]),BrightColour2);
   PutFrameCharsAttr ( 0,40-strlen(argv[2])/2,2,argv[2],strlen(argv[2]),BrightColour2);

   RecordCount[0] = 0;
   RecordCount[1] = 0;

   NextRecord(0);
   NextRecord(1);

   while (KbdCommand());

}
