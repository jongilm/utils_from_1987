#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <tools.h>

char buffer[2001];

void main(int argc,char *argv[])
{
   if (argc != 3)
   {
      puts("Usage : FillScr <char> <colour>");
      puts("     eg FillScr ° 15");
      puts("");
      puts("black   = 0");
      puts("blue    = 1");
      puts("green   = 2");
      puts("cyan    = 3");
      puts("red     = 4");
      puts("magenta = 5");
      puts("brown   = 6");
      puts("white   = 7");
      puts("bright  = +8  (blink when added to background)");
      puts("");
      puts("attribute = 256 * background + foreground");
      return;
   }
   memset(buffer,argv[1][0],2000);
   buffer[2000] = '\0';
   _put_text(1,1,atoi(argv[2]),buffer);
}

