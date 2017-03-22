stackseg                segment  para  stack    'STACK'
                        db    512 dup(?)
stackseg                ends



HIRES_VIDEO_MODE        equ   0
VIDEO_SEGMENT           equ   0a000h
;LOGICAL_SCREEN_WIDTH    equ   640/8
;LOGICAL_SCREEN_HEIGHT   equ   350
LOGICAL_SCREEN_WIDTH    equ   672/8
LOGICAL_SCREEN_HEIGHT   equ   384
PAGE0                   equ   0
PAGE1                   equ   1
PAGE0_OFFSET            equ   0
PAGE1_OFFSET            equ   LOGICAL_SCREEN_WIDTH * LOGICAL_SCREEN_HEIGHT
BALL_WIDTH              equ   24/8
BALL_HEIGHT             equ   24
BLANK_OFFSET            equ   PAGE1_OFFSET * 2
BALL_OFFSET             equ   BLANK_OFFSET + (BALL_WIDTH*BALL_HEIGHT)
NUM_BALLS               equ   4

SC_INDEX                equ   3C4h
MAP_MASK                equ   2
GC_INDEX                equ   3CEh
GC_MODE                 equ   5
CRTC_INDEX              equ   3D4h
START_ADDRESS_HIGH      equ   0Ch
START_ADDRESS_LOW       equ   0Dh
CRTC_OFFSET             equ   13h
STATUS_REGISTER_1       equ   03Dah
VSYNC_MASK              equ   08h
AC_INDEX                equ   03C0h
HPELPAN                 equ   20h OR 13h

dseg                    segment  para  common   'DATA'

CurrentPage             db PAGE1
CurrentPageOffset       dw PAGE1_OFFSET

BallPlane0Image         label    byte
                        db    000h, 03Ch, 000h, 001h, 0FFh, 080h
                        db    007h, 0FFh, 0E0h, 00Fh, 0FFh, 0F0h
                        db    000h, 000h, 000h, 000h, 000h, 000h
                        db    000h, 000h, 000h, 000h, 000h, 000h
                        db    07fh, 0ffh, 0feh, 0ffh, 0ffh, 0ffh
                        db    0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh
                        db    000h, 000h, 000h, 000h, 000h, 000h
                        db    000h, 000h, 000h, 000h, 000h, 000h
                        db    07fh, 0ffh, 0feh, 03fh, 0ffh, 0fch
                        db    03fh, 0ffh, 0fch, 01fh, 0ffh, 0f8h
                        db    000h, 000h, 000h, 000h, 000h, 000h
                        db    000h, 000h, 000h, 000h, 000h, 000h

BallPlane1Image         label    byte
                        db    000h, 000h, 000h, 000h, 000h, 000h
                        db    000h, 000h, 000h, 000h, 000h, 000h
                        db    01fh, 0ffh, 0f8h, 03fh, 0ffh, 0fch
                        db    03fh, 0ffh, 0fch, 07fh, 0ffh, 0feh
                        db    07fh, 0ffh, 0feh, 0ffh, 0ffh, 0ffh
                        db    0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh
                        db    000h, 000h, 000h, 000h, 000h, 000h
                        db    000h, 000h, 000h, 000h, 000h, 000h
                        db    000h, 000h, 000h, 000h, 000h, 000h
                        db    000h, 000h, 000h, 000h, 000h, 000h
                        db    00fh, 0ffh, 0f0h, 007h, 0ffh, 0e0h
                        db    001h, 0ffh, 080h, 000h, 03ch, 000h

BallPlane2Image         label    byte
                        db    000h, 000h, 000h, 000h, 000h, 000h
                        db    000h, 000h, 000h, 000h, 000h, 000h
                        db    000h, 000h, 000h, 000h, 000h, 000h
                        db    000h, 000h, 000h, 000h, 000h, 000h
                        db    000h, 000h, 000h, 000h, 000h, 000h
                        db    000h, 000h, 000h, 000h, 000h, 000h
                        db    0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh
                        db    0ffh, 0ffh, 0ffh, 07fh, 0ffh, 0feh
                        db    07fh, 0ffh, 0feh, 03fh, 0ffh, 0fch
                        db    03fh, 0ffh, 0fch, 01fh, 0ffh, 0f8h
                        db    00fh, 0ffh, 0f0h, 007h, 0ffh, 0e0h
                        db    001h, 0ffh, 080h, 000h, 03ch, 000h

