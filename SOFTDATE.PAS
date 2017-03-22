{$R-}    {Range checking off}
{$B+}    {Boolean complete evaluation on}
{$S+}    {Stack checking on}
{$I+}    {I/O checking on}
{$N-}    {No numeric coprocessor}
{$M 65500,16384,655360} {Turbo 3 default stack and heap}

program sdate;


Uses
  Crt,
  Dos;

const
   black   = 0;
   blue    = 1;
   green   = 2;
   cyan    = 3;
   red     = 4;
   magenta = 5;
   brown   = 6;
   white   = 7;

   weekday   : array[0..6] of string[9]
             = ('Sunday','Monday','Tuesday','Wednesday',
                'Thursday','Friday','Saturday');

type
   strng2  = string[2];
   strng4  = string[4];
   strng10 = string[10];
var
   datefile  : text;

   i,
   val_error : integer;

   dayofweek,
   leap,
   year,
   month,
   day       : word;

   new_date,
   date      : strng10;
   year_str  : strng4;
   month_str : strng2;
   day_str   : strng2;


procedure beep;
{-----------------------------------------------}
{                                               }
{-----------------------------------------------}
begin
   sound(760);
   delay(50);
   nosound;
end; {beep}

function bright(colour : byte) : byte;
{-----------------------------------------------}
{                                               }
{-----------------------------------------------}
begin
   bright := colour + 8;
end;

procedure display_banner;
begin
   textbackground(black);
   textcolor(white);
   gotoxy(8, 3); write('лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл');
   gotoxy(8, 4); write('лллллппппппппппппппппппппппппппппппппппппппппппппппппппппппллллл');
   gotoxy(8, 5); write('лллллоллллллллллллллллллллллллллллллллллллллллллллллллллллнллллл');
   gotoxy(8, 6); write('лллллолллллллллл                                ллллллллллнллллл');
   gotoxy(8, 7); write('лллллолллллллллл                                Блллллллллнллллл');
   gotoxy(8, 8); write('лллллолллллллллл                                Блллллллллнллллл');
   gotoxy(8, 9); write('лллллолллллллллл                                Блллллллллнллллл');
   gotoxy(8,10); write('лллллолллллллллл                                Блллллллллнллллл');
   gotoxy(8,11); write('лллллолллллллллллББББББББББББББББББББББББББББББББлллллллллнллллл');
   gotoxy(8,12); write('лллллоллллллллллллллллллллллллллллллллллллллллллллллллллллнллллл');
   gotoxy(8,13); write('лллллммммммммммммммммммммммммммммммммммммммммммммммммммммммллллл');
   gotoxy(8,14); write('лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл');
   textcolor(bright(cyan));
   gotoxy(24,7); write('           SoftDate             ');
   textcolor(bright(red));
   gotoxy(24,9); write('   Jonathan Gilmore  -  1988    ');
end;

procedure read_date;
{-----------------------------------------------}
{                                               }
{-----------------------------------------------}
begin
   {$i-}
   reset(datefile);
   {$i+}
   if IOResult = 0 then
   begin
      readln(datefile,year);
      readln(datefile,month);
      readln(datefile,day);
   end
   else
   begin
      rewrite(datefile);
      year := 1980;
      month := 1;
      day := 1;
   end;
   close(datefile);
end;

procedure rebuild_date;
{-----------------------------------------------}
{                                               }
{-----------------------------------------------}
begin
  date := concat(new_date,copy(date,length(new_date)+1,10-length(new_date)));
end;

function date_ok : boolean;
{-----------------------------------------------}
{                                               }
{-----------------------------------------------}
begin
   val(copy(date,7,4),year,val_error);
   if val_error <> 0 then
      date_ok := false
   else
   begin
      val(copy(date,4,2),month,val_error);
      if val_error <> 0 then
         date_ok := false
      else
      begin
         val(copy(date,1,2),day,val_error);
         if val_error <> 0 then
            date_ok := false
         else
         begin
            case month of
               1,3,5,7,8,10,12 :
                   date_ok := (day >= 1) and (day <= 31);

               4,6,9,11 :
                   date_ok := (day >= 1) and (day <= 30);

               2  : begin
                      leap := year mod 4;
                      if (leap = 0) and (year <> 0) then
                         date_ok := (day >= 1) and (day <= 29)
                      else
                         date_ok := (day >= 1) and (day <= 28);
                   end;
            else
               date_ok := false;
            end;
         end;
      end;
   end;
end;

procedure get_date;
{-----------------------------------------------}
{                                               }
{-----------------------------------------------}
begin
   textbackground(black);
   str(year:4,year_str);
   str(month:2,month_str);
   str(day:2,day_str);
   repeat
      date := concat(day_str,'/',month_str,'/',year_str);
      for i := 1 to 10 do
         if copy(date,i,1) = ' ' then
            date := concat(copy(date,1,i-1),'0',copy(date,i+1,length(date)));
      textcolor(cyan);
      gotoxy(20,20); write('Enter date : ');
      beep;
      textcolor(bright(cyan));
      gotoxy(33,20); write(date);
      gotoxy(33,20); readln(new_date);
      rebuild_date;
   until date_ok;
end;

procedure set_date;
{-----------------------------------------------}
{                                               }
{-----------------------------------------------}
begin
   setdate(year,month,day);
   getdate(year,month,day,dayofweek);
   gotoxy(45,20);
   write(weekday[dayofweek]);
end;

procedure write_date;
{-----------------------------------------------}
{                                               }
{-----------------------------------------------}
begin
   rewrite(datefile);
   writeln(datefile,year);
   writeln(datefile,month);
   writeln(datefile,day);
   close(datefile);
end;

{-----------------------------------------------}
{ Main program                                  }
{-----------------------------------------------}
BEGIN
assign(datefile,'\softdate.dat');
clrscr;
display_banner;
read_date;
get_date;
set_date;
write_date;
END.
