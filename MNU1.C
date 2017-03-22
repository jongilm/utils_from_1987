#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>
#include <direct.h>
/*#include <c:\c6\include\ctype.h>*/

#include <tools.h>

#define EDIT          -1
#define EXIT          -2
#define ABEND         -1
#define CANCEL        -1

#define ENTER         0x0D
#define ESCAPE        0x1B
#define TAB           0x09
#define BACKTAB      -0x0F
#define PGUP         -0x49
#define PGDN         -0x51
#define UP           -0x48
#define DOWN         -0x50
#define HOME         -0x47
#define END          -0x4F
#define LEFT         -0x4B
#define RIGHT        -0x4D
#define CTRL_F1      -0x5E
#define CTRL_F2      -0x5F

#pragma pack(1)
typedef struct
{
   char          heading[81];
   unsigned char background_character;
   unsigned char background_colour;
   unsigned char normal_colour;
   unsigned char highlight_colour;
   unsigned char shadow_colour;
   int           number_of_options;
} menu_parms;

typedef struct
{
   int      hotkey;
   char     description[81];
   int      return_to_menu;
   int      hidden;
   char     command[5][41];
} menu_options;
#pragma pack()

char         exefilename[129]  = "MNU.EXE";
char         batfilename[129]  = "MENU.BAT";
char         datafilename[129] = "MAIN.MNU";
FILE         *datafile;
menu_parms   mnu_parms;
menu_options mnu_options[50];
menu_options tmp_options[50];
menu_parms   maint_parms1;
menu_parms   maint_parms2;
menu_options maint_options[9];
int          menu_pos  = 0;
int          maint_pos = 0;

void read_parms(void);
void write_parms(void);
void initialise_option(int choice);
void create_batch_file(int choice);
void edit_menu(void);
void edit_option(int choice);
void delete_option(int choice);
int get_new_colour(char *heading, int oldcolour);
void clean_parms(void);

void menu_background(menu_parms *prms);
void menu_header(menu_parms *prms);
int  menu_width(menu_parms *prms, menu_options *opts);
int  menu_length(menu_parms *prms, menu_options *opts);
void menu_paint(menu_parms *prms, menu_options *opts);
int  menu_choice(menu_parms *prms, menu_options *opts, int Qkey1, int Qkey2, int *ppos);
int  menu_do(menu_parms *prms, menu_options *opts, int Qkey1, int Qkey2, int *ppos, int repaint);
void menu_set_item(menu_options *opts, int choice, int hotkey, char *description, int hidden, int return_to_menu, char command[5][41]);

char *pic(int ch, int length);
int field_zstr(int x, int y, char *prompt, char *picture, char *zstr);
int field_char(int x, int y, char *prompt, char *picture, char *ch);
int field_int(int x, int y, char *prompt, char *picture, int *value);
int field_hex(int x, int y, char *prompt, char *picture, char *value);



void debug(char *str, int i)
{
   char istr[21];
   clrscr(0x07);
   itoa(i,istr,10);
   _put_text(1,1,0x70,str);
   _put_text(1,2,0x70,istr);
   _put_text(1,3,0x70,mnu_options[0].command[0]);
   _put_text(1,4,0x70,mnu_options[0].command[1]);
   _put_text(1,5,0x70,mnu_options[0].command[2]);
   _put_text(1,6,0x70,mnu_options[0].command[3]);
   _put_text(1,7,0x70,mnu_options[0].command[4]);
   getch();
}

void abend(char *str1,char *str2)
{
   clrscr(0x07);
   gotoxy(1,1);
   fcloseall();
   create_batch_file(ABEND);
   _put_text(1,1,0x4F,"MENU : ABNORMAL TERMINATION");
   _put_text(1,2,0x4F,str1);
   _put_text(1,3,0x4F,str2);
   gotoxy(1,4);
   exit(1);
}

