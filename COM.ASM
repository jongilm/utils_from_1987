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

_TEXT   SEGMENT BYTE PUBLIC 'CODE'
        ASSUME DS:_TEXT, SS:_TEXT ,CS:_TEXT ,ES:_TEXT
        ORG     $+0100H

start:  jmp     begin

string   DB 'Hello$'

begin:
        OutStr string
        InKey
        Exit 0

        _TEXT   ENDS

END     start
