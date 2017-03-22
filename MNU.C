#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>
#include <direct.h>

#include <tools.h>

#define NORMALEXIT    -2
#define ABNORMALEXIT  -1

#pragma pack(1)

typedef struct
{
   char     Drive[2];
   char     Directory[41];
   char     Command[41];
   int      ReturnToMenu;
   char     Spare[42];
} BATCH_REC;

#pragma pack()

#define MAXMENUS 128

int          FirstTime = 0;
int          NotInitiatedFromBatchFile = 0;
int          QuietExit = 0;
int          NoLogo    = 0;
char         ExeFileName[_MAX_PATH];
char         BatchFileName[_MAX_PATH];
char         MenuDataFileName[_MAX_PATH];
FILE        *MenuDataFile = NULL;
MENU_REC    *MenuRecArray[MAXMENUS] = {NULL};
MENU_REC    *EditRecArray[2];

char         current_drive[2];
char         current_directory[_MAX_PATH];
fieldtype   *pField;
int          numberoffields;
char         drive[_MAX_DRIVE];
char         dir  [_MAX_DIR];
char         fname[_MAX_FNAME];
char         ext  [_MAX_EXT];


void ExtraPaint             ( MENU_REC *MenuRecArray, int MenuNumber );
void EditExtraPaint         ( MENU_REC *MenuRecArray, int MenuNumber );

static void ReadMenuFile           ( void );
static void WriteMenuFile          ( void );
static void CreateBatchFileAndExit ( MENU_REC *MenuRecord, int MenuNumber, int ItemNumber );
static void BatchSetItemDefault    ( MENU_REC *MenuRecord, int ItemNumber );
static void EditMenu               ( MENU_REC *MenuRecArray[], int MenuNumber, int ItemNumber );
static void EditItem               ( MENU_REC *MenuRecArray, int ItemNumber );
int DOS33orLater ( void );

void DisplayBanner(void);
static void abend                  ( int ErrorNo );

int main ( int argc, char *argv[] )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   int  MenuNumber  = 0;
   int  ItemNumber  = 0;
   int  i;

   current_drive[0] = (char)(_getdrive() + 0x40); /* 1 = A, 2 = B etc */
   current_drive[1] = 0;
   getcwd(current_directory,_MAX_PATH);

   strcpy(ExeFileName,argv[0]);

   _splitpath( ExeFileName , drive, dir, fname , ext    );
   _makepath ( BatchFileName , drive, dir, "MENU", ".BAT" );
   _makepath ( MenuDataFileName, drive, dir, "MENU", ".DAT" );

   if (argc < 3)
      NotInitiatedFromBatchFile = 1;

   if (argc >= 2) MenuNumber = atoi(argv[1]);
   if (argc >= 3) ItemNumber = atoi(argv[2]);
   for (i=3;i<=argc-1;i++)
      if ((argv[i][0] == '/')||(argv[i][0] == '-'))
      {
         if ((argv[i][1] == 'q')||(argv[i][1] == 'Q')) QuietExit = 1;
      }

   if (getenv("QUIET"))
         NoLogo    = 1;

   setvgablink(0);

   for(;;)
   {
      int Ret;
      ReadMenuFile ();

      if (FirstTime || NotInitiatedFromBatchFile)
      {
         CreateBatchFileAndExit ( NULL                    , 0, 0 );
         return(0);
      }

      if (QuietExit)
         Ret = MenuProcess ( MAXMENUS, MenuRecArray, &MenuNumber, &ItemNumber, ExtraPaint, ESCAPE, ESCAPE, F2, 0 );
      else
         Ret = MenuProcess ( MAXMENUS, MenuRecArray, &MenuNumber, &ItemNumber, ExtraPaint, ESCAPE, F3, F2, 0 );

      switch (Ret)
      {
         case 0: CreateBatchFileAndExit ( MenuRecArray[MenuNumber], MenuNumber, ItemNumber ); return(0);
         case 1: CreateBatchFileAndExit ( NULL                    , MenuNumber, ItemNumber ); return(0);
         case 2: EditMenu               ( MenuRecArray            , MenuNumber, ItemNumber ); break;
      }
   }
}

