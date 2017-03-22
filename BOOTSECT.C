#include <stdio.h>
#include <conio.h>
#include <bios.h>
#include <dos.h>
#include <stdlib.h>
#include <ctype.h>

char _far diskbuf[32768];

void main( int argc, char *argv[] )
{
    unsigned status = 0,i;
    struct diskinfo_t di;
    unsigned char _far *p;

    if( argc == 1 )
        di.drive = 0x80;
    else
    {
       if( (di.drive = toupper( argv[1][0] ) - 'A' ) > 1 ) /* 0,1,2,3 */
          di.drive = di.drive - 2 + 0x80;                  /* 80,81   */
    }

    di.head     = 0;
    di.track    = 0;
    di.sector   = 1;
    di.nsectors = 1;
    di.buffer   = diskbuf;

    for( i = 0; i < 3; i++ )
    {
        status = _bios_disk( _DISK_READ, &di ) >> 8;
        if( !status ) break;
    }

    if( status )
    {
        char str[33];
        cputs("Error : ");
        cputs(itoa(status,str,10));
    }
    else
    {
        for( p = diskbuf; p < (diskbuf + 512); p++ )
           if (*p!=7) putch(*p);
           else       putch('.');
    }
}
