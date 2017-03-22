
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>

int main(int argc, char *argv[]);
int argok(char *arg);
void check_params(void);

int xargc;
char **xargv;
FILE *file;
int ch;
long pos;
long i;
long j;
int k;
int paramerror;
int oldch;
int newch;

int main(int argc, char *argv[])
{
   xargc = argc;
   xargv = argv;
   i = 0;
   j = 0;
   k = 99;
   check_params();
   if (paramerror)
   {
      printf("\n");
      printf("Usage : CHANGE filename 'a' 'b'      where a = oldchar\n");
      printf("                                           b = newchar\n");
      printf("\n");
      printf("The following \"C\" characters are supported :\n");
      printf("      \\0  null           \\n  newline        \n");
      printf("      \\b  backspace      \\r  carriage return\n");
      printf("      \\a  alarm          \\f  form feed      \n");
      printf("      \\t  horozontal tab \\v  vertical tab   \n");
      printf("      \\\'  quote          \\\"  double quote   \n");
      printf("      \\\\  backslash                         \n");
      printf("as well as :\n");
      printf("      \\s  space                              \n");
      printf("\n");
      printf("Jonathan Gilmore 1989");
   }
   else
   {
      printf("\n");
      printf("CHANGEing %s to %s in file : %s\n\n",argv[2],argv[3],argv[1]);
      printf("Press any key to contine or <Escape> to abort ");
      if (getch()==27)
      {
         printf("\r                                                 ");
      }
      else
      {
         printf("\r                                                 ");
         file = fopen(argv[1],"r+b");
         while (!feof(file))
         {
            ch = fgetc(file);
            j++;
            k++;
            if (k==100)
            {
               k=0;
               printf("\rCharacters:%8ld, Changes:%8ld",j,i);
            }
            if (ch == oldch)
            {
               i++;
               pos = ftell(file) - 1;
               fseek(file,pos,0);
               fputc(newch,file);
               pos = ftell(file);
               fseek(file,pos,0);
            }
         }
      }
      printf("\rCharacters:%8ld, Changes:%8ld",j,i);
   }
   printf("\n");
   fclose(file);
   return(0);
}

int argok(char *arg)
{
   int c;

   if (arg[0] != '\'') {paramerror = 1; goto ret1;}
   if (arg[1] == '\\')
   {
      switch (arg[2])
      {
      case '0'  : c = '\0'; break;
      case 'n'  : c = '\n'; break;
      case 'b'  : c = '\b'; break;
      case 'a'  : c = '\a'; break;
      case 't'  : c = '\t'; break;
      case 'r'  : c = '\r'; break;
      case 'f'  : c = '\f'; break;
      case 'v'  : c = '\v'; break;
      case '\\' : c = '\\'; break;
      case '\'' : c = '\''; break;
      case '\"' : c = '\"'; break;
      case 's'  : c = ' ' ; break;
      default   : paramerror = 1;
      }
      if (paramerror) goto ret1;
      if (arg[3] != '\'') {paramerror = 1; goto ret1;}
      if (arg[4] != '\0') {paramerror = 1; goto ret1;}
   }
   else
   {
      if (!isgraph(arg[1])) {paramerror = 1; goto ret1;}
      if (arg[2] != '\'') {paramerror = 1; goto ret1;}
      if (arg[3] != '\0') {paramerror = 1; goto ret1;}
      c = arg[1];
   }
ret1:
   return(c);
}

void check_params(void)
{
   if (xargc != 4) {paramerror = 1; goto ret;}
   oldch = argok(xargv[2]);
   if (paramerror) goto ret;
   newch = argok(xargv[3]);
   if (paramerror) goto ret;
ret: return;
}
