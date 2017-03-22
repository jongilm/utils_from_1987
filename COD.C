#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <io.h>
#include <search.h>
#include <direct.h>
#include <sys\types.h>
#include <sys\utime.h>
#include <sys\stat.h>

#include <dodtype.h>
#include <biosdir.h>
#include <dosfile.h>

#define TRUE 1
#define FALSE 0

extern void compare_directories(char *spec0, char *spec1);

/* Prototypes */
void checkerc(int location);

static void make_paths(char *arg0,char *arg1);
static void GetPath ( int drive, struct path_t *path );
static void SetPath ( struct path_t *path );
static int scan( char *name, int (*dirfunction)(void));
static int dirfunction(void);

/* Structures */
struct dir_t
{
   char     name[61];
   char     found_in_1;
   char     found_in_2;
};

struct path_t
{
   int  drive;
   char directory[_MAX_PATH];
};
/* Variables */

int  erc = 0;
char errstr[257];
int f_dont_print_Ss  = 0;
int f_dont_print_Ds  = 0;
int f_dont_print_1s  = 0;
int f_dont_print_2s  = 0;
int f_scan_subdirs   = 0;
int f_raw_format     = 0;
int f_quiet          = 0;
int f_verbose        = 0;
int f_quick          = 0;
char *f_prefix       = NULL;

static struct path_t Initial[3];
int                  Initial1Saved = 0;
int                  Initial2Saved = 0;

static char          origpath[2][_MAX_PATH];
static char          origspec[2][_MAX_PATH];
static char          temppath[2][_MAX_PATH];
static char          tempspec[2][_MAX_PATH];

static struct dir_t  savedir[1024];
static int           activeone    = 0;
static unsigned int  dirs_saved   = 0;
static int           dir_count[2] = {0,0};

void main(int argc,char *argv[])
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   char          arg[2][_MAX_PATH];
   int i;
   unsigned int j;
   unsigned numdrives;
   int paths = 0;
   char tempstr[_MAX_PATH];

   if (!f_quiet)
   {
      fprintf(stderr,"Compare Directory  Version 2.02\n");
      fprintf(stderr,"Copyright (C) Softnet Computer Services (Pty) Ltd. 1990.  All rights reserved.\n");
   }
   for (i=1;i<argc;i++)
   {
      if (argv[i][0] == '-' || argv[i][0] == '/')
         switch toupper(argv[i][1])
         {
            case 'X' : switch (toupper(argv[i][2]))
                       {
                          case 'S' : f_dont_print_Ss  = 1; break;
                          case 'D' : f_dont_print_Ds  = 1; break;
                          case '1' : f_dont_print_1s  = 1; break;
                          case '2' : f_dont_print_2s  = 1; break;
                          default  : printf("Invalid /X option %s - ignored\n",argv[i]);
                       }
                       break;
            case 'R' : switch (toupper(argv[i][2]))
                       {
                          case 'N' :
                          case 'O' :
                          case '1' :
                          case '2' : f_raw_format = toupper(argv[i][2]);
                                     break;
                          default  : printf("Invalid /R option %s - ignored\n",argv[i]);
                       }
                       break;
            case 'P' : f_prefix         = argv[i]+2;
                       break;
            case 'S' : f_scan_subdirs   = 1;
                       break;
            case 'V' : f_verbose = 1;
                       break;
            case 'Q' : f_quick = 1;
                       break;
            case 'N' : f_quiet = 1;
                       break;
            default  : printf("Invalid switch %s - ignored\n",argv[i]);
         }
      else
         if (paths==0)
         {
            strcpy(arg[0],argv[i]);
            paths++;
         }
         else if (paths==1)
         {
            strcpy(arg[1],argv[i]);
            paths++;
         }
   }

   if (paths==1)
   {
      strcpy(arg[1],".");
      paths++;
   }

   if (paths != 2)  {erc = 1;   checkerc(1);}

   GetPath(' ', &Initial[0]);

   if (arg[0][1] == ':')
   {
      Initial1Saved = 1;
      GetPath(arg[0][0],&Initial[1]);
   }

   if (arg[1][1] == ':')
   {
      Initial2Saved = 1;
      GetPath(arg[1][0],&Initial[2]);
   }

   if (!f_quiet)
      if (!f_raw_format)
         printf("Args  = %s and %s\n",arg[0],arg[1]);
      else
         fprintf(stderr,"Args  = %s and %s\n",arg[0],arg[1]);

   make_paths(arg[0],arg[1]); checkerc(2);
   strcpy(origpath[0],temppath[0]);
   strcpy(origpath[1],temppath[1]);
   strcpy(origspec[0],tempspec[0]);
   strcpy(origspec[1],tempspec[1]);

   if (!f_quiet)
      if (!f_raw_format)
      {
         printf("Spec1 = %s\n",origspec[0]);
         printf("Spec2 = %s\n",origspec[1]);
      }
      else
      {
         fprintf(stderr,"Spec1 = %s\n",origspec[0]);
         fprintf(stderr,"Spec2 = %s\n",origspec[1]);
      }

   if (f_scan_subdirs)
   {
      char fname [2][_MAX_FNAME];
      char ext   [2][_MAX_EXT];

      for (activeone=0;activeone<2;activeone++)
      {
         char drive [_MAX_DRIVE];
         char dir   [_MAX_DIR];
         struct path_t Now;

         GetPath(' ',&Now);
         _splitpath( origspec[activeone], drive, dir, fname[activeone], ext[activeone]);
         _dos_setdrive(drive[0]-'A'+1,&numdrives);
         strcpy(tempstr,origpath[activeone]);
         if (strlen(tempstr) > 3) tempstr[strlen(tempstr)-1] = '\0';
         if (!f_quiet)
            fprintf(stderr,"Scanning : %s\n",tempstr);
         scan(tempstr,dirfunction);
         SetPath(&Now);
      }

   if (!f_quiet)
      if (f_raw_format)
      {
         fprintf(stderr,"\n");
         fprintf(stderr,"    Comparing Spec1 : %5d dirs in %s\n",dir_count[0],origpath[0]);
         fprintf(stderr,"           to Spec2 : %5d dirs in %s\n",dir_count[1],origpath[1]);
         fprintf(stderr,"\n");
      }
      else
      {
         printf("\n");
         printf("    Comparing Spec1 : %5d dirs in %s\n",dir_count[0],origpath[0]);
         printf("           to Spec2 : %5d dirs in %s\n",dir_count[1],origpath[1]);
/*
         fprint("         Total dirs : %5d\n",dir_count[0]+dir_count[1]);
         fprint("     Total dirnames : %5d\n",dirs_saved);
         fprint("    Common dirnames : %5d\n",dir_count[0]+dir_count[1]-dirs_saved);
*/
         printf("\n");
      }

      for (j=0;j<dirs_saved;j++)
      {
         strcpy(arg[0],origpath[0]);
         arg[0][strlen(arg[0])-1] = '\0';
         strcat(arg[0],savedir[j].name);
         strcat(arg[0],"\\");
         strcat(arg[0],fname[0]);
         strcat(arg[0],ext[0]);

         strcpy(arg[1],origpath[1]);
         arg[1][strlen(arg[1])-1] = '\0';
         strcat(arg[1],savedir[j].name);
         strcat(arg[1],"\\");
         strcat(arg[1],fname[1]);
         strcat(arg[1],ext[1]);

/*         printf("Makepaths %-30s and %-30s\n",arg[0], arg[1]);*/
         make_paths(arg[0],arg[1]); checkerc(3);
/*         printf("Comparing %-30s and %-30s\n",tempspec[0], tempspec[1]);*/
         compare_directories(tempspec[0], tempspec[1]);
      }
   }
   else
   {
      compare_directories(origspec[0], origspec[1]);
   }
   if (Initial1Saved)
      chdir(Initial[2].directory);
   if (Initial2Saved)
      chdir(Initial[1].directory);
   SetPath(&Initial[0]);
}