BallPlane3Image         label    byte
                        db    000h, 03ch, 000h, 001h, 0ffh, 080h
                        db    007h, 0ffh, 0e0h, 00fh, 0ffh, 0f0h
                        db    01fh, 0ffh, 0f8h, 03fh, 0ffh, 0fch
                        db    03fh, 0ffh, 0fch, 07fh, 0ffh, 0feh
                        db    07fh, 0ffh, 0feh, 0ffh, 0ffh, 0ffh
                        db    0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh
                        db    0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh
                        db    0ffh, 0ffh, 0ffh, 007h, 0ffh, 0feh
                        db    07fh, 0ffh, 0feh, 03fh, 0ffh, 0fch
                        db    03fh, 0ffh, 0fch, 01fh, 0ffh, 0f8h
                        db    00fh, 0ffh, 0f0h, 007h, 0ffh, 0e0h
                        db    001h, 0ffh, 080h, 000h, 03ch, 000h



BallX                   dw     15,  50,  40,  70
BallY                   dw     40, 200, 110, 300
LastBallX               dw     15,  50,  40,  70
LastBallY               dw     40, 100, 160,  30
BallXInc                dw      1,   1,   1,   1
BallYInc                dw      8,   8,   8,   8
BallRep                 dw      1,   1,   1,   1
BallControl             dw    Ball0Control, Ball1Control
                        dw    Ball2Control, Ball3Control
BallControlString       dw    Ball0Control, Ball1Control
                        dw    Ball2Control, Ball3Control

Ball0Control            label word
                        dw     10,  1,  4, 10, -1,  4, 10, -1, -4, 10, 1, -4, 0
Ball1Control            label word
                        dw     12, -1,  1, 28, -1, -1, 12,  1, -1, 28, 1,  1, 0
Ball2Control            label word
                        dw     20,  0, -1, 40,  0,  1, 20,  0, -1,  0
Ball3Control            label word
                        dw      8,  1,  0, 52, -1,  0, 44,  1,  0,  0

ifdef HIRES_VIDEO_MODE
PanningControlString    dw    32, 1, 0,  34, 0, 1, 32, -1, 0,  34, 0, -1, 0
else
PanningControlString    dw    32, 1, 0, 184, 0, 1, 32, -1, 0, 184, 0, -1, 0
endif

PanningControl          dw    PanningControlString
PanningRep              dw    1
PanningXInc             dw    1
PanningYInc             dw    0
HPan                    db    0
PanningStartOffset      dw    0

dseg                    ends


SETREG macro P1, P2
   mov   dx, P1
   mov   ah, al
   mov   al, P2
   out   dx, ax
endm

GETKEY macro
   mov   ah,0
   int   16h
endm

SET_GRAPHICS_MODE macro
ifdef HIRES_VIDEO_MODE
   mov ax,010h
else
   mov ax,0eh
endif
   int   10h
endm

SET_TEXT_MODE macro
   mov   ax,3
   int   10h
endm

TERMINATE_PROGRAM macro
   mov   ah,4Ch
   int   21h
endm

cseg                    segment  para  public   'CODE'
                        assume   cs:cseg, ds:dseg


start proc near
   mov ax,dseg
   mov ds,ax


   SET_GRAPHICS_MODE


   mov ax,VIDEO_SEGMENT
   mov es,ax

   mov di,PAGE0_OFFSET
   call DrawBorder
   mov di,PAGE1_OFFSET
   call DrawBorder

;PLANE 0
   mov al,01h
   SETREG SC_INDEX, MAP_MASK
   mov si,offset BallPlane0Image
   mov di,BALL_OFFSET
   mov cx,BALL_WIDTH * BALL_HEIGHT
   rep movsb