void ExtraPaint ( MENU_REC *MenuRecArray, int MenuNumber )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   char HelpLine[81];
   int x1,x2;

   if ((MenuNumber == 0) && (!QuietExit))
      strcpy(HelpLine,"  <F2> Edit    <F3> Exit  ");
   else
      strcpy(HelpLine,"  <F2> Edit    <ESCAPE> Return  ");

   x1 = (80-strlen(HelpLine)-4) / 2;
   x2 = x1 + 4 + strlen(HelpLine);
   clrwin(x1,21,x2,23,MenuRecArray->NormalColour);
   box   (x1,21,x2,23,MenuRecArray->HighlightColour,3);
   shadow(x1,21,x2,23,MenuRecArray->ShadowColour,1);
   _put_string(x1+2,22,HelpLine);

   /*_put_string(x2-7,23,"Softnet");*/
   /*_put_string(18,25,"Copyright (c) Softnet Computer Services 1992");*/
}

void DisplayBanner (void)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   int x1,y1,x2,y2;
   char Str[81];

   int normalcolour = 0x71;
   int boxcolour    = 0x79;
   int shadowcolour = 0x10;
   int logocolour   = 0x70;
   int logocolourhi = 0x40;

   if (getvdev() == 1) /* mono */
   {
      normalcolour = 0x70;
      boxcolour    = 0x70;
      shadowcolour = 0x07;
      logocolour   = 0x70;
      logocolourhi = 0x07;
   }

   if (getvgamono()) /* vga mono */
   {
      normalcolour = 0x7F;
      boxcolour    = 0x7F;
      shadowcolour = 0x07;
      logocolour   = 0x70;
      logocolourhi = 0x07;
   }

   x1 =  4;
   x2 = 75;
   y1 =  3;
   y2 = 13;


   if ((getvdev() == 1) || (getvgamono())) /*mono*/
      box   (x1+2,y1+1,x2+2,y2+1,boxcolour,1);
   clrwin(x1,y1,x2,y2,normalcolour);
   box   (x1,y1,x2,y2,boxcolour,1);
   shadow(x1,y1,x2,y2,shadowcolour,1);
   _put_string(x1+2,y1+2,"SOFTNET Computer Services (Pty) Ltd                (Est. 1983)");
   _put_attr  (x1+2,y1+2,35,logocolour);
   _put_attr  (x1+6,y1+2,3,logocolourhi);
   _put_string(x1+2,y1+3,"");
   if (FirstTime || NotInitiatedFromBatchFile)
   {
      if (FirstTime)
         _put_string(x1+2,y1+4," - THE MENU SYSTEM HAS NOW BEEN INITIALISED !!!");
      else
         _put_string(x1+2,y1+4," - THE MENU SYSTEM WAS NOT LAUNCHED CORRECTLY !!!");
      _put_string(x1+2,y1+5," - Please run MENU (instead of MNU). e.g. C:\\>menu");
      _put_string(x1+2,y1+6," - Ensure that MENU.BAT can be found in your DOS PATH.");
   }
   else
   {
      _put_string(x1+2,y1+4," - The creators of Doddle, UTE, Nedinform, Rooikat and Wildkat.");
      _put_string(x1+2,y1+5," - Specialists in PC to mainframe connectivity and image processing.");
      _put_string(x1+2,y1+6," - Lotus Authorised Consultants and Novell Professional Developers.");
   }
   _put_string(x1+2,y1+7,"");
   _put_string(x1+2,y1+8,"TEL: +27(11) 463-4470        FAX: +27(11) 463-4476");
   _put_attr  (x1+2,y1+8,x2-x1-2,logocolour);

   sprintf(Str,"Softnet Freeware Menu Version 1.10");
   _put_string(1,1,Str);
   _put_attr  (1,1,strlen(Str),0x07);

   if (!FirstTime && !NotInitiatedFromBatchFile)
   {
      delay(1);
      sprintf(Str,"Performance index of this machine : %4lu",delay_reference/902);
      _put_string(1,y2+3,Str);
      _put_attr  (1,y2+3,strlen(Str),0x07);
      sprintf(Str,"                 IBM XT 8088/4.77 :    1");
      _put_string(1,y2+4,Str);
      _put_attr  (1,y2+4,strlen(Str),0x07);
      sprintf(Str,"          PS2 Model 70 80386SX/20 :   15");
      _put_string(1,y2+5,Str);
      _put_attr  (1,y2+5,strlen(Str),0x07);
      sprintf(Str,"          Compaq DeskPro 80486/33 :   72");
      _put_string(1,y2+6,Str);
      _put_attr  (1,y2+6,strlen(Str),0x07);
   }
   gotoxy(1,20);
}