void checkerc ( int location )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   if (erc)
      fprintf(stderr,"COD Error : LOC=%d,ERC=%d\n",location,erc);

   switch (erc)
   {
      case  0 : return;
      case  1 : fprintf(stderr,"SYNTAX: COD <wildfilespec1> [<wildfilespec2>] [switches]\n");
                fprintf(stderr,"switches : /Xn = eXclude files, where n is\n");
                fprintf(stderr,"                 S = files that are the same\n");
                fprintf(stderr,"                 D = files that are the different\n");
                fprintf(stderr,"                 1 = files in Path 1 only\n");
                fprintf(stderr,"                 2 = files in Path 2 only\n");
                fprintf(stderr,"           /Rn = output in Raw format where n is\n");
                fprintf(stderr,"                 N = display only NEWER differing files\n");
                fprintf(stderr,"                 O = display only OLDER differing files\n");
                fprintf(stderr,"                 1 = display only files in path 1\n");
                fprintf(stderr,"                 2 = display only files in path 2\n");
                fprintf(stderr,"           /P = Prefix raw format output with <command>\n");
                fprintf(stderr,"           /S = Scan sub-directories\n");
                fprintf(stderr,"           /V = Verbose listing\n");
                fprintf(stderr,"           /N = No headings\n");
                break;
      case  2 : fprintf(stderr,"ERROR: No files found: %s\n", errstr);        break;
      case  3 : fprintf(stderr,"ERROR: Invalid filespec: \"%s\"\n", errstr);   break;
      case  4 : fprintf(stderr,"ERROR: %s\n",_strerror(NULL));                   break;
      case  5 : fprintf(stderr,"ERROR: %s %s\n",_strerror(NULL),errstr);                   break;
      case  6 : fprintf(stderr,"ERROR: Error reading %s\n",errstr);                   break;
      default : fprintf(stderr,"ERROR: Unknown error code %d\n",erc);
   }
   if (Initial1Saved)
      chdir(Initial[2].directory);
   if (Initial2Saved)
      chdir(Initial[1].directory);
   SetPath(&Initial[0]);
   exit(1);
}

