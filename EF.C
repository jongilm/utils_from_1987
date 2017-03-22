#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
#include <fcntl.h>

#include <tools.h>

#define ESC    0x1B
#define PGUP  -0x49
#define PGDN  -0x51
#define UP    -0x48
#define DN    -0x50
#define HOME  -0x47
#define END   -0x4F
#define LEFT  -0x4B
#define RIGHT -0x4D

void error(int errno);

#define NUMBER_X     0
#define NUMBER_Y     0

#define ASCII_X      NUMBER_X + 5 + 3
#define ASCII_Y      0
#define ASCII_WIDTH  16
#define ASCII_LENGTH 24

#define HEX_X        ASCII_X + ASCII_WIDTH + 3
#define HEX_Y        0

#define BUFFER_SIZE  ASCII_WIDTH*ASCII_LENGTH

static FILE *file;
static char buffer[BUFFER_SIZE];
static size_t count;
static long location,prev_location,max_location,current_location;
static long file_size;
static int ch,x,y,ndx;
static int curx,cury;
static int finished;

void main(int argc,char **argv)
{
   int handle;
   char tempstr[81];
   int extra;

   clrscr(0x17);
   gotoxy(1,1);
   if (argc < 2) error(1);

   /* get file_size */
   if ((handle = open(argv[1],O_RDONLY)) == -1) error(3);
   file_size = filelength(handle);
   close(handle);

   _put_attr(1,25,80,0x70);
   sprintf(tempstr,"%s (%ld bytes)",argv[1],file_size);
   _put_string(1,25,tempstr);
   sprintf(tempstr,"Last byte=%5.5X",file_size-1);
   _put_string(31,25,tempstr);


   if ((file = fopen(argv[1],"r+b")) == NULL) error(3);
   location = 0L;
   prev_location = -1L;
   curx = 0;
   cury = 0;
   finished = 0;
   max_location = (((file_size-BUFFER_SIZE)/ASCII_WIDTH) + ((file_size-BUFFER_SIZE)%ASCII_WIDTH ? 1:0))*ASCII_WIDTH;
   while (!finished)
   {
      if (curx<0)
      {
         if ((cury==0) && (location == 0L))
            curx = 0;
         else
         {
            cury--;
            curx = ASCII_WIDTH-1;
         }
      }
      if (curx>ASCII_WIDTH-1)
      {
         if ((cury==ASCII_LENGTH-1) && (location >= max_location))
            curx = ASCII_WIDTH-1;
         else
         {
            cury++;
            curx = 0;
         }
      }
      if (cury<0)
      {
         location -= ASCII_WIDTH;
         cury = 0;
      }
      if (cury>ASCII_LENGTH-1)
      {
         location += ASCII_WIDTH;
         cury = ASCII_LENGTH-1;
      }

      if (location <= 0)            location = 0;
      if (location >= max_location) location = max_location;

      current_location  = location+cury*ASCII_WIDTH+curx;
      if (current_location > file_size-1)
      {
         current_location = file_size-1;
         curx = (int)(current_location-location)-cury*ASCII_WIDTH;
      }

      sprintf(tempstr,"Current byte=%5.5X     ",current_location);
      _put_string(51,25,tempstr);

      if (location != prev_location)
      {
         if (fseek(file,location,SEEK_SET) != 0) error(4);
         count = fread(buffer,1,BUFFER_SIZE,file);

         ndx = 0;
         for (y=0;y<ASCII_LENGTH;y++)
         {
            sprintf(tempstr,"%5.5X",location+y*ASCII_WIDTH);
            _put_string(NUMBER_X+1,NUMBER_Y+y+1,tempstr);
            extra = 0;
            for (x=0;x<ASCII_WIDTH;x++)
            {
               sprintf(tempstr,"%2.2X",*((unsigned char *)(buffer+ndx)));
               _put_char(ASCII_X+x+1  ,ASCII_Y+y+1,1,(ndx<(int)count) ? *(buffer+ndx):' ');
               if (x==ASCII_WIDTH/2) extra++;
               else if ((x==ASCII_WIDTH/4)||(x==3*(ASCII_WIDTH/4))) extra++;
               _put_chars(HEX_X+  x*3+extra+1,HEX_Y  +y+1,2,(ndx<(int)count) ? tempstr:"  ");
               ndx++;
            }
         }
         prev_location = location;
      }
      gotoxy(ASCII_X+curx+1  ,ASCII_Y+cury+1);

      extra = 0;
      if (curx>=ASCII_WIDTH/4) extra++;
      if (curx>=ASCII_WIDTH/2) extra++;
      if (curx>=3*(ASCII_WIDTH/4)) extra++;
      _put_attr(HEX_X  +curx*3  +extra+1,HEX_Y  +cury+1  ,2, 0x70);

      ch = getch();
      if (!ch) ch = -getch();
      _put_attr(HEX_X  +curx*3  +extra+1,HEX_Y  +cury+1  ,2, 0x17);

      switch(ch)
      {
         case ESC   : finished = 1;
                      break;
         case PGUP  : if (cury==0) location -= BUFFER_SIZE;
                      else         cury=0;
                      break;
         case PGDN  : if (cury==ASCII_LENGTH-1) location += BUFFER_SIZE;
                      else cury=ASCII_LENGTH-1;
                      break;
         case HOME  : if ((curx==0) && (cury==0)) location = 0L;
                      else if (curx==0 )          cury=0;
                      else                        curx=0;
                      break;
         case END   : if ((curx==ASCII_WIDTH-1) && (cury==ASCII_LENGTH-1)) location = file_size-1;
                      else if (curx==ASCII_WIDTH-1) cury=ASCII_LENGTH-1;
                      else                          curx=ASCII_WIDTH-1;
                      break;
         case UP    : cury--;
                      break;
         case DN    : cury++;
                      break;
         case LEFT  : curx--;
                      break;
         case RIGHT : curx++;
                      break;
      }
   }
   fclose(file);
   clrscr(0x07);
   gotoxy(1,1);
}

void error(int errno)
{
   switch(errno)
   {
      case 0: printf("Normal termination\n");      break;
      case 1: printf("USAGE : EF <filename>\n");   break;
      case 2: printf("Error allocating memory\n"); break;
      case 3: printf("Error opening file\n");      break;
      case 4: printf("Error seeking file\n");      break;
   }
   exit(errno);
}
