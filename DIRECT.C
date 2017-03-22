// To build:
//    cl direct.c c:\msvc\lib\setargv.obj /link /noe

#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

struct _stat filestat;
int ok=0;
char full[_MAX_PATH];
struct tm *ptm;
/*
  struct tm
  {
      int tm_sec;    // Seconds after the minute - [0,59]
      int tm_min;    // Minutes after the hour - [0,59]
      int tm_hour;   // Hours since midnight - [0,23]
      int tm_mday;   // Day of the month - [1,31]
      int tm_mon;    // Months since January - [0,11]
      int tm_year;   // Years since 1900
      int tm_wday;   // Days since Sunday - [0,6]
      int tm_yday;   // Days since January 1 - [0,365]
      int tm_isdst;  // Daylight-saving-time flag
  };
*/


int main ( int argc, char *argv[] )
{
   int i;
   char ascdate[27];

   if (argc<2) return 1;

   for (i=1;i<argc;i++)
   {
      ok=0;
      if( !_stat( argv[i], &filestat ) )
      {
         if (filestat.st_mode & _S_IFDIR)
            ok = 1;
         else

         if ( _fullpath( full, argv[i], _MAX_PATH ) != NULL )
         {
            strcpy (ascdate, ctime( &filestat.st_atime ));
            ascdate[3] = 0; // Just day of the week

            ptm = localtime( &filestat.st_atime );
            printf( "%4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d %s %10ld %s\n",
                                   ptm->tm_year+1900,
                                   ptm->tm_mon+1,
                                   ptm->tm_mday,
                                   ptm->tm_hour,
                                   ptm->tm_min,
                                   ptm->tm_sec,
                                   ascdate,
                                   filestat.st_size,
                                   strlwr(full)
                                   );
            ok=1;
         }

      }
      if (!ok)
         printf("*************NOT FOUND************ %s\n",argv[i]);
   }
   return 0;
}