static void CreateBatchFileAndExit ( MENU_REC *MenuRecord, int MenuNumber, int ItemNumber )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   FILE *batfile;

   batfile = fopen(BatchFileName,"wb");
   if (batfile == NULL) abend(1);
   if (DOS33orLater())
      fputc('@',batfile);
   fprintf(batfile,"ECHO OFF\r\n");

   fprintf(batfile,"%s %d %d %%1\r\n",ExeFileName, MenuNumber, ItemNumber);

   if (MenuRecord == NULL)
   {
      if ( !QuietExit )
         fprintf(batfile,"VER\r\n");
   }
   else
   {
      BATCH_REC *BatchRec;

      BatchRec = (BATCH_REC *)MenuRecord->Item[ItemNumber].Extra;

      fprintf(batfile,"%c:\r\n",BatchRec->Drive[0]);
      fprintf(batfile,"cd %s\r\n",BatchRec->Directory);

      if (DOS33orLater())
         fprintf(batfile,"call ");

      if (!memicmp(BatchRec->Command,"menu",4))
         fprintf(batfile,"MENU -Q\r\n");
      else
         fprintf(batfile,"%s\r\n",BatchRec->Command);

      if (BatchRec->ReturnToMenu)
      {
         fprintf(batfile,"%c:\r\n"  ,current_drive[0]);
         fprintf(batfile,"cd %s\r\n",current_directory);
         if ( QuietExit )
            fprintf(batfile,"%s -Q\r\n",BatchFileName);
         else
            fprintf(batfile,"%s\r\n",BatchFileName);
      }

      if (!memicmp(BatchRec->Command,"menu",4))
         QuietExit = 1;
      else
         QuietExit = 0;
   }
   fclose(batfile);

   fcloseall();


   if (!QuietExit)
   {
      setvgablink(1);
      clrscr(0x07);

      if (MenuRecord == NULL)
      {
         if (!NoLogo || FirstTime || NotInitiatedFromBatchFile)
            DisplayBanner();
         else
            gotoxy(1,1);
      }
      else
         gotoxy(1,1);
   }
   exit(0);
}

static void ReadMenuFile ( void )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   int i;
   size_t count;
   MENU_REC *Temp;

   Temp = malloc(sizeof(MENU_REC));
   if (Temp == NULL) abend(10);


   MenuDataFile = fopen(MenuDataFileName,"rb");
   if (MenuDataFile == NULL)
   {
      FirstTime = 1;
      MenuDataFile = fopen(MenuDataFileName,"wb");
      if (MenuDataFile == NULL) abend(2);

      MenuSetParamsDefault ( Temp, "Main Menu" );
      count = fwrite(Temp,1,sizeof(MENU_REC),MenuDataFile);
      if (count != sizeof(MENU_REC)) abend(3);
      fclose(MenuDataFile);

      MenuDataFile = fopen(MenuDataFileName,"rb");
      if (MenuDataFile == NULL) abend(4);
   }

   if (fseek(MenuDataFile,0L, SEEK_SET))
      abend(5);

   i = 0;

   for(;;)
   {
      count = fread(Temp,1,sizeof(MENU_REC),MenuDataFile);
      if (feof(MenuDataFile)) break;
      if (count != sizeof(MENU_REC)) abend(6);

      if (MenuRecArray[i] == NULL)
      {
         MenuRecArray[i] = Temp;
         Temp = malloc(sizeof(MENU_REC));
         if (Temp == NULL) abend(10);
      }
      else
         *MenuRecArray[i] = *Temp;

      i++;
      if (i>MAXMENUS) break;
   }

   fclose(MenuDataFile);
   free(Temp);
   Temp = NULL;
   if (MenuRecArray[0] == NULL) abend(11);
}