int main(int argc,char **argv)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   int choice;

   /*
   char drive [_MAX_DRIVE+1];
   char dir   [_MAX_DIR  +1];
   char fname [_MAX_FNAME+1];
   char exit  [_MAX_EXT  +1];

   _splitpath(argv[0],drive,dir,fname,ext);


   strrev(argv[0]);
   strcpy(batfilename,strchr(argv[0],'\\'));
   strrev(argv[0]);
   strrev(batfilename);
   strcat(batfilename,"MENU.BAT");
   */

   strcpy(exefilename,argv[0]);
   if (argc >= 2) strcpy(batfilename,argv[1]);
   if (argc >= 3) strcpy(datafilename,argv[2]);
   if (argc >= 4) menu_pos = atoi(argv[3]);

   for (;;)
   {
      read_parms();
      choice = menu_do(&mnu_parms, mnu_options, CTRL_F1, ESCAPE, &menu_pos,1);
      if      (choice >=  0)
      {
         create_batch_file(choice);
         break;
      }
      else if (choice == EDIT) edit_menu();
      else if (choice == EXIT)
      {
         create_batch_file(EXIT);
         break;
      }
   }
   clrscr(0x07);
   gotoxy(1,1);
   return(0);
}

void menu_set_item(menu_options *opts, int choice, int hotkey, char *description, int hidden, int return_to_menu, char command[5][41])
{
   opts[choice].hotkey         = hotkey;
   strcpy(opts[choice].description,description);
   opts[choice].hidden         = hidden;
   opts[choice].return_to_menu = return_to_menu;
   if (command == NULL)
   {
      strcpy(opts[choice].command[0],"");
      strcpy(opts[choice].command[1],"");
      strcpy(opts[choice].command[2],"");
      strcpy(opts[choice].command[3],"");
      strcpy(opts[choice].command[4],"");
   }
   else
   {
      strcpy(opts[choice].command[0],command[0]);
      strcpy(opts[choice].command[1],command[1]);
      strcpy(opts[choice].command[2],command[2]);
      strcpy(opts[choice].command[3],command[3]);
      strcpy(opts[choice].command[4],command[4]);
   }

}

