{beep.inc   Miscelaneous global subprograms}
{box.inc    Miscelaneous global subprograms}

procedure beep;
begin
   sound(760);
   delay(50);
   nosound;
end;

procedure box(x1,y1,x2,y2,border : byte);

var
   top_left,
   top_right,
   bot_left,
   bot_right,
   horoz_top,
   horoz_bot,
   vert_left,
   vert_right,
   top_lhc,
   top_rhc,
   bot_lhc,
   bot_rhc,
   ndx,
   scr_seg    : word;
   max_col    : byte;

procedure validate_input;
begin
   if (x1 > x2) then
   begin
      ndx := x1;
      x1  := x2;
      x2  := ndx;
   end;
   if (y1 > y2) then
   begin
      ndx := y1;
      y1  := y2;
      y2  := ndx;
   end;

   if (x1 < 1)       then x1 := 1;
   if (y1 < 1)       then y1 := 1;
   if (x1 > max_col) then x1 := max_col;
   if (y1 > 25)      then y1 := 25;
   if (x2 < 1)       then x2 := 1;
   if (y2 < 1)       then y2 := 1;
   if (x2 > max_col) then x2 := max_col;
   if (y2 > 25)      then y2 := 25;

   if (x1 = x2) then
      if (x2 < max_col) then
         x2 := x2 + 1
      else
         x1 := x1 - 1;
   if (y1 = y2) then
      if (y2 < 25) then
         y2 := y2 + 1
      else
         y1 := y1 - 1;
end;

procedure define_chars;
begin
   case border of
      1 : begin {single}
             lo(top_left)    := 218;
             lo(top_right)   := 191;
             lo(bot_left)    := 192;
             lo(bot_right)   := 217;
             lo(horoz_top)   := 196;
             lo(horoz_bot)   := 196;
             lo(vert_left)   := 179;
             lo(vert_right)  := 179;
          end;
      2 : begin {double}
             lo(top_left)    := 201;
             lo(top_right)   := 187;
             lo(bot_left)    := 200;
             lo(bot_right)   := 188;
             lo(horoz_top)   := 205;
             lo(horoz_bot)   := 205;
             lo(vert_left)   := 186;
             lo(vert_right)  := 186;
          end;
      3 : begin {big blocks}
             lo(top_left)    := 219;
             lo(top_right)   := 219;
             lo(bot_left)    := 219;
             lo(bot_right)   := 219;
             lo(horoz_top)   := 219;
             lo(horoz_bot)   := 219;
             lo(vert_left)   := 219;
             lo(vert_right)  := 219;
          end;
      4 : begin {little blocks}
             lo(top_left)    := 219;
             lo(top_right)   := 219;
             lo(bot_left)    := 219;
             lo(bot_right)   := 219;
             lo(horoz_top)   := 223;
             lo(horoz_bot)   := 220;
             lo(vert_left)   := 221;
             lo(vert_right)  := 222;
          end;
      5 : begin {furry blocks}
             lo(top_left)    := 176;
             lo(top_right)   := 176;
             lo(bot_left)    := 176;
             lo(bot_right)   := 176;
             lo(horoz_top)   := 176;
             lo(horoz_bot)   := 176;
             lo(vert_left)   := 176;
             lo(vert_right)  := 176;
          end;
      6 : begin {furry blocks}
             lo(top_left)    := 177;
             lo(top_right)   := 177;
             lo(bot_left)    := 177;
             lo(bot_right)   := 177;
             lo(horoz_top)   := 177;
             lo(horoz_bot)   := 177;
             lo(vert_left)   := 177;
             lo(vert_right)  := 177;
          end;
      7 : begin {furry blocks}
             lo(top_left)    := 178;
             lo(top_right)   := 178;
             lo(bot_left)    := 178;
             lo(bot_right)   := 178;
             lo(horoz_top)   := 178;
             lo(horoz_bot)   := 178;
             lo(vert_left)   := 178;
             lo(vert_right)  := 178;
          end;
   else   begin {single}
             lo(top_left)    := 218;
             lo(top_right)   := 191;
             lo(bot_left)    := 192;
             lo(bot_right)   := 217;
             lo(horoz_top)   := 196;
             lo(horoz_bot)   := 196;
             lo(vert_left)   := 179;
             lo(vert_right)  := 179;
          end;
   end;

   hi(top_left)    := textattr;
   hi(top_right)   := textattr;
   hi(bot_left)    := textattr;
   hi(bot_right)   := textattr;
   hi(horoz_top)   := textattr;
   hi(horoz_bot)   := textattr;
   hi(vert_left)   := textattr;
   hi(vert_right)  := textattr;
end;

begin
   case lastmode of
   mono : begin
             scr_seg := $B000;
             max_col := 80;
          end;
   co80,
   bw80 : begin
             scr_seg := $B800;
             max_col := 80;
          end;
   co40,
   bw40 : begin
             scr_seg := $B800;
             max_col := 40;
          end;
   end;

   validate_input;
   define_chars;
   top_lhc := (((y1 - 1) * max_col) + x1 - 1) * 2;
   bot_lhc := (((y2 - 1) * max_col) + x1 - 1) * 2;
   top_rhc := (((y1 - 1) * max_col) + x2 - 1) * 2;
   bot_rhc := (((y2 - 1) * max_col) + x2 - 1) * 2;


      memw[scr_seg:top_lhc] := top_left;
      memw[scr_seg:top_rhc] := top_right;
      memw[scr_seg:bot_lhc] := bot_left;
      memw[scr_seg:bot_rhc] := bot_right;

      for ndx := 1 to (x2 - x1 - 1) do
      begin
           memw[scr_seg:top_lhc + (2*ndx)] := horoz_top;
           memw[scr_seg:bot_lhc + (2*ndx)] := horoz_bot;
      end;

      for ndx := 1 to (y2 - y1 - 1) do
      begin
           memw[scr_seg:top_lhc + (max_col*2*ndx)] := vert_left;
           memw[scr_seg:top_rhc + (max_col*2*ndx)] := vert_right;
      end;
end;
