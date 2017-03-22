
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

int main ( int argc, char *argv[] )
{
   char InStr[81];
   char Command[81]={0};
   int Row=-1;
   int Col=-1;
   int Len=-1;
   int rc;

   strcpy(InStr,argv[1]);
   rc = sscanf(InStr,"%[^( ] ( %d , %d , %d )",Command,&Row,&Col,&Len);

   printf("%s (%d,%d,%d) rc=%d",Command,Row,Col,Len,rc);


   return(0);
}
