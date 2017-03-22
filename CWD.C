
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

   char buf[1000];
int main ( int argc, char *argv[] )
{
   printf("%s",_getcwd(buf,999));
   return(0);
}