static void GetPath ( int drive, struct path_t *path )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   drive = toupper(drive);
   if (drive == ' ')
   {
      GetDefaultDrive((GPCHAR)&path->drive);
      getcwd(path->directory,_MAX_PATH);
   }
   else
   {
      _getdcwd(drive-'A'+1,path->directory,_MAX_PATH);
   }
}

static void SetPath ( struct path_t *path )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   SetDefaultDrive((BYTE)path->drive);
   chdir(path->directory);
}

static void make_spec(char *specin, char *specout)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   struct find_t find;
   char fullspec[_MAX_PATH];
  // unsigned attrib = _A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH | _A_SUBDIR | _A_VOLID;
   unsigned attrib = _A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH;

   strupr(specin);
   if( _fullpath( fullspec, specin, _MAX_PATH ) == NULL ) { erc = 3; strcpy(errstr,specin); return; }

   strcpy(specout,fullspec);
   if (!_dos_findfirst(specout, attrib, &find)) return;

   strcpy(specout,fullspec);
   strcat(specout,"\\*.*");
   if (!_dos_findfirst(specout, attrib, &find)) return;

   strcpy(specout,fullspec);
   strcat(specout,"*.*");
   if (!_dos_findfirst(specout, attrib, &find)) return;

   strcpy(specout,fullspec);
}

static void make_path(char *specin, char *pathout)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   char drive [_MAX_DRIVE];
   char dir   [_MAX_DIR];
   char fname [_MAX_FNAME];
   char ext   [_MAX_EXT];

   _splitpath( specin , drive, dir, fname, ext );
   _makepath ( pathout, drive, dir, ""   , ""  );
}

static void make_paths(char *arg0,char *arg1)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   char drive [_MAX_DRIVE];
   char dir   [_MAX_DIR];
   char fname [2][_MAX_FNAME];
   char ext   [2][_MAX_EXT];

   make_spec(arg0,tempspec[0]);
   make_path(tempspec[0],temppath[0]);

   make_spec(arg1,tempspec[1]);
   make_path(tempspec[1],temppath[1]);

   _splitpath( tempspec[0], drive, dir, fname[0], ext[0] );
   _splitpath( tempspec[1], drive, dir, fname[1], ext[1] );
   if (fname[1][0]=='*') strcpy(fname[1],fname[0]);
   if (ext  [1][1]=='*') strcpy(ext  [1],ext  [0]);
   _makepath( tempspec[1], drive, dir, fname[1], ext[1] );
}

static int scan( char *name, int (*dirfunction)(void))
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   struct find_t dir;

   chdir(name);
   if (dirfunction != NULL) if ((dirfunction)()) return(TRUE);

   if( !FindFirstFile( (unsigned int)_A_SUBDIR,"*.*",3,  (unsigned char *)&dir ) )
   {
      if ((dir.name[0] != '.') && (dir.attrib & 0x10) )
         scan(dir.name,dirfunction);

      while( !FindNextFile( (unsigned char *)&dir ) )
      {
         if ((dir.name[0] != '.') && (dir.attrib & 0x10) )
            scan(dir.name,dirfunction);
      }
   }

   chdir("..");
   return(FALSE);
}

int cmped(const void *pkey, const void *ptableentry)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   const struct dir_t *key = pkey;
   const struct dir_t *tableentry = ptableentry;

   return(strcmp(key->name,tableentry->name));
}

static int dirfunction(void)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   char dir[_MAX_PATH];
   int ndx;

   char *result;
   struct dir_t temp;
   unsigned int temp_dirs_saved = 0;

   getcwd(dir,_MAX_PATH);
   strlwr(dir);

   strcpy(temp.name,dir+strlen(temppath[activeone])-1);
   if (temp.name[strlen(temp.name)-1] == '\\') strcat(temp.name,".");
   else                                        strcat(temp.name,"\\.");

  /* printf("%s\n",temp.name);*/

   if (activeone == 0)
   {
      temp.found_in_1 = 1;
      temp.found_in_2 = 0;
   }
   else
   {
      temp.found_in_1 = 0;
      temp.found_in_2 = 1;
   }

   temp_dirs_saved = dirs_saved;

   result = lsearch((const void*)&temp,(void*)savedir,&dirs_saved,(unsigned)sizeof(struct dir_t),cmped);

   ndx = ((result - (char*)savedir)/sizeof(struct dir_t));

   if (temp_dirs_saved == dirs_saved)
   {
      /*printf("Found at %3.3d - %s\n",ndx,savedir[ndx].name);*/
      if (activeone == 0)
         savedir[ndx].found_in_1 = 1;
      else
         savedir[ndx].found_in_2 = 1;
   }
 /*  else
      printf("Inserted at %3.3d - %s\n",ndx,savedir[ndx].name);*/

   dir_count[activeone]++;

   return(0);
}
