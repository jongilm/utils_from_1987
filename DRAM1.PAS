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
  val_error         : integer;
  done              : boolean;
  mptr, scrn, pntr  : ^char;

procedure beep;
begin
   sound(760);
   delay(50);
   nosound;
end; {beep}

procedure initialize;
begin
  initial_segment := 0;
  if paramcount > 0 then
  begin
     val(paramstr(1),initial_segment,val_error);

  end;
  textbackground(black);
  textcolor(white);
  clrscr;
  textbackground(blue);
  textcolor(lightred);
  gotoxy(1,1);
  write(' D-RAM');
  clreol;
  textcolor(lightcyan);
  gotoxy(10,1);
  write('Display Address = ');
  textcolor(yellow);
  write('B800:0000');
  textcolor(lightcyan);
  gotoxy(40,1);
  write('Memory Address = ');
  textcolor(yellow);
end;
{-------------------------------------------------------------}

procedure adjust(pseg, pofs : integer);
var
  segadr, ofsadr : integer;
begin
  segadr := memw[pseg:pofs+2];
  ofsadr := memw[pseg:pofs  ];

  if (ofsadr < 0) then
    begin
      ofsadr := 15;
      segadr := segadr - 1;
    end;
  segadr := (segadr + (ofsadr shr 4));
  ofsadr := (ofsadr and 15);

  memw[pseg:pofs  ] := ofsadr;
  memw[pseg:pofs+2] := segadr;
end;

{-------------------------------------------------------------}

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

procedure display_address;
begin
    gotoxy(57,1);
    write(hexstring(memw[seg(mptr):ofs(mptr)+2]));
    write(':');
    write(hexstring(memw[seg(mptr):ofs(mptr)]));
end;

{-------------------------------------------------------------}

procedure read_keyboard;
const
  pagesz = 120;  {120 segments = 1920 bytes = 24 lines of chars}
  linesz = 5;    {  5 segments =   80 bytes =  1 line  of chars}
  charsz = 1;    {  1 char }
var
  key              : char;
  segmptr, ofsmptr : word;

begin
   segmptr := memw[seg(mptr):ofs(mptr)+2];
   ofsmptr := memw[seg(mptr):ofs(mptr)];
   key := readkey;
   if key = #0 then
   begin
      key := readkey;
      case key of
         'Q' : segmptr := segmptr + pagesz;  { page dn }
         'I' : segmptr := segmptr - pagesz;  { page up }
         'P' : segmptr := segmptr + linesz;  { down arrow }
         'H' : segmptr := segmptr - linesz;  { up arrow   }
         'M' : ofsmptr := ofsmptr + charsz;  { right arrow }
         'K' : ofsmptr := ofsmptr - charsz;  { left arrow  }
         'G' : begin segmptr := 0;        ofsmptr := 0; end;  { home }
         'O' : begin segmptr := endofmem; ofsmptr := 0; end;  { end  }
         ';' : begin segmptr := $1000;    ofsmptr := 0; end;  { F1   }
         '<' : begin segmptr := $2000;    ofsmptr := 0; end;  { F2   }
         '=' : begin segmptr := $3000;    ofsmptr := 0; end;  { F3   }
         '>' : begin segmptr := $4000;    ofsmptr := 0; end;  { F4   }
         '?' : begin segmptr := $5000;    ofsmptr := 0; end;  { F5   }
         '@' : begin segmptr := $6000;    ofsmptr := 0; end;  { F6   }
         'A' : begin segmptr := $7000;    ofsmptr := 0; end;  { F7   }
         'B' : begin segmptr := $8000;    ofsmptr := 0; end;  { F8   }
         'C' : begin segmptr := $9000;    ofsmptr := 0; end;  { F9   }
         'D' : begin segmptr := $A000;    ofsmptr := 0; end;  { F10  }
         'T' : begin segmptr := $B000;    ofsmptr := 0; end;  { s-F1 }
         'U' : begin segmptr := $C000;    ofsmptr := 0; end;  { s-F2 }
         'V' : begin segmptr := $D000;    ofsmptr := 0; end;  { s-F3 }
         'W' : begin segmptr := $E000;    ofsmptr := 0; end;  { s-F4 }
         'X' : begin segmptr := $F000;    ofsmptr := 0; end;  { s-F5 }
         'Y' : begin segmptr := $0000;    ofsmptr := 0; end;  { s-F6 }
         else  beep;
      end;
   end
   else
      if key = esc then
         done := true
      else
         beep;

   memw[seg(mptr):ofs(mptr)+2] := segmptr;
   memw[seg(mptr):ofs(mptr)  ] := ofsmptr;

end;  { procedure readcommand }

{-------------------------------------------------------------}

begin
  done := false;
  initialize;
  memw[seg(mptr):ofs(mptr)+2] := initial_segment;
  memw[seg(mptr):ofs(mptr)  ] := 0;
  repeat
    adjust(seg(mptr),ofs(mptr));
    pntr := mptr;
    display_address;
    memw[seg(scrn):ofs(scrn)+2] := colorgraphscreen;
    memw[seg(scrn):ofs(scrn)] := 0;
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