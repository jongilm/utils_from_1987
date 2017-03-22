program  joystick;
uses
   crt;

var
   swch1,
   swch2,
   left,
   right,
   up,
   down       : boolean;


procedure readstick(var u,d,l,r,s1,s2 : boolean);
const
   portno     = $201;

var
   portvalue1 : byte;
   portvalue2 : byte;
   count      : byte;

function bit(ndx,bytex : byte) : boolean;
const
   mask       : array[0..7] of byte = (1,2,4,8,16,32,64,128);
begin
   bit := ((bytex and mask[ndx]) <> 0);
end;

begin
   port[portno] := $00;

   for count := 0 to 10 do;
   portvalue1 := port[portno];
   for count := 0 to 50 do;
   portvalue2 := port[portno];

   l  := not bit(0,portvalue1);
   u  := not bit(1,portvalue1);
   r  :=     bit(0,portvalue2);
   d  :=     bit(1,portvalue2);
   s1 := not bit(5,portvalue2);
   s2 := not bit(4,portvalue2);

end;



begin
   clrscr;
   textcolor(white);
   textbackground(black);
   gotoxy(1,1);
   writeln('***');
   writeln('***');
   writeln('***');

   repeat
      readstick(up,down,left,right,swch1,swch2);
         mem[$B000:0*160+1] := black;
         mem[$B000:0*160+3] := black;
         mem[$B000:0*160+5] := black;
         mem[$B000:1*160+1] := black;
         mem[$B000:1*160+3] := black;
         mem[$B000:1*160+5] := black;
         mem[$B000:2*160+1] := black;
         mem[$B000:2*160+3] := black;
         mem[$B000:2*160+5] := black;

      if up then
         if left then
            mem[$B000:0*160+1] := white
         else
            if right then
               mem[$B000:0*160+5] := white
            else
               mem[$B000:0*160+3] := white;

      if down then
         if left then
            mem[$B000:2*160+1] := white
         else
            if right then
               mem[$B000:2*160+5] := white
            else
               mem[$B000:2*160+3] := white;

      if left and not up and not down then
         mem[$B000:1*160+1] := white;

      if right and not up and not down then
         mem[$B000:1*160+5] := white;

      if not up and not down and not left and not right then
         mem[$B000:1*160+3] := white;
   until keypressed;
end.