static void WriteMenuFile ( void )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   int i,j,k,l;
   size_t count;
   int NewRec[MAXMENUS];

   for (i=0; i<MAXMENUS; i++)
   {
      if (MenuRecArray[i] == NULL) continue;

      for (l=0; l<ITEMSPERMENU;l++)
      {
         for (j=0; j<ITEMSPERMENU;j++)
         {
            if (MenuRecArray[i]->Item[j].Description[0] == 0)
            {
               for (k=j; k<ITEMSPERMENU-1; k++)
                  MenuRecArray[i]->Item[k] = MenuRecArray[i]->Item[k+1];
               MenuRecArray[i]->Item[ITEMSPERMENU-1].Description[0] = 0;
            }
         }
      }
   }

   for (i=0; i<MAXMENUS; i++)
      NewRec[i] = -1;

   for (j=0, i=0; i<MAXMENUS; i++)
   {
      if (MenuRecArray[i] == NULL) continue;
      NewRec[i] = j;
      j++;
   }

   for (i=0; i<MAXMENUS; i++)
   {
      if (MenuRecArray[i] == NULL) continue;
      for (j=0; j<ITEMSPERMENU; j++)
      {
         if (MenuRecArray[i]->Item[j].Description[0])
            if ((MenuRecArray[i]->Item[j].SubMenu[0] == 'Y') &&
                (MenuRecArray[i]->Item[j].SubMenuNumber != 0))
            {
               MenuRecArray[i]->Item[j].SubMenuNumber = NewRec[MenuRecArray[i]->Item[j].SubMenuNumber];
            }
      }
   }

   MenuDataFile = fopen(MenuDataFileName,"wb");
   if (MenuDataFile == NULL) abend(7);

   for (i=0; i<MAXMENUS; i++)
   {
      if (MenuRecArray[i] == NULL) continue;
      count = fwrite(MenuRecArray[i],1,sizeof(MENU_REC),MenuDataFile);
      if (count != sizeof(MENU_REC)) abend(8);
      free(MenuRecArray[i]);
      MenuRecArray[i] = NULL;
   }
   fclose(MenuDataFile);
   ReadMenuFile();
}