void edit_menu(void)
{
   int choice, maint_choice, last_char, new_colour;
   int repaint = 1;
   window_t win;
   for (;;)
   {
      strcpy(maint_parms1.heading,"MENU MAINTENANCE");
      maint_parms1.background_character = (unsigned char)'±';
      maint_parms1.background_colour    = 0x07;
      maint_parms1.normal_colour        = 0x17;
      maint_parms1.highlight_colour     = 0x1F;
      maint_parms1.shadow_colour        = 0x08;
      maint_parms1.number_of_options    = 9;

      menu_set_item(maint_options, 0, 'A', "Add item"                 , 0, 1, NULL);
      menu_set_item(maint_options, 1, 'C', "Change item"              , 0, 1, NULL);
      menu_set_item(maint_options, 2, 'D', "Delete item"              , 0, 1, NULL);
      menu_set_item(maint_options, 3, 'T', "Title"                    , 0, 1, NULL);
      menu_set_item(maint_options, 4, 'F', "Background fill character", 0, 1, NULL);
      menu_set_item(maint_options, 5, 'B', "Background colour"        , 0, 1, NULL);
      menu_set_item(maint_options, 6, 'N', "Normal colour"            , 0, 1, NULL);
      menu_set_item(maint_options, 7, 'H', "Highlight colour"         , 0, 1, NULL);
      menu_set_item(maint_options, 8, 'S', "Shadow colour"            , 0, 1, NULL);

      win.scrnbufptr    = NULL;
      win.x1            = 5;
      win.y1            = 21;
      win.x2            = 75;
      win.y2            = 23;
      win.window_colour = 0x17;
      win.box_colour    = 0x1F;
      win.box_type      = 1;
      win.shadow_colour = 0x08;
      win.shadow_type   = 1;

      maint_choice = menu_do(&maint_parms1, maint_options, CTRL_F1, ESCAPE, &maint_pos, repaint);
      repaint = 0;
      if (maint_choice < 0) break;
      switch (maint_choice)
      {
         case 0 : /* ADD */
                  mnu_parms.number_of_options++;
                  initialise_option(mnu_parms.number_of_options-1);
                  edit_option(mnu_parms.number_of_options-1);
                  repaint = 1;
                  break;
         case 1 : /* CHANGE */
                  strcpy(maint_parms2.heading,"SELECT ITEM");
                  maint_parms2.background_character = (unsigned char)'±';
                  maint_parms2.background_colour    = 0x07;
                  maint_parms2.normal_colour        = 0x17;
                  maint_parms2.highlight_colour     = 0x1F;
                  maint_parms2.shadow_colour        = 0x08;
                  maint_parms2.number_of_options    = mnu_parms.number_of_options;
                  choice = menu_do(&maint_parms2, mnu_options, CTRL_F1, ESCAPE, &menu_pos,1);
                  if (choice >= 0) edit_option(choice);
                  repaint = 1;
                  break;
         case 2 : /* DELETE */
                  strcpy(maint_parms2.heading,"SELECT ITEM");
                  maint_parms2.background_character = (unsigned char)'±';
                  maint_parms2.background_colour    = 0x07;
                  maint_parms2.normal_colour        = 0x17;
                  maint_parms2.highlight_colour     = 0x1F;
                  maint_parms2.shadow_colour        = 0x08;
                  maint_parms2.number_of_options    = mnu_parms.number_of_options;
                  choice = menu_do(&maint_parms2, mnu_options, CTRL_F1, ESCAPE, &menu_pos,1);
                  if (choice >= 0) delete_option(choice);
                  repaint = 1;
                  break;
         case 3 : /* TITLE */
                  window_up(&win);
                  last_char = field_zstr(win.x1+2,win.y1+1,"Menu Title : ",pic('X',40), mnu_parms.heading);
                  gotoxy(1,26);
                 /* last_char = field_int (win.x1+2,win.y1+1,"Number of options : ",pic('9', 3),&mnu_parms.number_of_options);*/
                  window_dn(&win);
                  if (last_char == ENTER) write_parms();
                  break;
         case 4 : /* BACKGROUND CHARACTER */
                  window_up(&win);
                  last_char = field_char(win.x1+2,win.y1+1,"Background character : ",pic('A', 1),&mnu_parms.background_character);
                  gotoxy(1,26);
                  window_dn(&win);
                  if (last_char == ENTER) write_parms();
                  break;
         case 5 : /* BACKGROUND COLOUR */
                  new_colour = get_new_colour( "Background Colour" , mnu_parms.background_colour );
                  if (new_colour != CANCEL)
                  {
                     mnu_parms.background_colour = (unsigned char)new_colour;
                     write_parms();
                  }
                  break;
         case 6 : /* NORMAL COLOUR */
                  new_colour = get_new_colour( "Normal Colour"     , mnu_parms.normal_colour );
                  if (new_colour != CANCEL)
                  {
                     mnu_parms.normal_colour = (unsigned char)new_colour;
                     write_parms();
                  }
                  break;
         case 7 : /* HIGHLIGHT COLOUR */
                  new_colour = get_new_colour( "Highlight Colour"     , mnu_parms.highlight_colour );
                  if (new_colour != CANCEL)
                  {
                     mnu_parms.highlight_colour = (unsigned char)new_colour;
                     write_parms();
                  }
                  break;
         case 8 : /* SHADOW COLOUR */
                  new_colour = get_new_colour( "Shadow Colour"     , mnu_parms.shadow_colour );
                  if (new_colour != CANCEL)
                  {
                     mnu_parms.shadow_colour = (unsigned char)new_colour;
                     write_parms();
                  }
                  break;
      }
   }
}


void initialise_option(int choice)
{
   memset(&mnu_options[choice],'\0',sizeof(menu_options));

   mnu_options[choice].hotkey         = ' ';
   strcpy(mnu_options[choice].description,"");
   mnu_options[choice].hidden         = 0;
   mnu_options[choice].return_to_menu = 1;
   strcpy(mnu_options[choice].command[0],"");
   strcpy(mnu_options[choice].command[1],"");
   strcpy(mnu_options[choice].command[2],"");
   strcpy(mnu_options[choice].command[3],"");
   strcpy(mnu_options[choice].command[4],"");
}

