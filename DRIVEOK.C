
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

#include <tools.h>

int _cdecl main ( int argc, char *argv[] )
{
   unsigned origdrive, drive, testdrive, drivecount;
   int retcode = 1;
   if (argc > 1)
   {
      _dos_getdrive( &origdrive );

      drive = toupper(argv[1][0]) - 'A' + 1;

      _dos_setdrive( drive, &drivecount );
      _dos_getdrive( &testdrive );
      if (drive==testdrive) retcode = 0;

      _dos_setdrive( origdrive, &drivecount );
   }
   return(retcode);
}
