{box.inc    Miscelaneous global subprograms}

{-----------------------------------------------------------------------------}
procedure beep;
begin
   sound(760);
   delay(50);
   nosound;
end; {beep}


{-----------------------------------------------------------------------------}
procedure repeat_char(character : char;  count : integer);
var i        : integer;

begin
   i := 0;
   while i < count do
   begin
      write(character);
      i := succ(i);
   end;
end; {repeat_char}

{-----------------------------------------------------------------------------}
procedure box(x,y,w,h : integer);

const
   top_left    = #218;
   top_right   = #191;
   bot_left    = #192;
   bot_right   = #217;
   horoz       = #196;
   vert        = #179;

var
   zi,x1      :  byte;

begin
   gotoxy(x+w-1,y+h-1);
   if ((x+w) > 80) and ((y+h) > 25) then
      if (lastmode = mono) then
      begin
         mem[$B000:$0F9E] := ord(bot_right);  {   $b000:3998}
         mem[$B000:$0F9F] := textattr;  {mem[$B000:$0F9F] + 8;}
      end
      else
      begin
         mem[$B800:$0F9E] := ord(bot_right); {   $b800:3998}
         mem[$B800:$0F9F] := textattr;   {mem[$B800:$0F9F] + 8;}
      end
   else
      write(bot_right);

   gotoxy(x,y+h-1);
   write(bot_left);
   repeat_char(horoz,w-2);

   gotoxy(x,y);
   write(top_left);
   repeat_char(horoz,w-2);
   write(top_right);

   x1 := x+w-1;
   for zi:= y+1 to y+h-2 do
   begin
      gotoxy(x,zi);
      write(vert);
      gotoxy(x1,zi);
      write(vert);
   end;

   gotoxy(x,y);
   end;

