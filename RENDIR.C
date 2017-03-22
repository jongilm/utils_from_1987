
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <io.h>

void main ( int argc, char *argv[] )
{
   if (argc == 3)
   {
      if (rename(argv[1],argv[2]))
      {
	 printf("ERROR %d : ",errno);
	 switch (errno)
	 {
	    case EACCES : printf("%s already exists or an invalid path was specified\n", argv[2]); break;
	    case ENOENT : printf("%s not found\n", argv[1]);					   break;
	    case EXDEV	: printf("Attemp to move to a different device\n");			   break;
	    default	: printf("Unknown error\n");						   break;
	 }
      }
   }
   else
      printf("Usage : RENDIR <oldname> <newname>\n");
}