void read_parms(void)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   int i;
   size_t count;

   datafile = fopen(datafilename,"rb");
   if (datafile == NULL)
   {
      clean_parms();
      datafile = fopen(datafilename,"wb");
      if (datafile == NULL) abend("opening before write",datafilename);
      count = fwrite(&mnu_parms,1,sizeof(menu_parms),datafile);
      if (count != sizeof(menu_parms)) abend("writing",datafilename);
      for (i=0;i<mnu_parms.number_of_options;i++)
      {
         count = fwrite(&mnu_options[i],1,sizeof(menu_options),datafile);
         if (count != sizeof(menu_options)) abend("writing",datafilename);
      }
      fclose(datafile);
      datafile = fopen(datafilename,"rb");
      if (datafile == NULL)
      {
         abend("opening before read",datafilename);
      }
   }
   count = fread(&mnu_parms,1,sizeof(menu_parms),datafile);
   if (count != sizeof(menu_parms)) abend("reading",datafilename);
   for (i=0;i<mnu_parms.number_of_options;i++)
   {
      count = fread(&mnu_options[i],1,sizeof(menu_options),datafile);
      if (count != sizeof(menu_options)) abend("writing",datafilename);
   }
   fclose(datafile);
}

void clean_parms(void)
{
   int i;
   menu_parms   parms;
   menu_options options;

   parms = mnu_parms;
   memset(&mnu_parms,'\0',sizeof(menu_parms));

   strcpy(mnu_parms.heading,parms.heading);
   mnu_parms.background_character = parms.background_character;
   mnu_parms.background_colour    = parms.background_colour   ;
   mnu_parms.normal_colour        = parms.normal_colour       ;
   mnu_parms.highlight_colour     = parms.highlight_colour    ;
   mnu_parms.shadow_colour        = parms.shadow_colour       ;
   mnu_parms.number_of_options    = parms.number_of_options   ;

   for (i=0;i<mnu_parms.number_of_options;i++)
   {
      options = mnu_options[i];
      memset(&mnu_options[i],'\0',sizeof(menu_options));

      mnu_options[i].hotkey         = options.hotkey;
      strcpy(mnu_options[i].description,options.description);
      mnu_options[i].hidden         = options.hidden;
      mnu_options[i].return_to_menu = options.return_to_menu;

      strcpy(mnu_options[i].command[0],options.command[0]);
      strcpy(mnu_options[i].command[1],options.command[1]);
      strcpy(mnu_options[i].command[2],options.command[2]);
      strcpy(mnu_options[i].command[3],options.command[3]);
      strcpy(mnu_options[i].command[4],options.command[4]);
   }
}

void write_parms(void)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   int i;
   size_t count;

   /*clean_parms();*/
   datafile = fopen(datafilename,"wb");
   if (datafile == NULL) abend("opening before write",datafilename);
   count = fwrite(&mnu_parms,1,sizeof(menu_parms),datafile);
   if (count != sizeof(menu_parms)) abend("writing",datafilename);
   for (i=0;i<mnu_parms.number_of_options;i++)
   {
      count = fwrite(&mnu_options[i],1,sizeof(menu_options),datafile);
      if (count != sizeof(menu_options)) abend("writing",datafilename);
   }
   fclose(datafile);
   read_parms();
}

void menu_background(menu_parms *prms)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   gotoxy(1,26);
   clrscr(prms->background_colour);
   _put_char(1,1,2000,prms->background_character);
}

void menu_header(menu_parms *prms)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   int x,y,delta_x,delta_y;

   delta_x = strlen(prms->heading);
   delta_y = 1;
   delta_x += 4;
   delta_y += 2;
   x = (getvcols() - delta_x)/2;
   y = 2;

   clrwin(x,y,x+delta_x-1,y+delta_y-1,prms->normal_colour);
   box(x,y,x+delta_x-1,y+delta_y-1,prms->highlight_colour,3);
   shadow(x,y,x+delta_x-1,y+delta_y-1,prms->shadow_colour,1);
   _put_string(x+2,y+1,prms->heading);
}

int menu_width(menu_parms *prms, menu_options *opts)
{
   int delta_x = 0, i;

   for (i=0;i<prms->number_of_options;i++)
      if (!opts[i].hidden)
         if ((int)strlen(opts[i].description) > delta_x)
            delta_x = strlen(opts[i].description);
   return(delta_x);
}

int menu_length(menu_parms *prms, menu_options *opts)
{
   int delta_y = 0, i;

   for (i=0;i<prms->number_of_options;i++)
      if (!opts[i].hidden)
         delta_y++;
   return(delta_y);
}

