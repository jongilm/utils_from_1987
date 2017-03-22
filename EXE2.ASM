        DOSSEG
        .MODEL SMALL

        .STACK 100h

include bios.mac
include dos.mac

EXTRN  _OutChar : NEAR
EXTRN  _ClrWin  : NEAR
EXTRN  _Window  : NEAR

        .DATA
        PUBLIC  VideoSeg
VideoSeg DW 0B800h
string   DB 'Hello$'


        .CODE
start:  mov ax,@DATA
        mov ds,ax

        SetCursorSize 6,7
        ClrScrCol 57h
        OutChar 0,0,176,2000      ;'°'
        Window 5,5,74,19,3Fh,08h
        GotoXY 6,6
        Print string
        GotoXY 0,25
        KeyIn
        GotoXY 0,0
        ClrScr
        Exit 0

END     start
