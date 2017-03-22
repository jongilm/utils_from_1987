
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³ ASY.ASM                                                 ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

code_seg_a      segment byte
		assume	cs:code_seg_a, ds:code_seg_a
		org	100h
  
asy		proc	far
Start:
                mov     dx,offset Filename
                mov     ax,3D02h      ; DOS Open file, Read/Write mode
                int     21h
                mov     FileHandle,ax
                jc      Terminate     ; Jump if carry Set
MainLoop:
                call    ReadAsync
                call    WriteConsole
                call    ReadConsole
                call    WriteAsync
                jmp     short MainLoop
Terminate:
                mov     ah,4Ch        ; DOS - Terminate program
                int     21h
asy             endp

ReadAsync       proc    near
                mov     cx,100h       ; Number of bytes to read
                mov     bx,FileHandle ; File handle
                mov     dx,offset InBuffer    ; buffer
                mov     ax,3F00h      ; DOS - Read file
                int     21h           ; DOS Services  ah=function 3Fh
                                      ;  read file, cx=bytes, to ds:dx
                jc      Terminate     ; Jump if carry Set
                mov     BytesRead,ax  ; Number of bytes read
                ret
ReadAsync       endp


ReadConsole     proc    near
                mov     word ptr [KeysPressed],0
                mov     ah,0Bh        ; DOS - if Keypressed
                int     21h           ; DOS Services  ah=function 0Bh
                                      ;  if keybd char available,al=FF
                inc     al
                jnz     ReadConsole_ret   ; Jump if not zero
                mov     ah,7
                int     21h           ; DOS Services  ah=function 07h
                                      ;  get keybd char al, no echo
                cmp     al,3          ; Ctrl+C ?
                je      Terminate     ; Jump if equal
                mov     byte ptr [KeyIn],al
                inc     word ptr [KeysPressed]
                cmp     al,0Dh
                jne     ReadConsole_ret   ; Jump if not equal
                mov     byte ptr [KeyIn2],0Ah
                inc     word ptr [KeysPressed]
ReadConsole_ret:
                ret
ReadConsole     endp


WriteAsync      proc    near
                mov     cx,KeysPressed
                jcxz    WriteAsync_ret     ; Jump if cx=0
                mov     bx,FileHandle
                mov     dx,offset Keyin
                mov     ax,4000h
                int     21h           ; DOS Services  ah=function 40h
                                      ;  write file cx=bytes, to ds:dx
                jc      Terminate     ; Jump if carry Set

WriteAsync_ret:
                ret
WriteAsync      endp


WriteConsole    proc    near
                mov     cx,BytesRead
                jcxz    WriteConsole_ret     ; Jump if cx=0
                mov     bx,1          ; Console
                mov     dx,offset InBuffer
                mov     ah,40h        ; '@'
                int     21h           ; DOS Services  ah=function 40h
                                      ;  write file cx=bytes, to ds:dx
                jc      Terminate     ; Jump if carry Set
WriteConsole_ret:
		ret
WriteConsole    endp
  
Filename        db      'ASY1', 0
FileHandle      dw      0
KeysPressed     dw      0
BytesRead       dw      0
InBuffer        db      256 dup (0)
KeyIn           db      0
KeyIn2          db      0
		db	78 dup (0)
code_seg_a	ends
                end     Start
