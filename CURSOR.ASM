SHIFT_STATUS    equ     417h

code_seg_a      segment
                assume  cs:code_seg_a, ds:code_seg_a
                org     100h

bitcurs         proc    far
start:
                jmp     short install
                db      90h
int09ofs        dw      0
int09seg        dw      0
int10off        dw      0
int10seg        dw      0

int_09h_entry   proc    far
                pushf                            ; Push flags
                call    dword ptr cs:int09ofs
                push    ax
                push    ds
                xor     ax,ax                    ; Zero register
                mov     ds,ax
                mov     ax,ds:SHIFT_STATUS
                and     ax,0Fh
                cmp     ax,0Ah                   ; Alt + LShift
                jne     no_hotkey                ; Jump if not equal

                push    cx
                mov     cx,20Fh
                mov     ah,1
                int     10h                      ; Video display   ah=functn 01h ;  set cursor mode in cx
                pop     cx
no_hotkey:
                pop     ds
                pop     ax
                iret                             ; Interrupt return
int_09h_entry   endp
  
int_10h_entry   proc    far
                pushf                                   ; Push flags
                cmp     ah,1
                je      setcursor                       ; Jump if equal
chain_int10:
                popf                                    ; Pop flags
                jmp     dword ptr cs:int10off
setcursor:
                cmp     ch,10h
                ja      chain_int10                     ; Jump if above
                cmp     cx,0F00h
                je      chain_int10                     ; Jump if equal
                cmp     ch,6
                jb      loc_4                           ; Jump if below
                mov     ch,2
                jmp     short loc_5
                db      90h
loc_4:
                mov     ch,0
loc_5:
                mov     cl,0Fh
                pushf                                   ; Push flags
                call    dword ptr cs:int10off
                popf                                    ; Pop flags
                iret                                    ; Interrupt return
int_10h_entry   endp
  
install:
                mov     ax,3510h
                int     21h                            ; get intrpt vector 10 in es:bx
                mov     int10off,bx
                mov     int10seg,es

                mov     ax,2510h
                mov     dx, offset int_10h_entry
                int     21h                            ; set intrpt vector 10 to ds:dx

                mov     ax,3509h
                int     21h                            ; get intrpt vector 09 in es:bx
                mov     word ptr int09ofs,bx
                mov     int09seg,es

                mov     ax,2509h
                mov     dx, offset int_09h_entry
                int     21h                            ; set intrpt vector al to ds:dx

                mov     dx,offset banner
                mov     ah,9
                int     21h                            ; display char string at ds:dx

                mov     cx,607h
                mov     ah,1
                int     10h                            ; set cursor mode in cx

                mov     dx,offset install
                int     27h                            ; Terminate & stay resident

banner          db      'Laptop cursor TSR installed  -  Alt+Lshift to reset cursor', 0Dh, 0Ah, '$'

bitcurs         endp
code_seg_a      ends
end             start