static void EditMenu ( MENU_REC *MenuRecArray[], int MenuNumber, int ItemNumber )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   int new_colour;
   int last_char;
   window_t OneLineWin;
   int  EditMenuNumber  = 0;
   int  EditItemNumber  = 0;
   int  ExitEditMode = 0;
   int  NextItemNumber;
   int  SourceItemNumber;

   OneLineWin.scrnbufptr    = NULL;
   OneLineWin.x1            = 5;
   OneLineWin.y1            = 21;
   OneLineWin.x2            = 75;
   OneLineWin.y2            = 23;
   OneLineWin.window_colour = 0x70;
   OneLineWin.box_colour    = 0x7F;
   OneLineWin.box_type      = 1;
   OneLineWin.shadow_colour = 0x08;
   OneLineWin.shadow_type   = 1;

   EditRecArray[0] = malloc(sizeof(MENU_REC));
   if (EditRecArray[0] == NULL) abend(10);
   MenuSetParamsDefault ( EditRecArray[0], " E D I T   M E N U " );
   MenuSetItem          ( EditRecArray[0], 0, "T", "Title"                    , "N", "N", 0 );
   MenuSetItem          ( EditRecArray[0], 1, "A", "Add item"                 , "N", "N", 0 );
   MenuSetItem          ( EditRecArray[0], 2, "M", "Modify item"              , "N", "N", 0 );
   MenuSetItem          ( EditRecArray[0], 3, "C", "Copy item"                , "N", "N", 0 );
   MenuSetItem          ( EditRecArray[0], 4, "D", "Delete item"              , "N", "N", 0 );
   MenuSetItem          ( EditRecArray[0], 5, "R", "Relocate item"            , "N", "N", 0 );
   MenuSetItem          ( EditRecArray[0], 6, "E", "Edit Colours"             , "N", "Y", 1 );

   EditRecArray[1] = malloc(sizeof(MENU_REC));
   if (EditRecArray[1] == NULL) abend(10);
   MenuSetParamsDefault ( EditRecArray[1], " E D I T   C O L O U R S " );
   MenuSetItem          ( EditRecArray[1], 0, "F", "Background fill character", "N", "N", 0 );
   MenuSetItem          ( EditRecArray[1], 1, "B", "Background colour"        , "N", "N", 0 );
   MenuSetItem          ( EditRecArray[1], 2, "N", "Normal colour"            , "N", "N", 0 );
   MenuSetItem          ( EditRecArray[1], 3, "H", "Highlight colour"         , "N", "N", 0 );
   MenuSetItem          ( EditRecArray[1], 4, "S", "Shadow colour"            , "N", "N", 0 );

   for(;;)
   {
      switch (MenuProcess ( 2, EditRecArray, &EditMenuNumber, &EditItemNumber, EditExtraPaint, ESCAPE, ESCAPE, F2, 0 ))
      {
         case 0: /* selection was made */
         {
            switch (EditMenuNumber)
            {
               case 0 : /* EDIT MENU */
               {
                  switch (EditItemNumber)
                  {
                     case 0 : /* TITLE */
                        window_up(&OneLineWin);
                        last_char = EditFieldAlpha(OneLineWin.x1+2,OneLineWin.y1+1,"Menu Title : ",Pic('X',40), MenuRecArray[MenuNumber]->Heading);
                        gotoxy(1,26);
                        window_dn(&OneLineWin);
                        if (last_char == ENTER) WriteMenuFile();
                        break;
                     case 1 : /* ADD */
                        NextItemNumber = MenuNextItem ( MenuRecArray[MenuNumber] );
                        if (NextItemNumber >= ITEMSPERMENU)
                           putch(7);
                        else
                           ItemNumber = NextItemNumber;

                        MenuSetItemDefault  ( MenuRecArray[MenuNumber], ItemNumber );
                        BatchSetItemDefault ( MenuRecArray[MenuNumber], ItemNumber );
                        EditItem            ( MenuRecArray[MenuNumber], ItemNumber );
                        break;
                     case 2 : /* CHANGE */
                        switch (MenuProcess ( MAXMENUS, MenuRecArray, &MenuNumber, &ItemNumber, ExtraPaint, ESCAPE, F3, F2, 1 ))
                        {
                           case 0:
                              EditItem ( MenuRecArray[MenuNumber], ItemNumber );
                           case 1:
                           case 2:
                              break;
                        }
                        break;
                     case 3 : /* COPY */
                        SourceItemNumber = ItemNumber;
                        switch (MenuProcess ( MAXMENUS, MenuRecArray, &MenuNumber, &SourceItemNumber, ExtraPaint, ESCAPE, F3, F2, 1 ))
                        {
                           case 0:
                              NextItemNumber = MenuNextItem ( MenuRecArray[MenuNumber] );
                              if (NextItemNumber >= ITEMSPERMENU)
                                 putch(7);
                              else
                                 ItemNumber = NextItemNumber;

                              MenuSetItemDefault  ( MenuRecArray[MenuNumber], ItemNumber );
                              BatchSetItemDefault ( MenuRecArray[MenuNumber], ItemNumber );
                              MenuRecArray[MenuNumber]->Item[ItemNumber] = MenuRecArray[MenuNumber]->Item[SourceItemNumber];
                              EditItem            ( MenuRecArray[MenuNumber], ItemNumber );
                              break;
                           case 1:
                           case 2:
                              break;
                        }
                        break;
                     case 4 : /* DELETE */
                        switch (MenuProcess ( MAXMENUS, MenuRecArray, &MenuNumber, &ItemNumber, ExtraPaint, ESCAPE, F3, F2, 1 ))
                        {
                           case 0:
                              MenuDeleteItem ( MAXMENUS, MenuRecArray, MenuNumber, ItemNumber );
                              WriteMenuFile();
                              break;
                           case 1:
                           case 2:
                              break;
                        }
                        break;
                     case 5 : /* RELOCATE */
                        switch (MenuProcess ( MAXMENUS, MenuRecArray, &MenuNumber, &ItemNumber, ExtraPaint, ESCAPE, F3, F2, 1 ))
                        {
                           case 0:
                           {
                              int NewPos;
                              window_up(&OneLineWin);
                              NewPos = ItemNumber;
                              last_char = EditFieldNumeric(OneLineWin.x1+2,OneLineWin.y1+1,"New location [0..9] : ",Pic('9',1), &NewPos);
                              gotoxy(1,26);
                              window_dn(&OneLineWin);
                              if (last_char == ENTER)
                              {
                                 MenuRelocateItem ( MenuRecArray[MenuNumber], ItemNumber, NewPos);
                                 WriteMenuFile();
                              }
                              break;
                           }
                           case 1:
                           case 2:
                              break;
                        }
                        break;
                  } /* switch (EditItemNumber) */
                  break;
               } /* case (EditMenuNumber==0) */
               case 1: /* COLOUR MENU */
               {
                  switch (EditItemNumber)
                  {
                     case 0 : /* BACKGROUND CHARACTER */
                        window_up(&OneLineWin);
                        last_char = EditFieldAlpha(OneLineWin.x1+2,OneLineWin.y1+1,"Background character : ",Pic('A', 1), MenuRecArray[MenuNumber]->BackgroundCharacter);
                        gotoxy(1,26);
                        window_dn(&OneLineWin);
                        if (last_char == ENTER) WriteMenuFile();
                        break;
                     case 1 : /* BACKGROUND COLOUR */
                        new_colour = get_new_colour( "Background Colour" , MenuRecArray[MenuNumber]->BackgroundColour );
                        if (new_colour != NO_COLOUR_SELECTED)
                        {
                           MenuRecArray[MenuNumber]->BackgroundColour = (unsigned char)new_colour;
                           WriteMenuFile();
                        }
                        break;
                     case 2 : /* NORMAL COLOUR */
                        new_colour = get_new_colour( "Normal Colour"     , MenuRecArray[MenuNumber]->NormalColour );
                        if (new_colour != NO_COLOUR_SELECTED)
                        {
                           MenuRecArray[MenuNumber]->NormalColour = (unsigned char)new_colour;
                           WriteMenuFile();
                        }
                        break;
                     case 3 : /* HIGHLIGHT COLOUR */
                        new_colour = get_new_colour( "Highlight Colour"     , MenuRecArray[MenuNumber]->HighlightColour );
                        if (new_colour != NO_COLOUR_SELECTED)
                        {
                           MenuRecArray[MenuNumber]->HighlightColour = (unsigned char)new_colour;
                           WriteMenuFile();
                        }
                        break;
                     case 4 : /* SHADOW COLOUR */
                        new_colour = get_new_colour( "Shadow Colour"     , MenuRecArray[MenuNumber]->ShadowColour );
                        if (new_colour != NO_COLOUR_SELECTED)
                        {
                           MenuRecArray[MenuNumber]->ShadowColour = (unsigned char)new_colour;
                           WriteMenuFile();
                        }
                        break;
                  }
                  break;
               } /* case (EditMenuNumber==1) */
            } /* switch (EditMenuNumber) */
            break;
         }
         case 1:
         case 2:
            ExitEditMode = 1;
            break;
      } /* switch (MenuProcess) */
      if (ExitEditMode) break;
   } /* for */

   free(EditRecArray[0]);
   EditRecArray[0] = NULL;
   free(EditRecArray[1]);
   EditRecArray[1] = NULL;
}