;PLANE 1
   mov al,02h
   SETREG SC_INDEX, MAP_MASK
   mov si,offset BallPlane1Image
   mov di,BALL_OFFSET
   mov cx,BALL_WIDTH * BALL_HEIGHT
   rep movsb

;PLANE 2
   mov al,04h
   SETREG SC_INDEX, MAP_MASK
   mov si,offset BallPlane2Image
   mov di,BALL_OFFSET
   mov cx,BALL_WIDTH * BALL_HEIGHT
   rep movsb

;PLANE 3
   mov al,08h
   SETREG SC_INDEX, MAP_MASK
   mov si,offset BallPlane3Image
   mov di,BALL_OFFSET
   mov cx,BALL_WIDTH * BALL_HEIGHT
   rep movsb

;BLANK IMAGE
   mov al,0fh
   SETREG SC_INDEX, MAP_MASK
   mov di,BLANK_OFFSET
   mov cx,BALL_WIDTH * BALL_HEIGHT
   sub al,al
   rep stosb

; SET WRITE MODE 1
   mov al,1
   SETREG GC_INDEX, GC_MODE

; SET LOGICAL SCREEN WIDTH
   mov al,LOGICAL_SCREEN_WIDTH/2
   SETREG CRTC_INDEX, CRTC_OFFSET


BallAnimationLoop:
   mov bx,(NUM_BALLS * 2) - 2

EachBallLoop:
; ERASE OLD IMAGE OF BALL
   mov si,BLANK_OFFSET
   mov cx,[LastBallX+bx]
   mov dx,[LastBallY+bx]
   call DrawBall

   mov ax,[BallX+bx]
   mov [LastBallX+bx],ax
   mov ax,[BallY+bx]
   mov [LastBallY+bx],ax

   dec [BallRep+bx]
   jnz MoveBall
   mov si,[BallControl+bx]

   lodsw

   and ax,ax
   jnz SetNewMove
   mov si,[BallControlString+bx]
   lodsw
SetNewMove:

   mov [BallRep+bx],ax
   lodsw
   mov [BallXInc+bx],ax
   lodsw
   mov [BallYInc+bx],ax
   mov [BallControl+bx],si


MoveBall:
   mov ax,[BallXInc+bx]
   add [BallX+bx],ax
   mov ax,[BallYInc+bx]
   add [BallY+bx],ax

; DRAW BALL AT NEW LOCATION

   mov si,BALL_OFFSET
   mov cx,[BallX+bx]
   mov dx,[BallY+bx]
   call DrawBall

   dec bx
   dec bx
   jns EachBallLoop

; PAN AND FLIP DISPLAYED PAGE TO THE ONE WE'VE BEEN WORKING ON

   call WaitVSync

; DO PANNING

   call AdjustPanning

; FLIP TO NEW PAGE AND CHANGE STARTING ADDRESS

   mov ax,[CurrentPageOffset]
   add ax,[PanningStartOffset]
   push ax
   SETREG CRTC_INDEX, START_ADDRESS_LOW
   pop ax
   mov al,ah
   SETREG CRTC_INDEX, START_ADDRESS_HIGH

   call WaitVSync

; SET HORIZ PANNING NOW, JUST AS NEW ADDRESS TAKES EFFECT

   mov al,[HPan]
   mov dx,STATUS_REGISTER_1
   in al,dx
   mov dx,AC_INDEX
   mov al,HPELPAN
   out dx,al
   mov al,[HPan]
   out dx,al

; FLIP THE PAGE TO DRAW THE UNDISPLAYED PAGE

   xor [CurrentPage],1
   jnz IsPage1
   mov [CurrentPageOffset],PAGE0_OFFSET
   jmp short EndFlipPage
IsPage1:
   mov [CurrentPageOffset],PAGE1_OFFSET
EndFlipPage:

; EXIT IF A KEY HAS BEEN HIT

   mov ah,1
   int 16h
   jnz Done
   jmp BallAnimationLoop

Done:
   GETKEY
   SET_TEXT_MODE
   TERMINATE_PROGRAM

start endp





DrawBall proc near
   mov ax,LOGICAL_SCREEN_WIDTH
   mul dx
   add ax,cx
   add ax,[CurrentPageOffset]
   mov di,ax
   mov bp,BALL_HEIGHT
   push ds
   push es
   pop  ds
