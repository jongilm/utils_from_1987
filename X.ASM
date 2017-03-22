code      segment para public 'CODE'
          assume  CS:code, DS:code, ES:code
          ORG     80h
cmd_buff  LABEL   BYTE
          ORG     100h
strt:
          mov     BX,OFFSET cmd_buff
          CALL    argc                  ;get number of command line parameters
          mov     AH,4Ch
          int     21h                   ;TERMINATE


ARGC PROC ;------------------------------Return number of args in AX
          push bx
          mov ax,0
          mov ch,0
          mov cl,byte ptr [bx]
          cmp cl,0
          je argc_end
argc_space:
          dec cl
          cmp cl,0
          je argc_end
          inc bx
          mov dl,byte ptr [bx]

          cmp dl,' '
          je argc_space
argc_arg:
          inc ax
argc_char:
          dec cl
          cmp cl,0
          je argc_end
          inc bx
          mov dl,byte ptr [bx]

          cmp dl,' '
          jne argc_char
          jmp argc_space
ARGC_end:
          inc ax
          pop bx
          ret
ARGC ENDP ;----------------------------------------------------------------

Code     ends
         end     strt
