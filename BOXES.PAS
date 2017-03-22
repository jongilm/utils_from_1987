{$R-}    {Range checking off}
{$B+}    {Boolean complete evaluation on}
{$S+}    {Stack checking on}
{$I+}    {I/O checking on}
{$N-}    {No numeric coprocessor}
{$M 65500,16384,655360} {Turbo 3 default stack and heap}

program boxes;
{-----------------------------------------------}
{                                               }
{-----------------------------------------------}

Uses
  Crt,
  Dos;

{$i box.inc}
var x,y,z,max_col : byte;

{-----------------------------------------------}
{ Main program                                  }
{-----------------------------------------------}
begin
   textbackground(0);
   textcolor(7);
   clrscr;
   case lastmode of
      mono,
      co80,
      bw80 : max_col := 80;
      co40,
      bw40 : max_col := 40;
   end;
   randomize;
   repeat
{      delay(50);}
      textbackground(0{random(16)});
      textcolor(random(16));
      x := random((max_col div 2));
      y := random(12);
      z := random(8);
      box(x+1,y+1,max_col-x,25-y,z);
      window(x+2,y+2,max_col-x-1,25-y-1                                                      );
      clrscr;
   until keypressed;

      box(1,1,80,25,2);
  textbackground(0);
   textcolor(7);
end.