DrawBallLoop:
   push di
   mov cx,BALL_WIDTH
   rep movsb
   pop di
   add di,LOGICAL_SCREEN_WIDTH
   dec bp
   jnz DrawBallLoop
   pop ds
   ret
DrawBall endp




WaitVSync proc near
   mov dx,STATUS_REGISTER_1
WaitNotVSyncLoop:
   in al,dx
   and al,VSYNC_MASK
   jnz WaitNotVSyncLoop
WaitVSyncLoop:
   in al,dx
   and al,VSYNC_MASK
   jz WaitVSyncLoop
   ret
WaitVSync endp




AdjustPanning proc near
   dec [PanningRep]
   jnz DoPan
   mov si,[PanningControl]
   lodsw
   and ax,ax
   jnz SetNewPanValues
   mov si, offset PanningControlString
   lodsw
SetNewPanValues:
   mov [PanningRep],ax
   lodsw
   mov [PanningXInc],ax
   lodsw
   mov [PanningYInc],ax
   mov [PanningControl],si

; PAN ACCORDING TO PANNING VALUES

DoPan:
   mov ax,[PanningXInc]
   and ax,ax
   js  PanLeft
   jz  CheckVerticalPan
   mov al,[HPan]
   inc al
   cmp al,8
   jb SetHPan
   sub al,al
   inc [PanningStartOffset]
   jmp short SetHPan
PanLeft:
   mov al,[HPan]
   dec al
   jns SetHPan
   mov al,7
   dec [PanningStartOffset]
SetHPan:
   mov [HPan],al
CheckVerticalPan:
   mov ax,[PanningYInc]
   and ax,ax
   js PanUp
   jz EndPan
   add [PanningStartOffset],LOGICAL_SCREEN_WIDTH
   jmp short EndPan
PanUp:
   sub [PanningStartOffset],LOGICAL_SCREEN_WIDTH

EndPan:
   ret




DrawBorder proc near

; LEFT

   push di
   mov  cx,LOGICAL_SCREEN_HEIGHT / 16

DrawLeftBorderLoop:
   mov  al,0ch
   call DrawBorderBlock
   add  di,LOGICAL_SCREEN_WIDTH * 8
   mov  al,0eh
   call DrawBorderBlock
   add  di,LOGICAL_SCREEN_WIDTH * 8
   loop DrawLeftBorderLoop
   pop  di

; RIGHT

   push di
   add  di,LOGICAL_SCREEN_WIDTH - 1
   mov  cx,LOGICAL_SCREEN_HEIGHT / 16


DrawRightBorderLoop:
   mov  al,0eh
   call DrawBorderBlock
   add  di,LOGICAL_SCREEN_WIDTH * 8
   mov  al,0ch
   call DrawBorderBlock
   add  di,LOGICAL_SCREEN_WIDTH * 8
   loop DrawRightBorderLoop
   pop  di


; TOP

   push di
   mov  cx,(LOGICAL_SCREEN_WIDTH - 2) / 2

DrawTopBorderLoop:
   inc  di
   mov  al,0eh
   call DrawBorderBlock
   inc  di
   mov  al,0ch
   call DrawBorderBlock
   loop DrawTopBorderLoop
   pop  di


; BOTTOM

   add  di,(LOGICAL_SCREEN_HEIGHT - 8) * LOGICAL_SCREEN_WIDTH
   mov  cx,(LOGICAL_SCREEN_WIDTH - 2) / 2


DrawBottomBorderLoop:
   inc  di
   mov  al,0ch
   call DrawBorderBlock
   inc  di
   mov  al,0eh
   call DrawBorderBlock
   loop DrawBottomBorderLoop
   ret

DrawBorder endp

DrawBorderBlock proc near
   push di
   SETREG SC_INDEX, MAP_MASK
   mov  al,0ffh
   rept 8
   stosb
   add  di,LOGICAL_SCREEN_WIDTH - 1
   endm
   pop  di
   ret
DrawBorderBlock endp

AdjustPanning endp

cseg ends
     end start
