{box.inc    Miscelaneous global subprograms}
procedure box(x,y,w,h : integer);

const
   top_left    = #218;
   top_right   = #191;
   bot_left    = #192;
   bot_right   = #217;
   horoz       = #196;
   vert        = #179;

var
   zi,x1      :  byte;
   top_lhc,
   top_rhc,
   bot_lhc,
   bot_rhc,
   ix,
   iy         : integer; {byte;}
   scr_seg    : word;

begin
   top_lhc := abs(((y - 1) * 80) + x - 1) * 2;
   bot_lhc := abs(((h - 1) * 80) + x - 1) * 2;
   top_rhc := abs(((y - 1) * 80) + w - 1) * 2;
   bot_rhc := abs(((h - 1) * 80) + w - 1) * 2;

   if (lastmode = mono) then
      scr_seg := $B000
   else
      scr_seg := $B800;

      mem[scr_seg:top_lhc] := ord(top_left);
      mem[scr_seg:top_lhc + 1] := textattr;

      mem[scr_seg:top_rhc] := ord(top_right);
      mem[scr_seg:top_rhc + 1] := textattr;
      mem[scr_seg:bot_lhc] := ord(bot_left);
      mem[scr_seg:bot_lhc + 1] := textattr;
      mem[scr_seg:bot_rhc] := ord(bot_right);
      mem[scr_seg:bot_rhc + 1] := textattr;

      for ix := 1 to abs(w - x - 1) do
      begin
           mem[scr_seg:top_lhc     + (2*ix)] := ord(horoz);
           mem[scr_seg:top_lhc + 1 + (2*ix)] := textattr;
           mem[scr_seg:bot_lhc     + (2*ix)] := ord(horoz);
           mem[scr_seg:bot_lhc + 1 + (2*ix)] := textattr;
      end;

      for iy := 1 to abs(h - y - 1) do
      begin
           mem[scr_seg:top_lhc     + (160*iy)] := ord(vert);
           mem[scr_seg:top_lhc + 1 + (160*iy)] := textattr;
           mem[scr_seg:top_rhc     + (160*iy)] := ord(vert);
           mem[scr_seg:top_rhc + 1 + (160*iy)] := textattr;
      end;
end;