void menu_paint(menu_parms *prms, menu_options *opts)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   int x,y,delta_x,delta_y,i,j,hot_pos;
   char *hot_ptr;
   char save_description[81];

   menu_background(prms);
   menu_header(prms);

   delta_x = menu_width(prms, opts) + 4;
   delta_y = menu_length(prms, opts) + 4;

   x = (getvcols() - delta_x)/2;
   y = (20 - delta_y)/2 + 5;

   clrwin(x,y,x+delta_x-1,y+delta_y-1,prms->normal_colour);
   box(x,y,x+delta_x-1,y+delta_y-1,prms->highlight_colour,3);
   shadow(x,y,x+delta_x-1,y+delta_y-1,prms->shadow_colour,1);
   for (i=0,j=0;i<=prms->number_of_options-1;i++)
   {
      if (!opts[i].hidden)
      {
         hot_ptr = strchr(opts[i].description,opts[i].hotkey);
         if (!hot_ptr)
            hot_ptr = strchr(opts[i].description,toupper(opts[i].hotkey));
         if (!hot_ptr)
         {
            strcpy(save_description,opts[i].description);
            strupr(opts[i].description);
            hot_ptr = strchr(opts[i].description,toupper(opts[i].hotkey));
            strcpy(opts[i].description,save_description);
         }
         if (!hot_ptr)
            hot_ptr = opts[i].description;

         hot_pos = hot_ptr - opts[i].description;

         _put_attr(x+2+hot_pos,y+2+j,1,prms->highlight_colour);

         _put_string(x+2,y+2+j,opts[i].description);
         j++;
      }
   }
}

int menu_choice(menu_parms *prms, menu_options *opts, int Qkey1, int Qkey2, int *ppos)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   int ch,carry_on,i;
   int x,y,delta_x,delta_y;
   char old_attrs[81];
   char new_attrs[81];

   delta_x = menu_width(prms, opts);
   delta_y = menu_length(prms, opts);
   x = (getvcols() - delta_x)/2;
   y = (20 - delta_y)/2 + 5;

   carry_on = 1;
   while (carry_on)
   {
      if ((*ppos) < 0)                         (*ppos) = 0;
      if ((*ppos) > prms->number_of_options-1) (*ppos) = prms->number_of_options-1;

      _get_attrs(x-1,y+(*ppos),delta_x+2,old_attrs);
      for (i=0;i<delta_x+2;i++)
      {
         new_attrs[i] = (char)((old_attrs[i] & 0x0F) | 0x70);
         if (new_attrs[i] == 0x77) new_attrs[i] = 0x70;
      }
      _put_attrs(x-1,y+(*ppos),delta_x+2,new_attrs);
      /*_put_attr(x-1,y+(*ppos),delta_x+2,0x70);*/
      ch = toupper(getch());
      if (!ch) ch = -getch();
      _put_attrs(x-1,y+(*ppos),delta_x+2,old_attrs);

      if (ch == Qkey1) return(EDIT);
      if (ch == Qkey2) return(EXIT);

      switch (ch)
      {
         case UP    :
         case LEFT  : (*ppos)--;
                      break;
         case DOWN  :
         case RIGHT : (*ppos)++;
                      break;
         case PGUP  :
         case HOME  : (*ppos) = 0;
                      break;
         case PGDN  :
         case END   : (*ppos) = prms->number_of_options-1;
                      break;
         case ENTER : return(*ppos);
                      break;
         default    : for (i=0;i<prms->number_of_options;i++)
                         if (ch == opts[i].hotkey)
                         {
                            (*ppos) = i;
                            return(*ppos);
                         }
      }
   }
   return(0);
}

int menu_do(menu_parms *prms, menu_options *opts, int Qkey1, int Qkey2, int *ppos, int repaint)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   if (repaint) menu_paint(prms, opts);
   return(menu_choice(prms, opts, Qkey1, Qkey2, ppos));
}

