#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFERSIZE 1024

FILE *infile;
FILE *outfile;
char oldstring[128];
char newstring[128];
char buffer[BUFFERSIZE];
char *test_from_ptr;
char *found_at_ptr;
int  oldlen;
int  newlen;
int display = 0;
int displayed = 0;

void usage(void);

void main(int argc, char *argv[])
{
   if (argc < 5) usage();
   if ((argc >= 6) && (toupper(argv[5][1]) == 'D')) display = 1;

   if ((infile  = fopen(argv[1],"rt")) == NULL) usage();
   if ((outfile = fopen(argv[2],"wt")) == NULL) usage();

   strcpy(oldstring,argv[3]);
   strcpy(newstring,argv[4]);
   oldlen = strlen(oldstring);
   newlen = strlen(newstring);

   while (!feof(infile))
   {
      fgets(buffer,1000,infile);
      if (feof(infile)) break;

      test_from_ptr = buffer;
      do
      {
         found_at_ptr = strstr(test_from_ptr,oldstring);
         if (found_at_ptr != NULL)
         {
            if (display && !displayed) printf("%s",buffer);
            displayed = 1;
            if (oldlen!=newlen)
               memmove(found_at_ptr+newlen,found_at_ptr+oldlen,strlen(found_at_ptr)-oldlen+1);
            memcpy(found_at_ptr,newstring,newlen);
         }
         test_from_ptr = found_at_ptr + newlen;
      } while (found_at_ptr != NULL);
      if (display && displayed) printf("%s",buffer);
      displayed = 0;
      fputs(buffer,outfile);
   }

   fclose(outfile);
   fclose(infile);
}

void usage(void)
{
   puts("Usage : Change <infile> <outfile> <oldstring> <newstring>");
   exit(1);
}
