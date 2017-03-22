{$R-}    {Range checking off}
{$B+}    {Boolean complete evaluation on}
{$S+}    {Stack checking on}
{$I+}    {I/O checking on}
{$N-}    {No numeric coprocessor}
{$M 65500,16384,655360} {Turbo 3 default stack and heap}

program curset;

Uses
  Dos;

type
   strng3      = string[3];

var
   reg             :  registers;

   max_lines,
   cur_begin,
   cur_end         :  byte;

   error,
   cur_off,
   cur_set         : boolean;

   val_error1,
   val_error2      : integer;

function mono : boolean;
begin
   reg.ah := $0F;
   intr($10,Dos.Registers(reg));
   if reg.al = 7 then
      mono := true
   else
      mono := false;
end;

function valid_parms(num1,num2 : byte) : boolean;
begin
   val(paramstr(num1),cur_begin,val_error1);
   val(paramstr(num2),cur_end,val_error2);
   if (val_error1 =  0) and
      (val_error2 =  0) and
      (cur_begin >=  0) and
      (cur_begin <= max_lines) and
      (cur_end   >=  0) and
      (cur_end   <= max_lines) then
      valid_parms := true
   else
      valid_parms := false;
end;

procedure get_parameters;
begin
   if paramcount >= 1 then
      if (paramstr(1) = 'off') or (paramstr(1) = 'OFF') then
         if paramcount = 1 then
            cur_off := true
         else
            error   := true
      else
         if (paramstr(1) = 'on') or (paramstr(1) = 'ON') then
         begin
            if paramcount > 1 then
               if  (paramcount = 3  )
               and (valid_parms(2,3)) then
                  cur_set := true
               else
                  error  := true;
         end
         else
            if  (paramcount = 2  )
            and (valid_parms(1,2)) then
               cur_set := true
            else
               error := true;
end;

procedure set_cursor;
begin
   if cur_set then
   begin
      reg.ch := cur_begin;
      reg.cl := cur_end;
   end
   else
      if mono then
      begin
         reg.ch := 11;
         reg.cl := 12;
      end
      else
      begin
         reg.ch := 6;
         reg.cl := 7;
      end;

   if cur_off then
      reg.ch := reg.ch or 32
   else
      reg.ch := reg.ch and not 32;

   reg.ah := $01;
   intr($10,Dos.Registers(reg));
end;

procedure help;
begin
   writeln('Set the DOS cursor on/off and optionally change the shape.');
   writeln;
   writeln('Usage : CURset [ON] [begin end]');
   writeln('   or : CURset OFF');
   writeln;
   writeln('where : begin, end = 0..12 for monochrome');
   writeln('                   = 0..7  for colour');
end;


begin
   error   := false;
   cur_off := false;
   cur_set := false;
   if mono then
      max_lines := 12
   else
      max_lines := 7;
   get_parameters;
   writeln;
   writeln('CURset  -  Jonathan Gilmore 1988');
   writeln;
   if error then
      help
   else
      set_cursor;
end.