void create_batch_file(int choice)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   FILE *batfile;

   batfile = fopen(batfilename,"wb");
   if (batfile == NULL) abend("opening before create",batfilename);
   if ((_osmajor > 3) || (((_osmajor == 3))&&((_osminor == 30))) )
      fputc('@',batfile);
   fprintf(batfile,"ECHO OFF\r\n");

   fprintf(batfile,"%s %s %s %d\r\n",exefilename, batfilename, datafilename, menu_pos);

   if (choice == ABEND)
   {
      fprintf(batfile,"echo ABEND : %s\r\n",exefilename);
   }
   else if (choice == EXIT)
   {
      fprintf(batfile,"ECHO \b\r\n");
      fprintf(batfile,"ECHO Copyright (c) Jonathan Gilmore, Computer Lab 1991\r\n");
      fprintf(batfile,"VER\r\n");
   }
   else
   {
      if (mnu_options[choice].command[0][0]) fprintf(batfile,"%s\r\n",mnu_options[choice].command[0]);
      if (mnu_options[choice].command[1][0]) fprintf(batfile,"%s\r\n",mnu_options[choice].command[1]);
      if (mnu_options[choice].command[2][0]) fprintf(batfile,"%s\r\n",mnu_options[choice].command[2]);
      if (mnu_options[choice].command[3][0]) fprintf(batfile,"%s\r\n",mnu_options[choice].command[3]);
      if (mnu_options[choice].command[4][0]) fprintf(batfile,"%s\r\n",mnu_options[choice].command[4]);

      if (mnu_options[choice].return_to_menu)
      {
         fprintf(batfile,"%s\r\n",batfilename);
      }
   }
   fclose(batfile);
}

char *pic(int ch, int length)
{
   static char picture[81];
   memset(picture,ch,length);
   picture[length] = '\0';
   return(picture);
}

int field_zstr(int x, int y, char *prompt, char *picture, char *zstr)
{
   int last_char;
   _put_text(x,y,0x1F,prompt);
   gotoxy(x+strlen(prompt),y);
   last_char = edline(zstr,picture,0x70);
   _put_field(x+strlen(prompt),y,0x17,0x17,strlen(picture),zstr);
   return(last_char);
}

int field_char(int x, int y, char *prompt, char *picture, char *ch)
{
   int last_char;
   char zstr[2];

   zstr[0] = *ch;
   zstr[1] = '\0';
   _put_text(x,y,0x1F,prompt);
   gotoxy(x+strlen(prompt),y);
   last_char = edline(zstr,picture,0x70);
   _put_field(x+strlen(prompt),y,0x17,0x17,strlen(picture),zstr);
   *ch = zstr[0];
   return(last_char);
}

int field_int(int x, int y, char *prompt, char *picture, int *value)
{
   int last_char;
   char zstr[21];

   itoa(*value,zstr,10);

   _put_text(x,y,0x1F,prompt);
   gotoxy(x+strlen(prompt),y);
   last_char = edline(zstr,picture,0x70);
   _put_field(x+strlen(prompt),y,0x17,0x17,strlen(picture),zstr);
   *value = atoi(zstr);

   return(last_char);
}

int field_hex(int x, int y, char *prompt, char *picture, char *value)
{
   int last_char,tempint;
   char zstr[21];

   tempint = *value;
   sprintf(zstr,"%2.2X",tempint);

   _put_text(x,y,0x1F,prompt);
   gotoxy(x+strlen(prompt),y);
   last_char = edline(zstr,picture,0x70);
   _put_field(x+strlen(prompt),y,0x17,0x17,strlen(picture),zstr);

   sscanf(zstr,"%x",&tempint);
   *value = (char)tempint;

   return(last_char);
}