void EditExtraPaint ( MENU_REC *MenuRecArray, int MenuNumber )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   char HelpLine[81];
   int x1,x2;

   if (MenuNumber == 0)
      strcpy(HelpLine,"  <F2> or <ESCAPE> Finish editing  ");
   else
      strcpy(HelpLine,"  <F2> Finish Editing   <ESCAPE> Return to previous menu  ");

   x1 = (80-strlen(HelpLine)-4) / 2;
   x2 = x1 + 4 + strlen(HelpLine);
   clrwin(x1,21,x2,23,MenuRecArray->NormalColour);
   box   (x1,21,x2,23,MenuRecArray->HighlightColour,3);
   shadow(x1,21,x2,23,MenuRecArray->ShadowColour,1);
   _put_string(x1+2,22,HelpLine);

   /*_put_string(x2-7,23,"Softnet");*/
   /*_put_string(18,25,"Copyright (c) Softnet Computer Services 1992");*/
}

static void EditItem ( MENU_REC *MenuRecord, int ItemNumber )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   #define EDITING    1
   #define PROCEEDING 2
   #define CANCELLING 3

   int i;
   int last_char;
   int item,mode;
   char far *save_screen;
   MENU_REC *Temp;
   BATCH_REC *BatchRec;

   BatchRec = (BATCH_REC *)MenuRecord->Item[ItemNumber].Extra;

   save_screen = pushwindow(1,1,80,25);
   _put_attr(1,1,2000,0x07);
   _put_char(1,1,2000,'±');
   clrwin(5,5,75,20,0x70);
   box(5,5,75,20,0x7F,1);
   _put_string(7,5,"OPTION PARAMETERS");
   shadow(5,5,75,20,0x08,1);

   mode = EDITING;
   item = 0;
   numberoffields = 8;

   pField = calloc(numberoffields,sizeof(fieldtype));
   if (!pField) abend(9);

   SetFieldInfo( 0, 7, 8,"   Description : ",'A',Pic('X',40), MenuRecord->Item[ItemNumber].Description   );
   SetFieldInfo( 1, 7, 9,"        Hotkey : ",'A',Pic('X', 1), MenuRecord->Item[ItemNumber].Hotkey        );
   SetFieldInfo( 2, 7,10,"        Hidden : ",'A',Pic('X', 1), MenuRecord->Item[ItemNumber].Hidden        );
   SetFieldInfo( 3, 7,12,"       Submenu : ",'A',Pic('X', 1), MenuRecord->Item[ItemNumber].SubMenu       );
   SetFieldInfo( 4, 7,13,"         Drive : ",'A',Pic('X', 1), BatchRec->Drive         );
   SetFieldInfo( 5, 7,14,"     Directory : ",'A',Pic('X',40), BatchRec->Directory     );
   if (DOS33orLater())
      SetFieldInfo( 6, 7,15,"          Call : ",'A',Pic('X',40), BatchRec->Command       );
   else
      SetFieldInfo( 6, 7,15,"       Command : ",'A',Pic('X',40), BatchRec->Command       );
   SetFieldInfo( 7, 7,16,"Return to menu : ",'N',Pic('9', 1),&BatchRec->ReturnToMenu);

   SetFieldColours ( 0x7F, 0x07, 0x70 );
   DisplayFields();

   if (!DOS33orLater())
   {
      _put_text(6,18,0x7F,"  WARNING : DOS version is older than 3.30                          ");
      _put_text(6,19,0x7F,"  If command is a batch file, return to menu will not be automatic  ");
   }

   while (mode==EDITING)
   {
      if (item<0) item = numberoffields-1;
      if (item>numberoffields-1) item = 0;

      last_char = EditField(item);

      switch (item)
      {
         case 0 :
            if ((MenuRecord->Item[ItemNumber].Hotkey[0] == ' ') ||
                (MenuRecord->Item[ItemNumber].Hotkey[0] == 0))
               MenuRecord->Item[ItemNumber].Hotkey[0] = MenuRecord->Item[ItemNumber].Description[0];
            DisplayFields();
            break;
         case 2 :
            if (MenuRecord->Item[ItemNumber].Hidden[0] == 'y')
               MenuRecord->Item[ItemNumber].Hidden[0] = 'Y';
            if (MenuRecord->Item[ItemNumber].Hidden[0] != 'Y')
               MenuRecord->Item[ItemNumber].Hidden[0] = 'N';
            DisplayFields();
            break;
         case 3 :
            if (MenuRecord->Item[ItemNumber].SubMenu[0] == 'y')
               MenuRecord->Item[ItemNumber].SubMenu[0] = 'Y';
            if (MenuRecord->Item[ItemNumber].SubMenu[0] != 'Y')
               MenuRecord->Item[ItemNumber].SubMenu[0] = 'N';
            DisplayFields();
            break;
      }

      switch (last_char)
      {
         case UP     : item--;      break;
         case DOWN   : item++;      break;
         case BACKTAB: item--;      break;
         case TAB    : item++;      break;
         case PGUP   : item = 0;    break;
         case PGDN   : item = numberoffields-1;    break;
         case ENTER  : mode = PROCEEDING; break;
         case ESCAPE : mode = CANCELLING; break;
         default     : item++;      break;
      }
   }
   if (mode == PROCEEDING)
   {
      if (MenuRecord->Item[ItemNumber].SubMenu[0] == 'Y')
      {
         if (MenuRecord->Item[ItemNumber].SubMenuNumber == 0)
         {
            for (i=0; i<MAXMENUS; i++)
            {
               if (MenuRecArray[i] == NULL)
               {
                  MenuRecord->Item[ItemNumber].SubMenuNumber = i;
                  break;
               }
            }

            Temp = malloc(sizeof(MENU_REC));
            if (Temp == NULL) abend(10);

            MenuSetParamsDefault ( Temp, MenuRecord->Item[ItemNumber].Description );

            MenuRecArray[MenuRecord->Item[ItemNumber].SubMenuNumber] = Temp;
         }
      }
      else
      {
         if (MenuRecord->Item[ItemNumber].SubMenuNumber != 0)
         {
            MenuDeleteSubMenu ( MAXMENUS, MenuRecArray, MenuRecord->Item[ItemNumber].SubMenuNumber );
            MenuRecord->Item[ItemNumber].SubMenuNumber = 0;
         }
      }
      WriteMenuFile();
   }
   else
      ReadMenuFile();
   popwindow(save_screen,1,1,80,25);
}

