#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <dos.h>
#include <bios.h>

#include <tools.h>

FILE *file;
char str[2001];
int x = 81;
long unsigned int count = 0;
char _far diskbuf[32768];

void header(void);
void footer(void);
void closefile(void);
void reset_cursor(void);
void display_file(char *filename, int textmode);
void display_disk( unsigned int drive, unsigned int head, unsigned int track, unsigned int sector, unsigned int nsectors);

void main(int argc, char *argv[])
{
   int textmode= 0;
   char filename[129];

   if (argc<2) exit(1);

   strupr(argv[1]);
   if (argc>=3) strupr(argv[2]);

   strcpy(filename,argv[1]);

   if (!strcmp(filename,"DISK"))
   {
      if (argc>=3)
         display_disk(argv[2][0],0,0,1,9);
   }
   else
   {
      if (argc>=3) textmode = (argv[2][1] == 'T');
      display_file(filename,textmode);
   }
}

void display_file(char *filename, int textmode)
{
   int ch;
   int ki;
   int CR = 0;
   int LF = 0;

   file = fopen(filename,"rb");
   atexit(closefile);
   atexit(reset_cursor);
   header();
   while (!feof(file))
   {
      ch = fgetc(file);

      if (!feof(file))
      {
         if (ch==0x00)
         {
            _put_char(x,1,1,0xFE);
            _put_attr(x,1,1,0x12);
         }
         else if (ch==0xFF)
         {
            _put_char(x,1,1,0xFE);
            _put_attr(x,1,1,0x14);
         }
         else
         {
            _put_char(x,1,1,ch);
            _put_attr(x,1,1,0x17);
         }
         x++;
         count++;

         if (textmode)
         {
            LF = (ch==0x0A);
            if (CR && LF)
            {
               if (((x-1)%80) == 0)
                  x = (x/80)*80+1;
               else
                  x = (x/80+1)*80+1;
               CR = LF = 0;
            }
            CR = (ch==0x0D);
         }

         if (x>1920)
         {
            footer();
            _put_text(1,25,0x70,"                            Press any key to continue                           ");
            ki = getch();
            _put_text(1,25,0x07,"                                                                                ");
            if (ki == 0x1B)
            {
               x = 22*80+1;
               exit(1);
            }
            header();
         }
      }
   }
   footer();
}

void display_disk( unsigned int drive, unsigned int head, unsigned int track, unsigned int sector, unsigned int nsectors)
{
   int ki;
    unsigned status = 0,i;
    struct diskinfo_t di;
    unsigned char _far *p;

    if( (di.drive = toupper(drive) - 'A' ) > 1 ) /* 0,1,2,3 */
       di.drive = di.drive - 2 + 0x80;           /* 80,81   */

    di.head     = head;
    di.track    = track;
    di.sector   = sector;
    di.nsectors = nsectors;
    di.buffer   = diskbuf;

    for( i = 0; i < 3; i++ )
    {
        status = _bios_disk( _DISK_READ, &di ) >> 8;
        if( !status ) break;
    }

    if( status )
        printf( "Disk Read Error: 0x%.2x\n", status );
    else
    {
        atexit(reset_cursor);
        header();
        for( p = diskbuf; p < (diskbuf + (512 * nsectors)); p++ )
        {
           if (*p==0x00)
           {
              _put_char(x,1,1,0xFE);
              _put_attr(x,1,1,0x12);
           }
           else if (*p==0xFF)
           {
              _put_char(x,1,1,0xFE);
              _put_attr(x,1,1,0x14);
           }
           else
           {
              _put_char(x,1,1,*p);
              _put_attr(x,1,1,0x17);
           }
           x++;
           count++;

           if (x>1920)
           {
              footer();
              _put_text(1,25,0x70,"                            Press any key to continue                           ");
              ki = getch();
              _put_text(1,25,0x07,"                                                                                ");
              if (ki == 0x1B)
              {
                 x = 22*80+1;
                 exit(1);
              }
              header();
           }
        }
        footer();
    }

    exit(0);
}

void closefile(void)
{
   fclose(file);
}

void reset_cursor(void)
{
   gotoxy(1,(x/80+2));
}

void header(void)
{
   char countstr[33];
   clrscr(0x07);
   gotoxy(1,26);
   _put_text(1,1,0x70,"DF - Dump File (c) 1991 Computer Lab                                            ");
   sprintf(countstr,"%10lu - ",count+1);
   _put_text(56,1,0x70,countstr);
   x = 81;
}

void footer(void)
{
   char countstr[33];
   sprintf(countstr,"%10lu",count);
   _put_text(69,1,0x70,countstr);
}
