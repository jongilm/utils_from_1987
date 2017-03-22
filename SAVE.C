#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include <dos.h>
#include <direct.h>

#include <tools.h>

char far     *scrptr;
int          scrx       = 0;
int          scry       = 0;
int          retcode    = 0;
char         progname[_MAX_PATH];
char         **progargs;
int          change_dir = 0;
int          save_scr   = 0;
int          save_dir   = 0;
int          arg        = 0;
unsigned int origdrive;
unsigned int numdrives;
char         origpath[_MAX_PATH];
char         drive[_MAX_DRIVE];
char         dir[_MAX_DIR];
char         fname[_MAX_FNAME];
char         ext[_MAX_EXT];
char         fullpath[_MAX_PATH];


void usage(int retcode)
{
   printf("Usage : SAVE [/C] [/D] [/S] [<drive>:][<path>]<ProgramName> [<Parameters>...]\n");
   printf("Where : /C = Change drive/directory to that of ProgramName\n");
   printf("Where : /D = Save current Drive/directory\n");
   printf("Where : /S = Save current Screen/cursor position\n");
   exit(retcode);
}

int main(int argc, char *argv[])
{
   progname[0] = '\0';

   for(arg=1;arg<argc;arg++)
   {
      if ((argv[arg][0] == '/') || (argv[arg][0] == '-'))
      {
         strupr(argv[arg]);
         if      (argv[arg][1] == 'C') change_dir = 1;
         else if (argv[arg][1] == 'S') save_scr   = 1;
         else if (argv[arg][1] == 'D') save_dir   = 1;
         else
         {
            printf("\nERROR : Parameter %d - \"%s\" - switch invalid\n",arg,argv[arg]);
            usage(-1);
         }
      }
      else
      {
         if (progname[0] == '\0')
         {
            strcpy(progname,argv[arg]);
            progargs = &argv[arg];
            break;
         }
      }
   }

   if (progname[0] == '\0')
   {
      printf("\nERROR : ProgramName is mandatory\n");
      usage(-3);
   }

   if (save_scr)
   {
      scrptr = pushscreen();
      scrx   = wherex();
      scry   = wherey();
   }

   if (save_dir)
   {
      _dos_getdrive(&origdrive);
      getcwd(origpath,_MAX_PATH);
   }

   if (change_dir)
   {
      if( _fullpath( fullpath, progname, _MAX_PATH ) != NULL )
      {
         _splitpath( fullpath, drive, dir, fname, ext );
         if (strlen(dir) > 1) dir[strlen(dir)-1] = '\0';
         _dos_setdrive(toupper(drive[0])-0x40,&numdrives);
         chdir(dir);
      }
   }

   retcode = spawnvp(P_WAIT,progname,progargs);

   if (save_dir)
   {
      _dos_setdrive(origdrive,&numdrives);
      chdir(origpath);
   }

   if (save_scr)
   {
      gotoxy(scrx,scry);
      popscreen(scrptr);
   }
   return (retcode);
}
