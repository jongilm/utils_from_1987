{$R-}    {Range checking off}
{$B+}    {Boolean complete evaluation on}
{$S+}    {Stack checking on}
{$I+}    {I/O checking on}
{$N-}    {No numeric coprocessor}
{$M 65500,16384,655360} {Turbo 3 default stack and heap}

program memory_display;

{  this program displays the contents of memory onto the ibm pc's screen.   }
{  the user may move though the memory by using the page up, page dn, home, }
{  end, left, right, up arrow and down arrow keys.                          }

{  july 10, 1984.  jeff firestone.                                          }


uses
  crt;

const
  endofmem          = $9f88;   { top of mem. (640k) less one page (1920) }
  monochromescreen  = $b00a;   { location of monochrome card + 1 line }
  colorgraphscreen  = $b80a;   { location of color graphics card + 1 line }
  esc               = #27;

var
  i,
  initial_segment,
  initial_offset,
  val_error             : integer;
  screen_segment,
  screen_offset         : word;
  parameter_address     : longint;
  done                  : boolean;
  top_pntr, scrn, pntr  : ^char;

procedure beep;
begin
   sound(760);
   delay(50);
   nosound;
end; {beep}

function hexstring(a:integer) : string;
const
  hex = '0123456789ABCDEF';
var
  inter,i : byte;
  tempstr : string;

begin
   tempstr := '';
   for i := 1 to 4 do
   begin
     inter := a shr 12;
     a     := a shl 4;
     tempstr := tempstr + copy(hex,inter+1,1);
   end;
   hexstring := tempstr;
end;

procedure display_address(segment,offset : word);
var
  address          : longint;
begin
    address := segment;
    address := address * 16 + offset;
    gotoxy(9,1);
    textcolor(lightcyan);    write('Address = ');
    textcolor(yellow);       write(hexstring(segment));
                             write(':');
                             write(hexstring(offset));
    textcolor(lightcyan);    write('(');
    textcolor(yellow);       write(address:7);
    textcolor(lightcyan);    write(') "');
    textcolor(yellow);       write(chr(mem[segment:offset  ]));
                             write(chr(mem[segment:offset+1]));
    textcolor(lightcyan);    write('" = ');
    textcolor(yellow);       write(hexstring(mem[segment:offset]*256+mem[segment:offset+1]));
    textcolor(lightcyan);    write('(');
    textcolor(yellow);       write(mem[segment:offset  ]:3,',');
                             write(mem[segment:offset+1]:3);
    textcolor(lightcyan);    write(')');
end;


procedure initialize;
begin
  textbackground(black);
  textcolor(lightgray);
  clrscr;
  if lastmode = mono then
     screen_segment := monochromescreen
  else
     screen_segment := colorgraphscreen;
  screen_offset := 0;

  parameter_address := 0;
  val(paramstr(1),parameter_address,val_error);
  initial_segment := parameter_address div 16;
  initial_offset  := parameter_address mod 16;

  textbackground(blue);
  textcolor(lightred);
  gotoxy(1,1);
  write(' D-RAM');
  clreol;
  gotoxy(61,1);
  textcolor(lightcyan);   write('Display = ');
  textcolor(yellow);      write(hexstring(screen_segment),':');
                          write(hexstring(screen_offset));
  display_address(initial_segment,initial_offset);
end;
{-------------------------------------------------------------}

procedure normalize(segment_pntr, offset_pntr : integer);
var
  segment, offset : integer;
begin
  segment := memw[segment_pntr:offset_pntr+2];
  offset  := memw[segment_pntr:offset_pntr  ];

  if (offset < 0) then
    begin
      offset  := 15;
      segment := segment - 1;
    end;
  segment := (segment + (offset shr 4));
  offset  := (offset and 15);

  memw[segment_pntr:offset_pntr+2] := segment;
  memw[segment_pntr:offset_pntr  ] := offset;
end;

{-------------------------------------------------------------}

{-------------------------------------------------------------}

procedure read_keyboard;
const
  pagesz = 120;  {120 segments = 1920 bytes = 24 lines of chars}
  linesz = 5;    {  5 segments =   80 bytes =  1 line  of chars}
  charsz = 1;    {  1 char }
var
  key              : char;
  seg_top_pntr,
  ofs_top_pntr     : word;

begin
   seg_top_pntr := memw[seg(top_pntr):ofs(top_pntr)+2];
   ofs_top_pntr := memw[seg(top_pntr):ofs(top_pntr)  ];

   key := readkey;
   if key = #0 then
   begin
      key := readkey;
      case key of
        'Q' : seg_top_pntr := seg_top_pntr + pagesz;  { page dn }
         'I' : seg_top_pntr := seg_top_pntr - pagesz;  { page up }
         'P' : seg_top_pntr := seg_top_pntr + linesz;  { down arrow }
         'H' : seg_top_pntr := seg_top_pntr - linesz;  { up arrow   }
         'M' : ofs_top_pntr := ofs_top_pntr + charsz;  { right arrow }
         'K' : ofs_top_pntr := ofs_top_pntr - charsz;  { left arrow  }
         'G' : begin seg_top_pntr := 0;        ofs_top_pntr := 0; end;  { home }
         'O' : begin seg_top_pntr := endofmem; ofs_top_pntr := 0; end;  { end  }
         ';' : begin seg_top_pntr := $1000;    ofs_top_pntr := 0; end;  { F1   }
         '<' : begin seg_top_pntr := $2000;    ofs_top_pntr := 0; end;  { F2   }
         '=' : begin seg_top_pntr := $3000;    ofs_top_pntr := 0; end;  { F3   }
         '>' : begin seg_top_pntr := $4000;    ofs_top_pntr := 0; end;  { F4   }
         '?' : begin seg_top_pntr := $5000;    ofs_top_pntr := 0; end;  { F5   }
         '@' : begin seg_top_pntr := $6000;    ofs_top_pntr := 0; end;  { F6   }
         'A' : begin seg_top_pntr := $7000;    ofs_top_pntr := 0; end;  { F7   }
         'B' : begin seg_top_pntr := $8000;    ofs_top_pntr := 0; end;  { F8   }
         'C' : begin seg_top_pntr := $9000;    ofs_top_pntr := 0; end;  { F9   }
         'D' : begin seg_top_pntr := $A000;    ofs_top_pntr := 0; end;  { F10  }
         'T' : begin seg_top_pntr := $B000;    ofs_top_pntr := 0; end;  { s-F1 }
         'U' : begin seg_top_pntr := $C000;    ofs_top_pntr := 0; end;  { s-F2 }
         'V' : begin seg_top_pntr := $D000;    ofs_top_pntr := 0; end;  { s-F3 }
         'W' : begin seg_top_pntr := $E000;    ofs_top_pntr := 0; end;  { s-F4 }
         'X' : begin seg_top_pntr := $F000;    ofs_top_pntr := 0; end;  { s-F5 }
         'Y' : begin seg_top_pntr := $0000;    ofs_top_pntr := 0; end;  { s-F6 }
         else  beep;
      end;
   end
   else
      if key = esc then
         done := true
      else
         beep;

   memw[seg(top_pntr):ofs(top_pntr)+2] := seg_top_pntr;
   memw[seg(top_pntr):ofs(top_pntr)  ] := ofs_top_pntr;
   display_address(seg_top_pntr,ofs_top_pntr);

end;  { procedure readcommand }

{-------------------------------------------------------------}
begin
  done := false;
  initialize;
  memw[seg(top_pntr):ofs(top_pntr)+2] := initial_segment;
  memw[seg(top_pntr):ofs(top_pntr)  ] := initial_offset;
  repeat
    normalize(seg(top_pntr),ofs(top_pntr));
    pntr := top_pntr;
    memw[seg(scrn):ofs(scrn)+2] := screen_segment;
    memw[seg(scrn):ofs(scrn)  ] := screen_offset;
    for i := 0 to 1920 do
    begin
      scrn^ := pntr^;
      memw[seg(pntr):ofs(pntr)] := memw[seg(pntr):ofs(pntr)] + 1;
      memw[seg(scrn):ofs(scrn)] := memw[seg(scrn):ofs(scrn)] + 2;
    end;
    read_keyboard;
  until done;
  textbackground(black);
  textcolor(lightgray);
  clrscr;
  textbackground(blue);
  textcolor(lightred);
  gotoxy(1,1);
  write(' D-RAM');
  clreol;
  textcolor(lightcyan);
  gotoxy(10,1);
  write('Jonathan Gilmore 1988');
  writeln;
end.
