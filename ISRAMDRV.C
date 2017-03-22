
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
      if (drive==testdrive)
      {
         char path[21];
         struct find_t finfo;
         unsigned int notfound;

         //strcpy(path,"*.*");
         strcpy(path,"?:\\MS-RAMDR.IVE");
         path[0] = argv[1][0];
         if ( argc == 2 )
            printf("Checking drive %c: for volume label \"MS-RAMDRIVE\"\n",argv[1][0]);
         notfound = _dos_findfirst(path,_A_VOLID,&finfo);
         if (!notfound)
         {
            if ( memcmp("MS-RAMDRIVE",finfo.name,11) == 0 )
            {
               retcode = 0;
               if ( argc == 2 )
                  printf("Drive %c: is a MS-RAMDRIVE\n",argv[1][0]);
            }
         }
      }
      _dos_setdrive( origdrive, &drivecount );
   }
   return retcode;
}
