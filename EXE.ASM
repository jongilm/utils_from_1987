DOSSEG
.MODEL SMALL
.STACK 100h

Exit    MACRO ExitCode
        MOV     AH,4Ch
        MOV     AL,ExitCode
        INT     21h
ENDM

OutStr  MACRO String
        MOV     AH,9
        MOV     DX,offset String
        INT     21h
ENDM

InKey   MACRO
        MOV     AH,0
        INT     16h  ; AL=ASCII code, AH=Scan code
ENDM


.DATA
string   DB 'Hello$'

.CODE
start:  mov ax,@DATA
        mov ds,ax

        OutStr string
        InKey
        Exit 0

END     start
