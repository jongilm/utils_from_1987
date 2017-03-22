program  joystick;
uses
   crt;

const
   portno     = $201;
   mask       : array[0..7] of byte = (1,2,4,8,16,32,64,128);
   lft        = -1;
   mid        = 0;
   rit        = 1;
   top        = 1;
   bot        = -1;

var
   portvalue1 : byte;
   portvalue2 : byte;
   count      : byte;
   test       : byte;
   prevtest   : byte;
   sw1,sw2    : boolean;
   prevsw1,
   prevsw2    : boolean;
   x,y        : integer;
   map        : array[bot..top,lft..rit] of char;

function bit(bytex,mask : byte) : boolean;
begin
   bit := ((bytex and mask) <> 0);
end;


begin
   clrscr;
   test := 0;
   sw1  := false;
   sw2  := false;
   for x := bot to top do
      for y := lft to rit do
	 map[x,y] := ' ';

   repeat
      port[portno] := $00;

      for count := 0 to 10 do;
      portvalue1 := port[portno];
      for count := 0 to 50 do;
      portvalue2 := port[portno];

      prevtest := test;
      test := 0;

      if bit(portvalue1,mask[0]) then
	 test := test + 1;
      if bit(portvalue1,mask[1]) then
	 test := test + 2;
      if bit(portvalue2,mask[0]) then
	 test := test + 4;
      if bit(portvalue2,mask[1]) then
	 test := test + 8;

      if test <> prevtest then
      begin
         case prevtest of
	    0 : map[top,lft] := ' ';
   	    1 : map[top,mid] := ' ';
	    2 : map[mid,lft] := ' ';
	    3 : map[mid,mid] := ' ';
	    5 : map[top,rit] := ' ';
	    7 : map[mid,rit] := ' ';
	   10 : map[bot,lft] := ' ';
	   11 : map[bot,mid] := ' ';
	   15 : map[bot,rit] := ' ';
         end;

         case test of
	    0 : map[top,lft] := 'É';
	    1 : map[top,mid] := 'Í';
	    2 : map[mid,lft] := 'º';
	    3 : map[mid,mid] := 'Û';
	    5 : map[top,rit] := '»';
	    7 : map[mid,rit] := 'º';
	   10 : map[bot,lft] := 'È';
	   11 : map[bot,mid] := 'Í';
	   15 : map[bot,rit] := '¼';
         end;

         for x := top downto bot do
         begin
            gotoxy(38,13-x);
	    for y := lft to rit do
	       write(map[x,y]);
	    writeln;
         end;
      end;

      prevsw1 := sw1;
      prevsw2 := sw2;
      sw1 := not bit(portvalue2,mask[5]);
      sw2 := not bit(portvalue2,mask[4]);

      if sw1 <> prevsw1 then
      begin
         gotoxy(30,25);
         if sw1 then
            write('switch 1')
         else
            write('        ');
      end;

      if sw2 <> prevsw2 then
      begin
         gotoxy(42,25);
         if sw2 then
            write('switch 2')
         else
            write('        ');
      end;

   until keypressed;
end.