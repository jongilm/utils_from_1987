#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <search.h>

/* Externals */
extern int erc;
extern char errstr[257];
extern void checkerc(int location);

extern int   f_dont_print_Ss;
extern int   f_dont_print_Ds;
extern int   f_dont_print_1s;
extern int   f_dont_print_2s;
extern int   f_scan_subdirs;
extern int   f_raw_format;
extern int   f_verbose;
extern int   f_quick;
extern int   f_quiet;
extern char *f_prefix;


/* Structures */
struct file_t{
   char     name[13];

   char     found_in_1;
   long     size1;
   unsigned wr_date1;
   unsigned wr_time1;
   char     attrib1;

   char     found_in_2;
   long     size2;
   unsigned wr_date2;
   unsigned wr_time2;
   char     attrib2;

   char     compare_ok;
};


/* Variables */
struct file_t savefile[1024];
unsigned int files_saved = 0;
int file_count[2] = {0,0};

static char spec  [2][_MAX_PATH];
static char path  [2][_MAX_PATH];

static int MustPrintHeader = 0;

static int cmpef(const struct file_t *key, const struct file_t *tableentry)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
    return(strcmp(key->name,tableentry->name));
}

static void save_info(struct find_t *pfind,int which)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   char *result;
   struct file_t temp;
   unsigned int temp_files_saved = 0;
   int ndx;

   strlwr(pfind->name);
   strcpy(temp.name,pfind->name);
   if (which == 0)
   {
      temp.found_in_1 = 1;
      temp.size1      = pfind->size;
      temp.wr_date1   = pfind->wr_date;
      temp.wr_time1   = pfind->wr_time;
      temp.attrib1    = pfind->attrib;

      temp.found_in_2 = 0;
      temp.size2      = 0L;
      temp.wr_date2   = 0;
      temp.wr_time2   = 0;
      temp.attrib2    = 0;
   }
   else
   {
      temp.found_in_1 = 0;
      temp.size1      = 0L;
      temp.wr_date1   = 0;
      temp.wr_time1   = 0;
      temp.attrib1    = 0;

      temp.found_in_2 = 1;
      temp.size2      = pfind->size;
      temp.wr_date2   = pfind->wr_date;
      temp.wr_time2   = pfind->wr_time;
      temp.attrib2    = pfind->attrib;
   }

   temp_files_saved = files_saved;

   result = lsearch(&temp,savefile,&files_saved,sizeof(struct file_t),cmpef);

   ndx = ((result - (char*)savefile)/sizeof(struct file_t));

   if (temp_files_saved == files_saved)
   {
      /*printf("Found at %3.3d - %s\n",ndx,savefile[ndx].name);*/
      if (which == 0)
      {
         savefile[ndx].found_in_1 = 1;
         savefile[ndx].size1      = pfind->size;
         savefile[ndx].wr_date1   = pfind->wr_date;
         savefile[ndx].wr_time1   = pfind->wr_time;
         savefile[ndx].attrib1    = pfind->attrib;
      }
      else
      {
         savefile[ndx].found_in_2 = 1;
         savefile[ndx].size2      = pfind->size;
         savefile[ndx].wr_date2   = pfind->wr_date;
         savefile[ndx].wr_time2   = pfind->wr_time;
         savefile[ndx].attrib2    = pfind->attrib;
      }
   }
   /*else
      printf("Inserted at %3.3d - %s\n",ndx,savefile[ndx].name);*/

   file_count[which]++;
}

static void save_dir(char *spec,int which)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   struct find_t find;
   unsigned attrib = _A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH;
                     /* ie all except _A_SUBDIR and _A_VOLID*/

   if (!_dos_findfirst(spec, attrib, &find))
   {
      save_info(&find,which);
      while(!_dos_findnext(&find)) save_info(&find,which);
   }
   /*else {erc = 4; return;}*/
}

static int cmpgle(const struct file_t *elem1, const struct file_t *elem2 )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   return(strcmp(elem1->name,elem2->name));
}

static void sort_list(void)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   qsort((void *)savefile, (size_t)files_saved, sizeof(struct file_t), cmpgle );
}

