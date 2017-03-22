.RADIX  16

S0000   SEGMENT
        ASSUME CS:S0000
        ORG     100

L0100:
        MOV     AH,9
        MOV     DX,OFFSET Msg
        INT     21
        RET
Msg     DB      'Computer Lab$'

        S0000   ENDS
END     L0100
