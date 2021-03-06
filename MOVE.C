/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
�                                                                            �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main(int argc, char *argv[]);

main(int argc, char *argv[])
/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
�                                                                            �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸
*/
{
   if (argc != 3)
      printf("Usage : MOVE <oldname> <newname>");
   else
   {
      if (rename(argv[1],argv[2]))
      {
         if (errno == EACCES)
            printf("%s already exists or invalid path specified",argv[2]);
         else
            if (errno == ENOENT)
               printf("%s not found",argv[1]);
            else
               if (errno == EXDEV)
                  printf("Cannot move to different device");
               else
                  printf("Move unsuccessful - errno = %d",errno);
      }
      else
         printf("%s moved to %s",argv[1],argv[2]);
   }
   return(0);
}

