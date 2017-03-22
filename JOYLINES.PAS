program  joystick;
{---------------------------------------------------------------------------}
{                                                                           }
{---------------------------------------------------------------------------}
uses
   crt,graph;

var
   swch1,
   swch2,
   left,
   right,
   up,
   down       : boolean;
   x,y,x1,y1  : integer;
   gmode,gdriver : integer;
   trigger       : boolean;

procedure readstick(var u,d,l,r,s1,s2 : boolean);
{---------------------------------------------------------------------------}
{                                                                           }
{---------------------------------------------------------------------------}
const
   portno     = $201;

var
   portvalue1 : byte;
   portvalue2 : byte;
   count      : byte;

function bit(ndx,bytex : byte) : boolean;
{---------------------------------------------------------------------------}
{                                                                           }
{---------------------------------------------------------------------------}
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



{---------------------------------------------------------------------------}
{ MAIN PROGRAM                                                              }
{---------------------------------------------------------------------------}
begin
   detectgraph(gdriver,gmode);
   initgraph(gdriver,gmode,'');
   cleardevice;
   clearviewport;
{   clrscr;
   textbackground(black);}

   x := 360;
   y := 175;
   x1 := x;
   y1 := y;
   setcolor(getmaxcolor);
   trigger := false;
   repeat
{      setcolor(random(getmaxcolor+1));}
      readstick(up,down,left,right,swch1,swch2);

      if up then y := y - 1;
      if down then y := y + 1;
      if left then x := x - 1;
      if right then x := x + 1;
      if x < 0 then x := 0;
      if x > 719 then x := 719;
      if y < 0 then y := 0;
      if y > 347 then y := 347;
      if swch1 then
      begin
         x1 := x;
         y1 := y;
{         setfillstyle(solidfill,lightgray);
         floodfill(x+1,y+1,getmaxcolor);}
      end;
      if swch2 then
      begin
         if trigger then
            trigger := false
         else
            trigger := true;
         delay(200);
{         setcolor(random(getmaxcolor+1));}
{         clearviewport;
         moveto(x,y);}
      end;
{      delay(5);}

      if trigger then
         line(x1,y1,x,y)
      else
         lineto(x,y);
   until keypressed;
   closegraph;
   restorecrtmode;
end.