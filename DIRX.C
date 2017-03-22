/* EXTDIR.C illustrates wildcard handling using functions:
 *      _dos_findfirst      _dos_findnext       sprintf           (DOS-only)
 */

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <io.h>
#include <sys\types.h>
#include <sys\utime.h>
#include <sys\stat.h>

char *timestr( unsigned d, char *buf );
char *datestr( unsigned d, char *buf );

char *timestr( unsigned t, char *buf )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Takes unsigned time in the format:               fedcba9876543210 ³*/
/*³ s=2 sec incr, m=0-59, h=23                       hhhhhmmmmmmsssss ³*/
/*³ Changes to a 9-byte string (ignore seconds):     hh:mm ?m         ³*/
/*³                                                                   ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
    int h = (t >> 11) & 0x1f, m = (t >> 5) & 0x3f;

    sprintf( buf, "%2.2d:%02.2d %cm", h % 12, m,  h > 11 ? 'p' : 'a' );
    return buf;
}

char *datestr( unsigned d, char *buf )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Takes unsigned date in the format:               fedcba9876543210 ³*/
/*³ d=1-31, m=1-12, y=0-119 (1980-2099)              yyyyyyymmmmddddd ³*/
/*³ Changes to a 9-byte string:                      mm/dd/yy         ³*/
/*³                                                                   ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   sprintf( buf, "%2.2d/%02.2d/%02.2d",
            (d >> 5) & 0x0f, d & 0x1f, (d >> 9) + 80 );
   return buf;
}

void fileinfo ( struct _find_t *pfind )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Displays information about a file.               ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
    char timebuf[10], datebuf[10], *pkind;

    //datestr( pfind->wr_date, datebuf );
    //timestr( pfind->wr_time, timebuf );

    if      ( pfind->attrib & _A_SUBDIR ) pkind = "<DIR>";
    else if ( pfind->attrib & _A_VOLID  ) pkind = "<Label>";
    else                                  pkind = " "; //File

    printf( "%-12s %8ld %-9s %c%c%c%c\n",
            pfind->name, pfind->size, pkind,
            (pfind->attrib & _A_RDONLY) ? 'R' : ' ',
            (pfind->attrib & _A_HIDDEN) ? 'H' : ' ',
            (pfind->attrib & _A_SYSTEM) ? 'S' : ' ',
            (pfind->attrib & _A_ARCH)   ? 'A' : ' ' );
    //printf( "%-12s %8ld %8s %8s %-9s %c%c%c%c\n",
    //        pfind->name, pfind->size, timebuf, datebuf, pkind,
    //        (pfind->attrib & _A_RDONLY) ? 'R' : ' ',
    //        (pfind->attrib & _A_HIDDEN) ? 'H' : ' ',
    //        (pfind->attrib & _A_SYSTEM) ? 'S' : ' ',
    //        (pfind->attrib & _A_ARCH)   ? 'A' : ' ' );
}

void main( int argc, char *argv[] )
{
    struct _find_t find;
    long size;

    /* Find first matching file, then find additional matches. */
    if( !_dos_findfirst( argv[1], 0xffff, &find ) )
    {
        printf( "%-12s %-8s %-8s %-8s %-9s %s\n",
                "FILE", "SIZE", "TIME", "DATE", "KIND", "ATTR" );
        fileinfo ( &find );
        size = find.size;
    }
    else
    {
        printf( "  SYNTAX: EXTDIR <wildfilespec>" );
        exit( 1 );
    }
    while( !_dos_findnext( &find ) )
    {
        fileinfo ( &find );
        size += find.size;
    }
    printf( "%-12s   %8ld\n", "Total", size );
    exit( 0 );
}