void edit_option(int choice)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   #define EDITING    1
   #define PROCEEDING 2
   #define CANCELLING 3

   int last_char;
   int item,mode;
   char far *save_screen;

   save_screen = pushwindow(1,1,80,25);
   clrscr(0x07);
   _put_char(1,1,2000,'±');
   clrwin(5,5,75,20,0x17);
   box(5,5,75,20,0x1F,1);
   _put_string(7,5,"OPTION PARAMETERS");
   shadow(5,5,75,20,0x08,1);

   mode = EDITING;
   item = 1;
   while (mode==EDITING)
   {
      if (item<1) item = 9;
      if (item>9) item = 1;
      switch (item)
      {
         case 1 : last_char = field_zstr(7, 8,"   Description : ",pic('X',40), mnu_options[choice].description   ); break;
         case 2 : last_char = field_char(7, 9,"        Hotkey : ",pic('X', 1),(char *)&mnu_options[choice].hotkey); break;
         case 3 : last_char = field_int (7,10,"        Hidden : ",pic('9', 1),&mnu_options[choice].hidden        ); break;
         case 4 : last_char = field_int (7,11,"Return to menu : ",pic('9', 1),&mnu_options[choice].return_to_menu); break;
         case 5 : last_char = field_zstr(7,13,"      Commands : ",pic('X',40), mnu_options[choice].command[0]    ); break;
         case 6 : last_char = field_zstr(7,14,"                 ",pic('X',40), mnu_options[choice].command[1]    ); break;
         case 7 : last_char = field_zstr(7,15,"                 ",pic('X',40), mnu_options[choice].command[2]    ); break;
         case 8 : last_char = field_zstr(7,16,"                 ",pic('X',40), mnu_options[choice].command[3]    ); break;
         case 9 : last_char = field_zstr(7,17,"                 ",pic('X',40), mnu_options[choice].command[4]    ); break;
      }
      switch (last_char)
      {
         case UP     : item--;      break;
         case DOWN   : item++;      break;
         case BACKTAB: item--;      break;
         case TAB    : item++;      break;
         case PGUP   : item = 1;    break;
         case PGDN   : item = 9;    break;
         case ENTER  : mode = PROCEEDING; break;
         case ESCAPE : mode = CANCELLING; break;
         default     : item++;      break;
      }
   }
   popwindow(save_screen,1,1,80,25);
}

void delete_option(int choice)
{
   int i,j;

   for (i=0;i<mnu_parms.number_of_options;i++)
      tmp_options[i] = mnu_options[i];

   for (i=0,j=0;i<mnu_parms.number_of_options;i++)
      if (i!=choice)
      {
         mnu_options[j] = tmp_options[i];
         j++;
      }
   mnu_parms.number_of_options = j;
   write_parms();
}

int get_new_colour(char *heading, int oldcolour)
{
   int done;
   int ch;
   int i,j;
   int current;
   char buf[11];
   window_t win;

   current = oldcolour;

   win.scrnbufptr    = NULL;
   win.x1            = 13;
   win.y1            = 3;
   win.x2            = 63;
   win.y2            = 22;
   win.window_colour = 0x17;
   win.box_colour    = 0x1F;
   win.box_type      = 1;
   win.shadow_colour = 0x08;
   win.shadow_type   = 1;
   window_up(&win);
   _put_string(win.x1+2,win.y1,heading);

   for (i=0x00;i<=0x0F;i++)
      for (j=0x00;j<=0x0F;j++)
      {
         sprintf(buf,"%2.2X",i*0x10+j);
         _put_text(win.x1+2+j*3,win.y1+2+i,i*0x10+j,buf);
      }

   done = 0;

   while (!done)
   {
      if (current > 0xFF) current = 0xFF;
      if (current < 0x00) current = 0x00;

      _put_text(win.x1+1+(current%0x10)*3  ,win.y1+2+(current/0x10),0x1F,"\x10");
      _put_text(win.x1+1+(current%0x10)*3+3,win.y1+2+(current/0x10),0x1F,"\x11");
       ch = toupper(getch());
       if (!ch) ch = -getch();
      _put_text(win.x1+1+(current%0x10)*3  ,win.y1+2+(current/0x10),0x17," ");
      _put_text(win.x1+1+(current%0x10)*3+3,win.y1+2+(current/0x10),0x17," ");

      switch(ch)
      {
         case PGUP   : current &= 0x0F;                      break;
         case PGDN   : current |= 0xF0;                      break;
         case HOME   : current &= 0xF0;                      break;
         case END    : current |= 0x0F;                      break;
         case UP     : if ((current/0x10) >= 0x01) current -= 0x10; break;
         case DOWN   : if ((current/0x10) <  0x0F) current += 0x10; break;
         case LEFT   : if ((current%0x10) >= 0x01) current -= 0x01; break;
         case RIGHT  : if ((current%0x10) <  0x0F) current += 0x01; break;
         case ESCAPE : done = 1;                             break;
         case ENTER  : done = 2;                             break;
      }
   }

   window_dn(&win);

   if (done == 2)
      return(current);
   else
      return(CANCEL);
}
