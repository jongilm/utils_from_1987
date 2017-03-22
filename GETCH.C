#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
main()
{
   int ch;

   do
   {
      ch = getch();
      if (!ch) ch = -getch();
      printf("%c %2.2X\n",(ch<0) ? '-':' ',(ch<0) ? -ch : ch);
   } while (ch != 27);
}
