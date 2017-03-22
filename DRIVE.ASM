TITLE	DRIVE	3-23-90	[8-18-90]

.RADIX  16

S0000	SEGMENT
	ASSUME DS:S0000, SS:S0000 ,CS:S0000 ,ES:S0000
	ORG	$+0100H

L0100:	MOV	AH,9				;0100 B4 09
        MOV     DX,OFFSET Msg                   ;0102 BA 14 01
	INT	21				;0105 CD 21
	MOV	AH,19				;0107 B4 19
	INT	21				;0109 CD 21
	ADD	AL,41	;'A'			;010B 04 41
	MOV	DL,AL				;010D 88 C2
	MOV	AH,2				;010F B4 02
	INT	21				;0111 CD 21
        RET                                     ;0113 C3
Msg     DB      'Current drive is $'

        S0000   ENDS
END	L0100