static void BatchSetItemDefault ( MENU_REC *MenuRecord, int ItemNumber )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   BATCH_REC *BatchRec;

   BatchRec = (BATCH_REC *)MenuRecord->Item[ItemNumber].Extra;

   memset(BatchRec,'\0',sizeof(BATCH_REC));

   strcpy(BatchRec->Drive,"C");
   strcpy(BatchRec->Directory,"\\");
   strcpy(BatchRec->Command,"");
   BatchRec->ReturnToMenu = 1;
}

void abend ( int ErrorNo )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   char Str[81];

   strcpy(Str,"MNU ERROR ");
   itoa(ErrorNo,Str+10,10);
   strcat(Str," : ");
   switch (ErrorNo)
   {
      case  1: strcat(Str,"Opening BatchFile before create");    break;
      case  2: strcat(Str,"Opening MenuDataFile before write");  break;
      case  3: strcat(Str,"Writing MenuDataFile on create");     break;
      case  4: strcat(Str,"Opening MenuDataFile before read");   break;
      case  5: strcat(Str,"Seeking MenuDataFile before read");   break;
      case  6: strcat(Str,"Reading MenuDataFile");               break;
      case  7: strcat(Str,"Opening MenuDataFile before write");  break;
      case  8: strcat(Str,"Writing MenuDataFile");               break;
      case  9: strcat(Str,"Allocating memory for Fields");       break;
      case 10: strcat(Str,"Allocating memory for Menus");        break;
      case 11: strcat(Str,"Invalid menu data file");             break;
      default: strcat(Str,"Unknown error");                      break;
   }
   clrscr(0x07);
   _put_text(1,1,0x4F,Str);
   gotoxy(1,26);
   getch();
   CreateBatchFileAndExit(NULL,0,0);
}

int DOS33orLater ( void )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   if ((_osmajor > 3) || (((_osmajor == 3))&&((_osminor == 30))) )
      return(1);
   else
      return(0);
}

