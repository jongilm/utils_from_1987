/*

          The Microsoft C 4.0 program lists the duplicate file names on 
     a drive sorted by file name, date, time, size, and path.

          Written by James L. Dean
                     406 40th Street
                     New Orleans, LA 70124
                     May 15, 1987       

*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <dos.h>

#define FALSE 0
#define TRUE 1

typedef struct dir_node
                 {
                   char            file_name [14];
                   char            attributes;
                   unsigned int    time_stamp;
                   unsigned int    date_stamp;
                   unsigned long   file_size;
                   struct dir_node *next;
                 } *dir_node_ptr;

typedef struct path_node
                 {
                   char             *path_name;
                   unsigned int     time_stamp;
                   unsigned int     date_stamp;
                   unsigned long    file_size;
                   struct path_node *next;
                   struct path_node *previous;
                 } *path_node_ptr;

typedef struct file_node
                 {
                   char             *file_name;
                   struct path_node *path_head;
                   struct file_node *predecessor;
                   struct file_node *lesser;
                   struct file_node *greater;
                 } *file_node_ptr;

static void build_file_tree(char *,file_node_ptr *);
char far    *far_ptr(unsigned int,unsigned int);
static void get_dir(char *,dir_node_ptr *);
static void report_dups(file_node_ptr *);

struct dta {
             char          reserved [21];
             unsigned char attributes;
             unsigned int  time_stamp;
             unsigned int  date_stamp;
             unsigned long file_size;
             char          file_name [13];
           }  *dta_ptr;

int           fatal_error;

void main(argc,argv)
 int  argc;
 char *argv[];
  {
    static file_node_ptr file_head;
    static union REGS    inreg;
    static union REGS    outreg;

    inreg.h.ah=(unsigned char) 98;
    intdos(&inreg,&outreg);
    dta_ptr=(struct dta *) far_ptr(outreg.x.bx,128);
    fatal_error=FALSE;
    file_head=NULL;
    if (argc == 1)
      {
        build_file_tree("",&file_head);
        if (! fatal_error)
          report_dups(&file_head);
      }
    else
      if ((strlen(argv[1]) == 2) 
      &&  (argv[1][1] == ':')
      &&  (isalpha((int) argv[1][0])))
        {
          build_file_tree(argv[1],&file_head);
          if (! fatal_error)
            report_dups(&file_head);
        }
      else
        {
          printf("Finds duplicated file names on a drive.\n\n");
          printf("Usage:  DUPS [drive]\n");
          printf("Example:  DUPS C:\n");
        }
    return;
  }

static void build_file_tree(path_name,file_head)
  char          *path_name;
  file_node_ptr *file_head;
    {
      dir_node_ptr  dir_head;
      char          dir_name[256];
      dir_node_ptr  dir_ptr;
      file_node_ptr file_ptr_1;
      file_node_ptr file_ptr_2;
      int           finished;
      char          new_path_name[256];
      path_node_ptr path_ptr_1;
      path_node_ptr path_ptr_2;
      path_node_ptr path_ptr_3;
      int           relation;
      
      strcpy(dir_name,path_name);
      strcat(dir_name,"\\*.*"); 
      dir_head=NULL;
      get_dir(dir_name,&dir_head);
      if (! fatal_error)
        {
          while (dir_head != NULL)
            {
              if (dir_head->file_name[0] != '.')
                {
                  if ((dir_head->attributes) & 16)
                    {
                      strcpy(new_path_name,path_name);
                      strcat(new_path_name,"\\");
                      strcat(new_path_name,dir_head->file_name);
                      build_file_tree(new_path_name,file_head);
                    }
                  else
                    {
                      if ((path_ptr_1=(struct path_node *) malloc(
                       (unsigned) sizeof(struct path_node))) == NULL)
                        {
                          fatal_error=TRUE;
                          printf("Fatal error:  out of memory.\n");
                        }
                      else
                        {
                          path_ptr_1->next=NULL;
                          path_ptr_1->previous=NULL;
                          if ((path_ptr_1->path_name=malloc((unsigned)
                           (1+strlen(path_name)))) == NULL)
                            {
                              fatal_error=TRUE;
                              printf("Fatal error:  out of memory.\n");
                            }
                          else
                            {
                              strcpy(path_ptr_1->path_name,path_name);  
                              path_ptr_1->time_stamp=dir_head->time_stamp;
                              path_ptr_1->date_stamp=dir_head->date_stamp;
                              path_ptr_1->file_size=dir_head->file_size;
                            }
                        }
                      if (! fatal_error)
                        {
                          if (*file_head == NULL)
                            {
                              if ((*file_head
                               =(struct file_node *)
                               malloc((unsigned) sizeof(struct file_node))) 
                               == NULL)
                                {
                                  fatal_error=TRUE;
                                  printf("Fatal error:  out of memory.\n");
                                }
                              else    
                                {
                                  (*file_head)->predecessor=NULL;
                                  (*file_head)->lesser=NULL;
                                  (*file_head)->greater=NULL;
                                  (*file_head)->path_head=path_ptr_1;
                                  if (((*file_head)->file_name=malloc(
                                   (unsigned) (1+strlen(dir_head->file_name))))
                                   == NULL)
                                    {
                                      fatal_error=TRUE;
                                      printf("Fatal error:  out of memory.\n");
                                    }
                                  else
                                    strcpy((*file_head)->file_name,
                                     dir_head->file_name);
                                }
                            }
                          else
                            {
                              finished=FALSE;
                              file_ptr_1=*file_head;
                              while (! finished)
                                {
                                  relation=strcmp(dir_head->file_name,
                                   file_ptr_1->file_name);
                                  if (relation < 0)
                                    if (file_ptr_1->lesser == NULL)
                                      {
                                        if ((file_ptr_2
                                         =(struct file_node *)
                                         malloc((unsigned)
                                         sizeof(struct file_node))) 
                                         == NULL)
                                          {
                                            fatal_error=TRUE;
                                            printf(
                                             "Fatal error:  out of memory.\n");
                                          }
                                        else    
                                          {
                                            file_ptr_1->lesser=file_ptr_2;
                                            file_ptr_2->predecessor
                                             =file_ptr_1;
                                            file_ptr_2->lesser=NULL;
                                            file_ptr_2->greater=NULL;
                                            file_ptr_2->path_head=path_ptr_1;
                                            if ((file_ptr_2->file_name=malloc(
                                             (unsigned)
                                             (1+strlen(dir_head->file_name))))
                                             == NULL)
                                              {
                                                fatal_error=TRUE;
                                                printf(
                                             "Fatal error:  out of memory.\n");
                                              }
                                            else
                                              strcpy(file_ptr_2->file_name,
                                               dir_head->file_name);
                                          }
                                        finished=TRUE;
                                      }
                                    else
                                      file_ptr_1=file_ptr_1->lesser;
                                  else
                                    if (relation > 0) 
                                      if (file_ptr_1->greater == NULL)
                                        {
                                          if ((file_ptr_2
                                           =(struct file_node *)
                                           malloc((unsigned)
                                           sizeof(struct file_node))) 
                                           == NULL)
                                            {
                                              fatal_error=TRUE;
                                              printf(
                                             "Fatal error:  out of memory.\n");
                                            }
                                          else    
                                            {
                                              file_ptr_1->greater=file_ptr_2;
                                              file_ptr_2->predecessor
                                               =file_ptr_1;
                                              file_ptr_2->lesser=NULL;
                                              file_ptr_2->greater=NULL;
                                              file_ptr_2->path_head=path_ptr_1;
                                              if ((file_ptr_2->file_name
                                               =malloc((unsigned) (1+strlen(
                                               dir_head->file_name)))) == NULL)
                                                {
                                                  fatal_error=TRUE;
                                                  printf(
                                             "Fatal error:  out of memory.\n");
                                                }
                                              else
                                                strcpy(file_ptr_2->file_name,
                                                 dir_head->file_name);
                                            }
                                          finished=TRUE;
                                        }
                                      else
                                        file_ptr_1=file_ptr_1->greater;
                                    else
                                      {
                                        path_ptr_2=file_ptr_1->path_head;
                                        path_ptr_3=NULL;
                                        while ((path_ptr_2 != NULL)
                                        &&     ((path_ptr_1->date_stamp
                                                  > path_ptr_2->date_stamp)
                                             || ((path_ptr_1->date_stamp
                                                   == path_ptr_2->date_stamp)
                                              && (path_ptr_1->time_stamp
                                                   > path_ptr_2->time_stamp))
                                             || ((path_ptr_1->date_stamp
                                                   == path_ptr_2->date_stamp)
                                              && (path_ptr_1->time_stamp
                                                   == path_ptr_2->time_stamp)
                                              && (path_ptr_1->file_size
                                                   > path_ptr_2->file_size))
                                             || ((path_ptr_1->date_stamp
                                                   == path_ptr_2->date_stamp)
                                              && (path_ptr_1->time_stamp
                                                   == path_ptr_2->time_stamp)
                                              && (path_ptr_1->file_size
                                                   == path_ptr_2->file_size)
                                              && (strcmp(path_ptr_1->path_name,
                                                   path_ptr_2->path_name) 
                                                   > 0))))
                                           {
                                             path_ptr_3=path_ptr_2;
                                             path_ptr_2=path_ptr_2->next;
                                           }
                                        if (path_ptr_2 == NULL)
                                          path_ptr_3->next=path_ptr_1;
                                        else
                                          {
                                            if (path_ptr_3 == NULL)
                                              file_ptr_1->path_head=path_ptr_1;
                                            else
                                              path_ptr_3->next=path_ptr_1;
                                            path_ptr_2->previous=path_ptr_1;
                                          }
                                        path_ptr_1->next=path_ptr_2;
                                        path_ptr_1->previous=path_ptr_3;
                                        finished=TRUE;
                                      }
                                } 
                            }
                        }
                    }
                }
              dir_ptr=dir_head;
              dir_head=dir_head->next;
              free((char *) dir_ptr);
            }
        }
    }

static void get_dir(dir_name,dir_head)
  char far     *dir_name;
  dir_node_ptr *dir_head;
    {
      static union REGS    inreg;
      static dir_node_ptr  new_dir_entry;
      static union REGS    outreg;
      static struct SREGS  segreg;

      inreg.h.ah=(unsigned char) 78;
      segreg.ds=FP_SEG(dir_name);
      inreg.x.dx=FP_OFF(dir_name);
      inreg.x.cx=(unsigned int) 0x3f;
      intdosx(&inreg,&outreg,&segreg);
      if ((outreg.x.ax != (unsigned int) 2)
      &&  (outreg.x.ax != (unsigned int) 18))
        {
          while ((outreg.x.ax != (unsigned int) 18)
          &&     (! fatal_error))
            {
              if (*dir_head == NULL)
                {
                  if ((*dir_head
                   =(struct dir_node *)
                   malloc((unsigned) sizeof(struct dir_node))) == NULL)
                    {
                      fatal_error=TRUE;
                      printf("Fatal error:  out of memory.\n");
                    }
                  else    
                    {
                      (*dir_head)->next=NULL;
                      strcpy((*dir_head)->file_name,dta_ptr->file_name);
                      (*dir_head)->attributes=dta_ptr->attributes;
                      (*dir_head)->time_stamp=dta_ptr->time_stamp;
                      (*dir_head)->date_stamp=dta_ptr->date_stamp;
                      (*dir_head)->file_size=dta_ptr->file_size;
                    }
                }
              else
                {
                  if ((new_dir_entry=(struct dir_node *)
                   malloc((unsigned) sizeof(struct dir_node))) == NULL)
                    {
                      fatal_error=TRUE;
                      printf("Fatal error:  out of memory.\n");
                    }
                  else
                    {
                      strcpy(new_dir_entry->file_name,dta_ptr->file_name);
                      new_dir_entry->attributes=dta_ptr->attributes;
                      new_dir_entry->time_stamp=dta_ptr->time_stamp;
                      new_dir_entry->date_stamp=dta_ptr->date_stamp;
                      new_dir_entry->file_size=dta_ptr->file_size;
                      new_dir_entry->next=(*dir_head);
                      (*dir_head)=new_dir_entry;
                    }
                }
              inreg.h.ah=(unsigned char) 79;
              intdosx(&inreg,&outreg,&segreg);
            }
        }
      return;
    }

static void report_dups(file_head)
  file_node_ptr *file_head;
    {
      static   char           am_or_pm [2];
      static   int            bypass_smaller;
      static   int            char_index;
      static   char           *char_ptr;
      static   file_node_ptr  current_file_ptr;
      static   char           day [3];
      static   unsigned int   day_num;
      static   char           file_name [14];
      static   int            finished;
      static   int            hit;
      static   char           hour [3];
      static   unsigned int   hour_num;
      static   int            larger_predecessor_found;
      static   char           minute [3];
      static   unsigned int   minute_num;
      static   char           month [3];
      register unsigned int   month_num;
      static   path_node_ptr  path_ptr;
      static   file_node_ptr  previous_file_ptr;
      static   char           second [3];
      static   unsigned int   second_num;
      static   char           year [3];
      register unsigned int   year_num;

      if (*file_head == NULL)
        printf("There are no files!\n");
      else
        {      
          hit=FALSE;
          current_file_ptr=*file_head;
          finished=FALSE;
          bypass_smaller=FALSE;
          do
            {
              if (! bypass_smaller)
                while (current_file_ptr->lesser != NULL)
                  current_file_ptr=current_file_ptr->lesser;
              if (current_file_ptr->path_head->next != NULL)
                {
                  hit=TRUE;
                  char_index=0;
                  char_ptr=current_file_ptr->file_name;
                  while (file_name[char_index]=*char_ptr)
                    {
                      char_ptr++;
                      char_index++;
                    }
                  while (char_index < 13) file_name[char_index++]=' ';
                  file_name[char_index]='\0';
                  path_ptr=current_file_ptr->path_head;
                  while (path_ptr != NULL)
                    {
                      printf("%s",file_name);
                      year_num=path_ptr->date_stamp/(unsigned int) 512;
                      path_ptr->date_stamp-=((unsigned int) 512*year_num);
                      year_num+=(unsigned int) 80;
                      if (year_num > 100) year_num-=(unsigned int) 100;
                      month_num=path_ptr->date_stamp/(unsigned int) 32;
                      day_num=path_ptr->date_stamp-(unsigned int) 32*month_num;
                      sprintf(year,"%2d",year_num);
                      if (year[0] == ' ') year[0]='0';
                      sprintf(month,"%2d",month_num);
                      sprintf(day,"%2d",day_num);
                      if (day[0] == ' ') day[0]='0';
                      hour_num=path_ptr->time_stamp/(unsigned int) 2048;
                      path_ptr->time_stamp-=((unsigned int) 2048*hour_num);
                      minute_num=path_ptr->time_stamp/(unsigned int) 32;
                      path_ptr->time_stamp-=((unsigned int) 32*minute_num);
                      second_num=2*path_ptr->time_stamp;
                      if (hour_num >= 12)
                        am_or_pm[0]='p';
                      else
                        am_or_pm[0]='a';
                      am_or_pm[1]='\0';
                      if (hour_num > 12) hour_num-=12;
                      sprintf(hour,"%2d",hour_num);
                      sprintf(minute,"%2d",minute_num);
                      if (minute[0] == ' ') minute[0]='0';
                      sprintf(second,"%2d",second_num);
                      if (second[0] == ' ') second[0]='0';
                      printf("%s/%s/%s %s:%s:%s%s %10ld ",
                       month,day,year,hour,minute,second,am_or_pm,
                       path_ptr->file_size);
                      if (*(path_ptr->path_name) == '\0')
                        printf("\\\n");
                      else
                        printf("%s\n",path_ptr->path_name);
                      path_ptr=path_ptr->next;
                    }
                  printf("\n");
                }
              while (current_file_ptr->path_head != NULL)
                {
                  path_ptr=current_file_ptr->path_head;
                  current_file_ptr->path_head=path_ptr->next;
                  free(path_ptr->path_name);
                  free((char *) path_ptr);
                }
              if (current_file_ptr->greater != NULL)
                {
                  current_file_ptr=current_file_ptr->greater;
                  bypass_smaller=FALSE;
                }
              else
                {
                  larger_predecessor_found=FALSE;
                  do
                    {
                      if (current_file_ptr->predecessor == NULL)
                        finished=TRUE;
                      else
                        {
                          previous_file_ptr=current_file_ptr;
                          current_file_ptr=previous_file_ptr->predecessor;
                          if (strcmp(current_file_ptr->file_name,
                           previous_file_ptr->file_name) > 0)
                            larger_predecessor_found=TRUE;
                          free(previous_file_ptr->file_name);
                          free((char *) previous_file_ptr);
                        }
                    }
                  while ((! finished) && (! larger_predecessor_found));
                  bypass_smaller=TRUE;
                }
            }
          while (! finished);
          free((*file_head)->file_name);
          free((char *) *file_head);
          *file_head=NULL;
          if (! hit)
            printf("No duplicate file names were found.\n");
        }
      return;
    }
