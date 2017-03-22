
int main ( int argc, char *argv[] )
{
   char blnk = 1;

   if (argc > 1)
      if ((argv[1][0] == '0') || (argv[1][1] == 'f')) blnk = 0;

   _asm
   {
      mov ah, 10h
      mov al, 03h
      mov bl, blnk
      int 10h /* reset blink/intesity bit to blink */
   }
   return 0;
}
