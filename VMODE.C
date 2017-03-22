
//#include <stdlib.h>
//#include <stdio.h>
//#include <conio.h>
//#include <string.h>
//#include <dos.h>

char str1[] = "Changed to video mode $";
char str2[] = "00h$";
unsigned int mode = 3;

int main ( int argc, char *argv[] )
{

   if (argc==2)
   {
      char *ptr;
      char ch;

      ptr = argv[1];

      while(*ptr)
      {
        if ((*ptr>='a') && (*ptr<='f'))
           *ptr += 'A' - 'a';
        ptr++;
      }

      mode = 0;
      ch = argv[1][0];

      if (((ch>='0') && (ch<='9')) || ((ch>='A') && (ch<='F')))
      {
         mode += (ch<='9') ? ch - '0': ch - 'A' + 10;

         ch = argv[1][1];
         if (((ch>='0') && (ch<='9')) || ((ch>='A') && (ch<='F')))
         {
            mode *= 16;
            mode += (ch<='9') ? ch - '0': ch - 'A' + 10;
         }
      }
   }

   str2[0] = ((mode/16)<10)?((mode/16)+'0'):((mode/16)-10+'A');
   str2[1] = ((mode%16)<10)?((mode%16)+'0'):((mode%16)-10+'A');

   _asm
   {
      mov ah,0
      mov al,byte ptr mode
      int 10h

      mov ah,9
      mov dx,offset str1
      int 21h

      mov ah,9
      mov dx,offset str2
      int 21h
   }

   return(0);
}