static char *timestr( unsigned t, char *buf )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Takes unsigned time in the format:   fedcba9876543210 ³*/
/*³ s=2 sec incr, m=0-59, h=23           hhhhhmmmmmmsssss ³*/
/*³ Changes to a 9-byte string:          hh:mm:ss         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
    int hrs  = (t >> 11) & 0x1f;
    int mins = (t >>  5) & 0x3f;
    int secs = t         & 0x1f;

    strcpy( buf,"??:??:??");
    sprintf( buf, "%2.2d:%2.2d:%2.2d",hrs,mins,secs);
    return buf;
}

static char *datestr( unsigned d, char *buf )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Takes unsigned date in the format:      fedcba9876543210  ³*/
/*³ d=1-31, m=1-12, y=0-119 (1980-2099)     yyyyyyymmmmddddd  ³*/
/*³ Changes to a 9-byte string:             yy-mm-dd          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
    int year  = ((d >> 9) & 0x7f) + 80;
    int month = (d >> 5) & 0x0f;
    int day   = d        & 0x1f;

    strcpy( buf,"??-??-??");
    sprintf( buf, "%02.2d-%02.2d-%02.2d",year ,month, day);
    return buf;
}

static void print_list(char *spec0, char *spec1)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   char buf1[10];
   char buf2[10];
   unsigned int ndx;

   printf("%-12s ", "        ");
   printf("%-33s",spec0);
   printf("%-33s",spec1);
   printf("\n");
   printf("%-12sº", "FILENAME");
   printf("%7s³%-8s³%-8s³%6sº","SIZE","DATE","TIME","ATTRIB");
   printf("%7s³%-8s³%-8s³%6sº","SIZE","DATE","TIME","ATTRIB");
   printf("\n");

   for (ndx=0;ndx<files_saved;ndx++)
   {
      printf("%-12sº", savefile[ndx].name);

      if (savefile[ndx].found_in_1)
         printf("%7ld³%8s³%8s³%c%c%c%c%c%cº",
                 savefile[ndx].size1,
                 datestr(savefile[ndx].wr_date1,buf1),
                 timestr(savefile[ndx].wr_time1,buf2),
                 (savefile[ndx].attrib1 & _A_SUBDIR) ? 'd' : ' ',
                 (savefile[ndx].attrib1 & _A_VOLID ) ? 'v' : ' ',
                 (savefile[ndx].attrib1 & _A_RDONLY) ? 'r' : ' ',
                 (savefile[ndx].attrib1 & _A_HIDDEN) ? 'h' : ' ',
                 (savefile[ndx].attrib1 & _A_SYSTEM) ? 's' : ' ',
                 (savefile[ndx].attrib1 & _A_ARCH)   ? 'a' : ' ' );
      else
         printf("       ³        ³        ³      º");

      if (savefile[ndx].found_in_2)
         printf("%7ld³%8s³%8s³%c%c%c%c%c%cº",
                 savefile[ndx].size2,
                 datestr(savefile[ndx].wr_date2,buf1),
                 timestr(savefile[ndx].wr_time2,buf2),
                 (savefile[ndx].attrib2 & _A_SUBDIR) ? 'd' : ' ',
                 (savefile[ndx].attrib2 & _A_VOLID ) ? 'v' : ' ',
                 (savefile[ndx].attrib2 & _A_RDONLY) ? 'r' : ' ',
                 (savefile[ndx].attrib2 & _A_HIDDEN) ? 'h' : ' ',
                 (savefile[ndx].attrib2 & _A_SYSTEM) ? 's' : ' ',
                 (savefile[ndx].attrib2 & _A_ARCH)   ? 'a' : ' ' );
      else
         printf("       ³        ³        ³      º");

      printf("\n");
   }

}

static int compare_files(int ndx)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   int compare_ok = 1;
   char filename1[_MAX_PATH];
   char filename2[_MAX_PATH];
   FILE *file1;
   FILE *file2;
   size_t bytes_read;
   char buf1[100];
   char buf2[100];

   strcpy(filename1,path[0]);
   strcpy(filename2,path[1]);
   strcat(filename1,savefile[ndx].name);
   strcat(filename2,savefile[ndx].name);

   file1 = fopen(filename1,"rb");
   if (!file1) {erc = 5; strcpy(errstr,filename1); return(0);}
   file2 = fopen(filename2,"rb");
   if (!file1) {erc = 5; strcpy(errstr,filename2); return(0);}

   while (!feof(file1))
   {
      bytes_read = fread(buf1,1,sizeof(buf1),file1);
      if ((!bytes_read) && (!feof(file1)))                {erc = 6; strcpy(errstr,filename1); break;}
      if (fread(buf2,1,sizeof(buf2),file2) != bytes_read) {erc = 6; strcpy(errstr,filename2); break;}
      if (memcmp(buf1,buf2,bytes_read))
      {
         compare_ok = 0;
         break;
      }
   }

   fclose(file2);
   fclose(file1);
   return(compare_ok);
}

static void PrintHeader(void)
{
   if (MustPrintHeader)
   {
      printf("\n");
      printf("   FILENAME    ³ COMPARE ³  EXIST  ³ LARGEST ³ NEWEST  ³\n");
      printf("   ÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄ´\n");
      MustPrintHeader = 0;
   }
}

static void process_list(void)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   int exist;
   int newest;
   int largest;
   int compare;
   unsigned int ndx;


   if (!f_raw_format)
      MustPrintHeader = 1;
   else
      MustPrintHeader = 0;

   for (ndx=0;ndx<files_saved;ndx++)
   {
      exist = 0;
      newest = 0;
      largest = 0;
      compare = ' ';

      if (savefile[ndx].found_in_1) exist = 1;
      if (savefile[ndx].found_in_2) exist += 2;
      if (exist==3)                 exist = 0;

      if (!exist)
      {
         if      (savefile[ndx].size1 > savefile[ndx].size2) largest = 1;
         else if (savefile[ndx].size1 < savefile[ndx].size2) largest = 2;

         if      (savefile[ndx].wr_date1 > savefile[ndx].wr_date2) newest = 1;
         else if (savefile[ndx].wr_date1 < savefile[ndx].wr_date2) newest = 2;
         else if (savefile[ndx].wr_time1 > savefile[ndx].wr_time2) newest = 1;
         else if (savefile[ndx].wr_time1 < savefile[ndx].wr_time2) newest = 2;

         compare = '.';
         if (!largest)
         {
            if (f_quick)
            {
               if (newest==0) compare = 'X';
            }
            else
            {
               if (compare_files(ndx)) compare = 'X';
               checkerc(11);
            }
         }

      }

      if ( ((compare == 'X') && (!f_dont_print_Ss)) ||
           ((compare == '.') && (!f_dont_print_Ds)) ||
           ((exist   == 1  ) && (!f_dont_print_1s)) ||
           ((exist   == 2  ) && (!f_dont_print_2s))    )
      {
         if (!f_raw_format)
         {
            PrintHeader();
            printf("   %-12s³    %c    ³    %1.0d    ³    %1.0d    ³    %1.0d    ³\n",
                    savefile[ndx].name, compare, exist, largest, newest);
         }
         else
         {
            int which = 0;
            switch (f_raw_format)
            {
               case '1':
                  if ((exist == 1) || (compare == 'X') || (compare == '.'))
                     which = 1;
                  break;
               case '2':
                  if ((exist == 2) || (compare == 'X') || (compare == '.'))
                     which = 2;
                  break;
               case 'N':
                  if (compare == '.')
                     if (newest==1)
                        which = 1;
                     else if (newest==2)
                        which = 2;
                  break;
               case 'O':
                  if (compare == '.')
                     if (newest==1)
                        which = 2;
                     else if (newest==2)
                        which = 1;
                  break;
            }
            if (which)
            {
               PrintHeader();
               printf("%s   %s%-12s\n", f_prefix?f_prefix:"",path[which-1],savefile[ndx].name);
            }
         }
      }
   }
}

void compare_directories(char *spec0, char *spec1)
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   int  i;
   char drive [_MAX_DRIVE];
   char dir   [_MAX_DIR];
   char fname [_MAX_FNAME];
   char ext   [_MAX_EXT];

   files_saved = 0;
   file_count[0] = 0;
   file_count[1] = 0;

   strcpy(spec[0],spec0);
   strcpy(spec[1],spec1);
   for (i=0;i<2;i++)
   {
      _splitpath( spec[i], drive, dir, fname, ext );
      _makepath ( path[i], drive, dir, ""   , ""  );
   }

   save_dir(spec[0],0);           checkerc(12);
   save_dir(spec[1],1);           checkerc(13);

   if (!f_quiet)
   if (f_raw_format)
   {
      fprintf(stderr,"\n");
      fprintf(stderr,"    Comparing Spec1 : %5d files in %s\n",file_count[0],path[0]);
      fprintf(stderr,"           to Spec2 : %5d files in %s\n",file_count[1],path[1]);
      fprintf(stderr,"\n");
   }
   else
   {
      printf("\n");
      printf("    Comparing Spec1 : %5d files in %s\n",file_count[0],path[0]);
      printf("           to Spec2 : %5d files in %s\n",file_count[1],path[1]);
/*
      fprintf(stderr,"        Total files : %5d\n",file_count[0]+file_count[1]);
      fprintf(stderr,"    Total filenames : %5d\n",files_saved);
      fprintf(stderr,"   Common filenames : %5d\n",file_count[0]+file_count[1]-files_saved);
*/
      printf("\n");
   }

   sort_list();                 checkerc(14);
   if (f_verbose)
      print_list(spec0,spec1);
   else
      process_list();
   checkerc(15);
}
