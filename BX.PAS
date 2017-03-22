{$R-}    {Range checking off}
{$B+}    {Boolean complete evaluation on}
{$S+}    {Stack checking on}
{$I+}    {I/O checking on}
{$N-}    {No numeric coprocessor}
{$M 65500,16384,655360} {Turbo 3 default stack and heap}

program bx;
{-----------------------------------------------}
{                                               }
{-----------------------------------------------}

Uses
  Crt,
  Dos;

{$i box.inc}
var x,y : byte;

{-----------------------------------------------}
{ Main program                                  }
{-----------------------------------------------}
begin
   textbackground(0);
   textcolor(7);
   clrscr;
   box(1,1,80,25,1);
end.
