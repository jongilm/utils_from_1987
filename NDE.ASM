	PAGE	66,128
	PAGE
;
;	*****************************************************************
;	*								*
;	*		      The Norton DOS Editor			*
;	*	          A Command line editor for DOS			*
;	*								*
;	*			By Stanley Reifel			*
;	*								*
;	*     Copyright (C) 1986-88  Peter Norton and S. Reifel & Co.	*
;	*								*
;	*		01/01/88		   Rev 1.3C		*
;	*								*
;	*****************************************************************
;
;
;
MaxSWdt	EQU	134			;max screen width
MaxLin	EQU	140			;max line length to edit
CmdLen	EQU	1000			;number of char for previous commands
ShrtStr	EQU	2			;length of strings too short to save
;
BS	EQU	08H			;ASCII backspace
TAB	EQU	09H			;ASCII tab
LF	EQU	0AH			;ASCII linefeed
CR	EQU	0DH			;ASCII return
ESCAPE     EQU     1BH                     ;ASCII escape
;
HomeKey	EQU	0147H			;home key
UpArrw	EQU	0148H			;up arrow key
LtArrw	EQU	014BH			;left arrow key
RtArrw	EQU	014DH			;right arrow key
EndKey	EQU	014FH			;end key
DnArrw	EQU	0150H			;down arrow key
DelKey	EQU	0153H			;delete key
CntLf	EQU	0173H			;control left arrow key
CntRt	EQU	0174H			;control right arrow key
AltW	EQU	0111H			;ALT W key
AltL	EQU	0126H			;ALT L key
AltK	EQU	0125H			;ALT K key
AltU	EQU	0116H			;ALT U key
F3Key	EQU	013DH			;F3 function key
;
CNTL	EQU	40H			;difference between alpha and control
;
;------------------------------------------------------------------------------
;
CODSEG	SEGMENT	PUBLIC 'CODE'		;info for the assembler
	ASSUME	CS:CODSEG
	ASSUME	DS:CODSEG
	ASSUME	SS:CODSEG
	ASSUME	ES:CODSEG
;
	ORG	100H
;
;----------------------------------------------------------------------------
;
Start:	MOV	SP,OFFSET StckTop	;set stack pointer to top of stack
	MOV	CS:CodeSeg,CS		;save this code segment
	MOV	DS,CS:CodeSeg		;DS is same as code segment
;
	CALL	InitTny			;initialize the editor
	CALL	Install			;install the command editor
;
	MOV	SI,OFFSET Prompt	;display the installed message
	CALL	PutS
;
	MOV	AH,31H			;terminate and remain resident
	MOV	AL,0			;set ERRORLEVEL to 0
	MOV	DX,OFFSET ProgEnd	;DX = length of program
	MOV	CL,4			;divide by 16
	SHR	DX,CL			;DX = memory size value in paragraphs
	ADD	DX,2			;add a bit more
	INT	21H			;back to DOS
;
;
Prompt	DB	'Norton DOS Editor, Version 1.3C',CR,LF,0
;
;
;
;call to install the command line editor into the operating system
Install:CLI				;disable interrupts
	MOV	AH,35H			;get vector for DOS functions calls
	MOV	AL,21H			;read INT 21H vector
	INT	21H
	MOV	WORD PTR CS:DOSVect,BX	;save the vector to DOS functions
	MOV	WORD PTR CS:DOSVect+2,ES
;
	CLI				;make sure they are disabled
	MOV	DX,OFFSET DOSTrap	;DS:DX -> new DOS vector
	MOV	AH,25H			;set vector for DOS functions calls
	MOV	AL,21H			;write INT 21H vector
	INT	21H
;
	STI				;turn interrupts back on
	RET
;
;
;
;interrupt vector handler for DOS calls through INT 21H
DOSTrap	PROC	FAR
	CMP	AH,0AH			;check for call to buffered keyboard
	JNE	DOSTrp1			;branch if not this vector
;
	CLI				;turn interrupts off
	MOV	WORD PTR CS:OldSP,SP	;save the old stack pointer
	MOV	WORD PTR CS:OldSP+2,SS
;
	MOV	SS,CS:CodeSeg		;make a local stack
	MOV	SP,OFFSET StckTop	;set stack pointer to top of stack
	STI				;turn interrupts back on
;
	PUSH	AX			;save registers
	CALL	BufKey			;do buffer keyboard input
	POP	AX			;restore registers
;
	CLI				;turn interrupts off
	MOV	SS,WORD PTR CS:OldSP+2	;restore the old stack
	MOV	SP,WORD PTR CS:OldSP
	STI				;turn interrupts back on
	IRET
;
DOSTrp1:JMP	DWORD PTR CS:DOSVect	;pass call to DOS
;
DOSTrap	ENDP
;
;----------------------------------------------------------------------------
;  Function to emulate the DOS buffered keyboard input routine INT 21 AH=0AH
;----------------------------------------------------------------------------
;
;call to get buffered input from the keyboard using the tiny editor
;	Enter:	DS:DX   -> input buffer for the string
;		DS:DX+0 = number of characters the buffer can hold
;	Exit:	DS:DX+1 = number of characters in buffer excluding the CR
;		DS:DX+2 -> string terminated by CR
;		all other registers unchanged
BufKey:	PUSH	AX			;save the world
	PUSH	BX
	PUSH	CX
	PUSH	DX
	PUSH	BP
	PUSH	SI
	PUSH	DI
	PUSH	DS
	PUSH	ES
;
	MOV	AX,DS			;ES:DI -> string storage
	MOV	ES,AX
	MOV	DI,DX
;
	MOV	CL,[DI]			;CL = max number of char to accept
	MOV	CH,0			;CX = max number of char to accept
	DEC	CX			;sub 1 to make space for the CR
	CALL	TinyEdt			;get the string
;
	MOV	AL,CR			;echo the RETURN
	CALL	PutC
;
	CALL	StrLen			;compute the string length	
	INC	DI			;ES:DI -> string length storage
	MOV	ES:[DI],CL		;save the string length
	INC	DI			;ES:DI -> storage for the string
;
	CALL	CpyStr			;copy the string into buffer
	MOV	BYTE PTR ES:[DI],CR	;terminate the string with CR
;
	POP	ES			;restore all registers and return
	POP	DS
	POP	DI
	POP	SI
	POP	BP
	POP	DX
	POP	CX
	POP	BX
	POP	AX
	RET
;
;
;
;call to move output string
;	Enter:	CS:SI -> input string terminated by 0
;		ES:DI -> destination for string
;	Exit:	CS:SI -> 0 in source string
;		ES:DI -> 0 in destination string
;		AX = undefined
;		all other registers unchanged
CpyStr:	MOV	AL,CS:[SI]		;get char from source string
	MOV	ES:[DI],AL		;write the destination string
	CMP	AL,0			;is this the termiation char?
	JZ	CpyStr1			;yes, string moved
	INC	SI			;inc the from pointer
	INC	DI			;inc the two pointer
	JMP	CpyStr			;loop till done
CpyStr1:RET
;
;------------------------------------------------------------------------------
;			      Little line editor
;------------------------------------------------------------------------------
;
;call to get a single line of text from the keyboard with editing
;	Enter:	CX = max number of characters to accept exclucing termination 0
;	Exit:	CS:SI -> string terminated with 0, CR not included
;		all other registers unchanged
TinyEdt:PUSH	AX			;save registers
	PUSH	BX
	PUSH	CX
	PUSH	DX
	PUSH	BP
	PUSH	DI
	PUSH	DS
	PUSH	ES
;
	MOV	AX,CS			;data segment = code segment
	MOV	DS,AX
	MOV	MaxChar,CX		;save the max number of characters
;
	CALL	InitLin			;begin editing a new line
;
TinyEd1:CALL	GetC			;get a character
	CMP	AX,CR			;is the key a RETURN?
	JE	TinyEd2			;yes, done editing the line
;
	MOV	BX,OFFSET CmndTbl	;BX -> table of keys and commands
	CALL	GoToCmd			;lookup the command and execute
	CALL	Update			;update the display
	JMP	TinyEd1
;
TinyEd2:CALL	BegLine			;move the cursor to the beginning
	CALL	Update			;redraw the display
;
	CALL	EndLine			;remove the bubble in text
	MOV	SI,CursorB		;SI -> beginning of the cursor
	MOV	BYTE PTR [SI],0		;terminate the string
	MOV	SI,OFFSET BublBeg	;CS:SI -> beginning of the string

	CALL	StrLen			;compute the string length	
	CMP	CX,ShrtStr		;is the string too short to save?
	JBE	TinyEd3			;yes, don't save it
	CALL	CmdPut			;no, long enough, record the line
;
TinyEd3:POP	ES
	POP	DS
	POP	DI
	POP	BP
	POP	DX
	POP	CX
	POP	BX
	POP	AX
	RET
;
;
;
;this routine branches to a command based on table lookup
;	Enter:	AX = command number to lookup in table
;		BX -> beginning of lookup table
;	Exit:	AX = command number
;		PC = command subroutine address
;	Table format:	
;		match word, branch word
;		last match word must be 0, the addr is the no match routine
GoToCmd:CMP	AX,0			;is it a null command
	JZ	GoToCd3			;yes, skip it
;
GoToCd1:CMP	[BX],AX			;does table entry match command #
	JE	GoToCd2			;yes, go do that command
;
	CMP	BYTE PTR [BX],0		;is this the end of the table
	JE	GoToCd2			;yes, execute the no match command
;
	ADD	BX,4			;BX -> next table entry
	JMP	GoToCd1			;go test next table entry
;
GoToCd2:ADD	BX,2			;BX -> branch word
	JMP	WORD PTR [BX]		;execute the routine
;
GoToCd3:RET
;
;
;
;call to begin editing a new line
InitLin:CALL	VidStat			;get screen width and display page
	MOV	AL,AH			;AL = screen width
	MOV	AH,0			;AX = screen width
	MOV	ScrnWdt,AX		;save screen width
	MOV	DispPag,BH		;save the display page
;
	CALL	GetCPos			;get the cursor position
	MOV	CursorY,DH		;save cursor position in Y
	MOV	AL,DL			;AL = cursor position in X
	MOV	AH,0			;AX = cursor position in X
	MOV	CursorX,AX		;save cursor position in X
	MOV	LeftX,AX		;save X position of the left most spot
;
	MOV	AX,ScrnWdt		;AX = width of the screen
	SUB	AX,LeftX		;AX = length of editor line
	MOV	LineLen,AX		;save editor line length
;
	CALL	DelLine			;delete everything in the buffer
;
	MOV	SI,OFFSET ScrnImg	;fill image buffer with zeros
	MOV	CX,MaxSWdt		;CX = buffer size
InitLn1:MOV	BYTE PTR [SI],0		;put in a zero
	INC	SI			;SI -> next spot in the buffer
	LOOP	InitLn1			;loop till done
	RET
;
;
;
;call to initialize the tiny editor
InitTny:CALL	VidStat			;get the display page
	MOV	DispPag,BH		;save the display page
;
	MOV	CursorB,OFFSET BublBeg	;set pointer to cursor beginning
	MOV	CursorE,OFFSET BublEnd	;set pointer to cursor end
;
	CALL	NoUnDel			;mark nothing to undelete
	CALL	CmdInit			;initialize the old command recorder
	RET
;
;
;------------------------------------------------------------------------------
;			        Editor commands
;------------------------------------------------------------------------------
;
;call to move the cursor up one line, show previous command in buffer
CursUp:	CALL	DelLine			;delete whats in the buffer now
	CALL	CmdPrev			;SI -> previous command
;
CursUp1:CALL	CmdOtRg			;get a character from the last command
	CMP	AL,0			;is it the termination char?
	JZ	CursUp2			;yes
	CALL	PutIn			;no, place the char in the bubble
	JMP	CursUp1
;
CursUp2:CALL	NoUnDel			;mark nothing undeletable
	CALL	SetCol			;update start display column number
	RET
;
;
;
;call to move the cursor down one line, show next command in buffer
CursDn:	CALL	DelLine			;delete whats in the buffer now
	CALL	CmdNext			;SI -> next command
;
CursDn1:CALL	CmdOtRg			;get a character from the next command
	CMP	AL,0			;is it the termination char?
	JZ	CursDn2			;yes
	CALL	PutIn			;no, place the char in the bubble
	JMP	CursDn1
;
CursDn2:CALL	NoUnDel			;mark nothing undeletable
	CALL	SetCol			;update start display column number
	RET
;
;
;
;call to move the cursor to the beginning of the line
BegLine:CMP	CursorB,OFFSET BublBeg	;is the cursor at the beginning?
	JE	BegLin1			;yes, all done
	CALL	CursLf			;move the cursor over 1
	JMP	BegLine
BegLin1:RET
;
;
;
;call to move the cursor to the end of the line
EndLine:CMP	CursorE,OFFSET BublEnd	;is the cursor at the end?
	JE	EndLin1			;yes, all done
	CALL	CursRt			;move the cursor over 1
	JMP	EndLine
EndLin1:RET
;
;
;
;call to move the cursor left one word
WordL:	CALL	CursLf			;move cursor 1 to the left
	MOV	SI,CursorB		;SI -> character on the left of cursor
	CMP	SI,OFFSET BublBeg	;is it at the beginning of the bubble?
	JE	WordL1			;yes, all done moving
	MOV	AL,[SI-1]		;AL = character on the left
	CALL	WordChr			;is it a legal word character?
	JNC	WordL			;yes, go eat the char
	CALL	CursLf			;move one more across the white char
WordL1:	RET
;
;
;
;call to move the cursor left one word
WordR:	CALL	CursRt			;move one character to the right
	MOV	SI,CursorE		;SI -> character on the right of cursor
	CMP	SI,OFFSET BublEnd	;is it at the end of the bubble?
	JE	WordR1			;yes, all done moving
	MOV	AL,[SI+1]		;AL = character on the right
	CALL	WordChr			;is it a legal word character?
	JNC	WordR			;yes, go eat the char
	CALL	CursRt			;move one more across the white char
WordR1:	RET
;
;
;
;call to move the cursor left one char
CursLf:	CMP	CursorB,OFFSET BublBeg	;is the cursor at the beginning?
	JE	CursLf1			;yes, don't move it
;
	DEC	CursorB			;CursorB -> character left of cursor
	MOV	SI,CursorB		;SI -> cursor beginning
	MOV	DI,CursorE		;DI -> cursor end
	MOV	AL,[SI]			;AL = character left of the cursor
	MOV	[DI],AL			;move character right of the cursor
	DEC	CursorE			;CursorE -> end of the new cursor
	CALL	NoUnDel			;mark nothing undeletable
	CALL	SetCol			;update start display column number
CursLf1:RET
;
;
;
;call to move the cursor right one char
CursRt:	CMP	CursorE,OFFSET BublEnd	;is the cursor at the end?
	JE	CursRt1			;yes, don't move it
;
	INC	CursorE			;CursorE -> character right of cursor
	MOV	SI,CursorB		;SI -> cursor beginning
	MOV	DI,CursorE		;DI -> cursor end
	MOV	AL,[DI]			;AL = character right of the cursor
	MOV	[SI],AL			;move character left of the cursor
	INC	CursorB			;CursorB -> new beginning of the cursor
	CALL	NoUnDel			;mark nothing undeletable
	CALL	SetCol			;update start display column number
CursRt1:RET
;
;
;
;call to delete the line the cursor is on
DelLine:CALL	DelLinL			;delete everything to the left
	CALL	DelLinR			;delete everything to the right
	RET
;
;
;
;call to delete all char to beginning of line
DelLinL:CMP	CursorB,OFFSET BublBeg	;is the cursor at the beginning?
	JE	DelLnL1			;yes, all deleted
	CALL	DelChrL			;no, delete char on the left
	JMP	DelLinL			;loop till all gone
DelLnL1:RET
;
;
;
;call to delete all char to the end of line
DelLinR:CMP	CursorE,OFFSET BublEnd	;is the cursor at the end?
	JE	DelLnR1			;yes, all deleted
	CALL	DelChrR			;no, delete char on the right
	JMP	DelLinR			;loop till all gone
DelLnR1:RET
;
;
;
;call to delete a word left of the cursor
DelWrdL:CALL	DelChrL			;delete the character to the left
	MOV	SI,CursorB		;SI -> character on the left of cursor
	CMP	SI,OFFSET BublBeg	;is it at the beginning of the bubble?
	JE	DelWrL1			;yes, all done deleting
	MOV	AL,[SI-1]		;AL = character on the left
	CALL	WordChr			;is it a legal word character?
	JNC	DelWrdL			;yes, go eat the char
DelWrL1:RET
;
;
;
;call to delete a word right of the cursor
DelWrdR:CALL	DelChrR			;delete the character to the right
	MOV	SI,CursorE		;SI -> character on the right of cursor
	CMP	SI,OFFSET BublEnd	;is it at the end of the bubble?
	JE	DelWrR1			;yes, all done deleting
	MOV	AL,[SI+1]		;AL = character on the right
	CALL	WordChr			;is it a legal word character?
	JNC	DelWrdR			;yes, go eat the char
DelWrR1:RET
;
;
;
;call to delete the character left of the cursor
DelChrL:CMP	CursorB,OFFSET BublBeg	;is the cursor at the beginning?
	JE	DelChL1			;yes, don't delete
;
	DEC	CursorB			;delete the character
	INC	UnLeft			;make the character undeletable
	CALL	SetCol			;update start display column number
DelChL1:RET
;
;
;
;call to delete the character right of the cursor
DelChrR:CMP	CursorE,OFFSET BublEnd	;is the cursor at the end?
	JE	DelChR1			;yes, don't delete
;
	INC	CursorE			;delete the character
	INC	UnRight			;make the character undeletable
	CALL	SetCol			;update start display column number
DelChR1:RET
;
;
;
;call to undelete text from the buffer
Undelet:MOV	AX,UnLeft		;AL = # of char to undelete on left
	ADD	CursorB,AX		;undelete on the left
;
	MOV	AX,UnRight		;AL = # of char to undelete on right
	SUB	CursorE,AX		;undelete on the right
;
	CALL	NoUnDel			;mark nothing undeletable
	CALL	SetCol			;update start display column number
	RET
;
;
;
;call to place a character in the edit buffer, filter out trash first
NoMatch:CALL	Filter			;filter out trash
	JC	NoMtch1			;branch on trash
	CALL	PutIn			;else insert character in edit buffer
	CALL	NoUnDel			;mark nothing undeletable
	CALL	SetCol			;update start display column number
NoMtch1:RET
;
;------------------------------------------------------------------------------
;			  Editor display functions
;------------------------------------------------------------------------------
;
;
;call to update the display line
Update:	PUSH	AX			;save registers
	PUSH	BX
	PUSH	DX
	PUSH	SI
	PUSH	DI
;
	MOV	SI,OFFSET BublBeg	;SI -> first char in bubble
	MOV	BX,StartX		;BX = start column # of display line
	CALL	FindCol			;SI -> first character to display
;
	MOV	DI,OFFSET ScrnImg	;DI -> screen image buffer
	MOV	UpdateX,0		;X pos on display line to update
;
Update1:CMP	SI,CursorB		;does SI -> the cursor in the bubble?
	JNE	Update2			;no
;
	MOV	AX,UpdateX		;yes, save the editor cursor position
	ADD	AX,LeftX		;add the starting left column number
	MOV	EdCursX,AX
	MOV	SI,CursorE		;SI -> end of cursor marker
	INC	SI			;SI -> first char right of the cursor
;
Update2:CMP	SI,OFFSET BublEnd+1	;does SI -> end of the buffer
	JE	Update5			;yes, go blank end of line
;
	MOV	AL,[SI]			;AL = next char to display
	INC	SI			;SI -> next char in the bubble
	CMP	AL,TAB			;is the character a TAB?
	JNE	Update4			;no, go display the character
;
Update3:MOV	AL,' '			;yes, expand the tab, display a blank
	CALL	UpdateC			;display the blank
	JZ	Update6			;branch if update complete
	MOV	AX,UpdateX		;AX = current update column
	ADD	AX,StartX		;AX = X pos from start of edit line
	AND	AX,111B			;AX = column MOD 8
	JNZ	Update3			;loop till tab expanded
	JMP	Update1			;go display the next character
;
Update4:CALL	UpdateC			;display a normal character
	JZ	Update6			;branch if update complete
	JMP	Update1			;go display the next character
;
Update5:MOV	AX,LineLen		;blank end of line
	DEC	AX			;AX = right most column number
	CMP	AX,UpdateX		;has the full screen been displayed?
	JZ	Update6			;yes, all done
;
	CMP	BYTE PTR [DI],0		;has this character never been changed?
	JZ	Update6			;yes, don't change it
;
	MOV	AL,' '			;end of line, display blanks
	CALL	UpdateC			;display the character
	JNZ	Update1			;go display next char if not done
;
Update6:MOV	DX,EdCursX		;position the hardware editor cursor
	CALL	SetCurX
	MOV	CursorX,DX		;save the cursor X value
;
	POP	DI			;restore registers and return
	POP	SI
	POP	DX
	POP	BX
	POP	AX
	RET
;
;
;
;call to update a single character on the display
;	Enter:	AL = character to write
;		DI -> character in same position on the screen
;		CursorX = current position of the hardware cursor in X
;		UpdateX = next X location to update on the screen
;	Exit:	CursorX = location of the hardware cursor in X
;		UpdateX = new X location to update on the screen next time
;		DI -> next character in screen image
;		Z flag set if line fully displayed
UpdateC:CMP	AL,[DI]			;is the char already shown?
	JE	UpdatC2			;yes, don't change it
;
	MOV	DX,UpdateX		;no, must display the character
	ADD	DX,LeftX		;DL = desired X location to update
	CMP	CursorX,DX		;is the cursor in the correct spot?
	JE	UpDatC1			;yes, don't need to move it
	CALL	SetCurX			;no, set the cursor column	
	MOV	CursorX,DX		;save the cursor X value
;
UpdatC1:CALL	PutC			;display the character
	INC	CursorX			;increment the hardware cursor column
	MOV	[DI],AL			;put the character in the image buffer
;
UpdatC2:INC	UpdateX			;increment the next update column #
	INC	DI			;DI -> next char in image buffer
	MOV	AX,LineLen		;AX = line length
	DEC	AX			;AX = left most column, line relative
	CMP	AX,UpdateX		;set Z flag if display line is full
	RET
;
;
;------------------------------------------------------------------------------
;			Primitive editor functions
;------------------------------------------------------------------------------
;
;call to filter out unwanted characters
;	Enter:	AX = keyboard character
;	Exit:	AX = keyboard character
;		Carry set if key filter out
;		all other registers unchanged
Filter:	CMP	AX,07FH			;is it higher than ASCII char
	JAE	Filter2			;yes, filter it out
;
	CMP	AL,TAB			;is it a tab?
	JE	Filter1			;yes, don't filter
;
	CMP	AL,' '			;is it a control char?
	JB	Filter2			;yes, filter it out
;
Filter1:CLC				;don't filter
	RET				;return with no carry
;
Filter2:STC				;do filter the character
	RET				;return with no carry
;
;
;
;call to insert a character into the bubble buffer, no filtering
;	Enter:	AL = character
;	Exit:	all registers unchanged
PutIn:	PUSH	AX			;save registers
	PUSH	SI
;
	CALL	BublCnt			;BX = number of characters in bubble
	CMP	BX,MaxChar		;are more characters allowed?
	JAE	PutIn1			;no, trash the character
;
	MOV	SI,CursorB		;SI -> beginning of the cursor
	MOV	[SI],AL			;put the character in the bubble
	INC	SI			;SI -> next spot in the bubble
	CMP	SI,CursorE		;is the bubble full?
	JE	PutIn1			;yes, don't update the cursor position
	MOV	CursorB,SI		;update the beginning of the cursor
;
PutIn1:	POP	SI			;restore registers and return
	POP	AX
	RET
;
;
;
;call to compute the number of characters in the bubble buffer
;	Exit:	BX = number of characters in the buffer
;		all other registers unchanged
BublCnt:MOV	BX,OFFSET BublEnd	;add char count before & after bubble
	SUB	BX,CursorE
	ADD	BX,CursorB
	SUB	BX,OFFSET BublBeg
	RET
;
;
;
;call to mark nothing undeletable
NoUnDel:MOV	UnLeft,0
	MOV	UnRight,0
	RET
;
;
;
;find address of char at column N
;	Enter:	SI -> beginning of string
;		BX = desired column number
;	Exit:	SI -> character at column N
;		all other registers unchanged
FindCol:PUSH	AX			;save registers
	PUSH	BX
	PUSH	CX
;
	MOV	CX,0			;CX = column counter
	CMP	CX,BX			;is this the correct column number?
	JE	FindCl3			;yes, all done
	INC	BX			;incrememt the desired column number
;
FindCl1:MOV	AL,[SI]			;AL = character from string
	CMP	AL,TAB			;is it a tab?
	JNE	FindCl2			;no, normal character
;
	ADD	CX,8			;yes, expand tab, add tab width
	AND	CX,0FFF8H		;CX = column at next tab stop
	CMP	CX,BX			;is this the correct column?
	JAE	FindCl3			;yes, stop look'n
	INC	SI			;no, SI -> next char in string
	JMP	FindCl1			;branch if this not desired column
;
FindCl2:INC	CX			;no, add 1 column for displayable char
	CMP	CX,BX			;is this the correct column?
	JE	FindCl3			;yes, stop look'n
	INC	SI			;SI -> next char in string
	JMP	FindCl1			;keep looking
;
FindCl3:POP	CX			;restore registers and return
	POP	BX
	POP	AX
	RET
;
;
;
;call to compute the column number to begin displaying
;	Enter:	StartX = current starting column of display line
;	Exit:	StartX = new starting column of the display line
;		all registers unchanged
SetCol:	PUSH	AX			;save registers
	PUSH	BX
;
	CALL	ColCurs			;compute cursor column number
	CMP	BX,StartX		;is the cursor left of the screen?
	JAE	SetCol1			;no
	MOV	StartX,BX		;yes, set the start spot same as cursor
	JMP	SetCol2			;all done
;
SetCol1:MOV	AX,LineLen		;AX = width of the display line
	DEC	AX			;AX = right most display column
	ADD	AX,StartX		;AX = X pos of last displayed character
	CMP	BX,AX
	JB	SetCol2			;branch if cursor is on the screen
;
	SUB	BX,LineLen		;cursor right of display line
	ADD	BX,2			;compute new starting column
	MOV	StartX,BX
;
SetCol2:POP	BX			;restore registers and return
	POP	AX
	RET
;
;
;
;call to compute the column number the cursor is on, from beginning of line
;	Exit:	BX = cursor column number
;		all other register unchanged
ColCurs:PUSH	AX			;save registers 
	PUSH	SI
	MOV	SI,OFFSET BublBeg	;SI -> first char on line
	MOV	BX,0			;zero column counter
;
ColCur1:CMP	SI,CursorB		;does SI -> the cursor?
	JE	ColCur3			;yes, found the cursor column
;
	MOV	AL,[SI]			;get char from line
	INC	SI			;SI -> next char on line
	CMP	AL,TAB			;is it a tab?
	JNE	ColCur2			;no, normal character
;
	ADD	BX,8			;add distance between tab stops
	AND	BX,0FFF8H		;BX = column of next tab stop
	JMP	ColCur1			;go check next character
;
ColCur2:INC	BX			;add 1 column for displayable char
	JMP	ColCur1			;go check next char
;
ColCur3:POP	SI			;restore registers and return
	POP	AX
	RET
;
;
;------------------------------------------------------------------------------
;		Functions to record and retrieve past lines edited 
;------------------------------------------------------------------------------
;
;call to find the previous command
;	Exit:	SI -> beginning of the previous command
;		all other registers unchanged
CmdPrev:MOV	SI,CmdPnt		;SI -> current command
	CMP	SI,CmdOld		;is this the oldest command?
	JE	CmdPrv1			;yes, return pointer to this one
;
	CALL	CmdBefr			;SI -> the previous command
	MOV	CmdPnt,SI		;SI -> new current command
CmdPrv1:RET
;
;
;
;call to find the next command
;	Exit:	SI -> beginning of the next command
;		all other registers unchanged
CmdNext:MOV	SI,CmdPnt		;SI -> current command
	CMP	SI,CmdNew		;is this the newest command?
	JE	CmdNxt1			;yes, return pointer to this one
;
	CALL	CmdAftr			;SI -> the next command
	MOV	CmdPnt,SI		;SI -> new current command
CmdNxt1:RET
;
;
;
;call to enter a command into the ring
;	Enter:	SI -> command terminated by zero to insert into the ring
CmdPut:	PUSH	AX			;save registers
	PUSH	SI
;
	CMP	BYTE PTR [SI],0		;is it a null command?
	JZ	CmdPut2			;yes, don't put it in
;
CmdPut1:MOV	AL,[SI]			;get a character to insert
	INC	SI			;SI -> next char to insert
	CALL	CmdInRg			;put it in the ring
	CMP	AL,0			;is it the termination zero?
	JNZ	CmdPut1			;no, go insert the next one
;
CmdPut2:MOV	SI,CmdNew		;SI -> spot for the next new command
	MOV	CmdPnt,SI		;put the command pointer here
	MOV	BYTE PTR [SI],0		;make new command a null string
;
	POP	SI			;restore registers and return
	POP	AX
	RET
;
;
;
;call to get a character from the ring
;	Enter:	SI -> character in the ring
;	Exit:	AL = character from the ring
;		SI -> next character in the ring
CmdOtRg:MOV	AL,[SI]			;get char from the ring
	CALL	CmdInc			;SI -> next char in the ring
	RET
;
;
;
;call to enter a character into the ring
;	Enter:	AL = character to insert
;		CmdNew -> storage for new command character
;	Exit:	all registers unchanged
CmdInRg:PUSH	SI			;save registers
	MOV	SI,CmdNew		;SI -> storage for new commands
	MOV	[SI],AL			;save the new command character
	CALL	CmdInc			;increment the pointer
	MOV	CmdNew,SI		;update pointer for new commands
;
	CMP	SI,CmdOld		;is an old command in the way?
	JNE	CmdInR1			;no
	MOV	SI,CmdOld		;yes, remove the oldest command
	CALL	CmdAftr			;SI -> next to oldest command
	MOV	CmdOld,SI		;save pointer to new oldest command
;
CmdInR1:POP	SI
	RET
;
;
;
;call to search for the previous command
;	Enter:	SI -> beginning of current command
;	Exit:	SI -> beginning of previous command
;		all other registers unchanged
CmdBefr:CALL	CmdDec			;SI -> termination of previous command
CmdBfr1:CALL	CmdDec			;SI -> previous character
	CMP	BYTE PTR [SI],0		;does SI -> termination char?
	JNZ	CmdBfr1			;no, keep looping
	CALL	CmdInc			;SI -> first char of command
	RET
;
;
;
;call to search for the next command
;	Enter:	SI -> beginning of current command
;	Exit:	SI -> beginning of next command
;		all other registers unchanged
CmdAftr:CALL	CmdInc			;SI -> next character
	CMP	BYTE PTR [SI],0		;does SI -> termination char?
	JNZ	CmdAftr			;no, keep looping
	CALL	CmdInc			;SI -> first char of command
	RET
;
;
;
;call to increment a pointer into the commands buffer
;	Exit:	SI = increment pointer
;		all other registers unchanged
CmdInc:	INC	SI			;inc the pointer
	CMP	SI,OFFSET CmdBot	;is the pointer outside the ring?
	JBE	CmdInc1			;no
	MOV	SI,OFFSET CmdTop	;yes, set pointer to the top
CmdInc1:RET
;
;
;
;call to decrement a pointer into the commands buffer
;	Exit:	SI = decremented pointer
;		all other registers unchanged
CmdDec:	DEC	SI			;dec the pointer
	CMP	SI,OFFSET CmdTop	;is the pointer outside the ring?
	JAE	CmdDec1			;no
	MOV	SI,OFFSET CmdBot	;yes, set pointer to the bottom
CmdDec1:RET
;
;
;
;call to initialize the command recording functions
CmdInit:MOV	SI,OFFSET CmdTop	;SI -> beginning of command ring
	MOV	CmdOld,SI		;initialize all pointer to top of ring
	MOV	CmdNew,SI
	MOV	CmdPnt,SI
;
	MOV	SI,OFFSET CmdTop	;fill command buffer with null strings
	MOV	CX,CmdLen		;CX = buffer size
CmdInt1:MOV	BYTE PTR [SI],0		;put in a zero
	INC	SI			;SI -> next spot in the buffer
	LOOP	CmdInt1			;loop till done
	RET
;
;------------------------------------------------------------------------------
;			DOS and ROM interface functions
;------------------------------------------------------------------------------
;
;call to get the state of the video
;	Exit:	AL = display mode
;		AH = number of characters columns on screen
;		BH = current active display page
;		BL = undefined
;		all other registers unchanged
VidStat:PUSH	SI			;save registers
	PUSH	DI

	MOV	AH,15			;get the video state
	INT	10H
;
	POP	DI			;restore registers and return
	POP	SI
	RET
;
;
;
;call to get the cursor position
;	Exit:	DL = cursor position in X
;		DH = cursor position in Y
;		all other registers unchanged
GetCPos:PUSH	AX			;save registers
	PUSH	BX
	PUSH	SI
	PUSH	DI
;
	MOV	BH,DispPag		;BH = display page
	MOV	AH,3			;read the cursor position
	INT	10H
;
	POP	DI			;restore registers and return
	POP	SI
	POP	BX
	POP	AX
	RET
;
;
;
;call to set the cursor X position
;	Enter:	DL = cursor position in X
;	Exit:	all registers unchanged
SetCurX:PUSH	AX			;save registers
	PUSH	BX
	PUSH	SI
	PUSH	DI
;
	MOV	DH,CursorY		;DH = cursor line number
	MOV	BH,DispPag		;BH = display page
	MOV	AH,2			;set the cursor position
	INT	10H
;
	POP	DI			;restore registers and return
	POP	SI
	POP	BX
	POP	AX
	RET
;
;
;
;call to display a string
;	Enter:	SI -> string terminated by zero
;	Exit:	all registers unchanged
PutS:	PUSH	AX			;save registers
	PUSH	SI
;
PutS1:	MOV	AL,[SI]			;get char from the string
	INC	SI			;SI -> next char in string
	CMP	AL,0			;is it the termination zero?
	JZ	PutS2			;yes, all done
	CALL	PutC			;display the character
	JMP	PutS1			;go display the next character
;
PutS2:	POP	SI			;restore registers and return
	POP	AX
	RET
;
;
;
;call to put a single character at the cursor postion and advance the cursor
;	Enter:	AL = character to display
;	Exit:	all registers unchanged
PutC:	PUSH	AX			;save registers
	PUSH	BX
	PUSH	SI
	PUSH	DI
;
	MOV	AH,14			;go draw the character
	MOV	BL,0			;BL = forground color for graphics mode
	MOV	BH,DispPag		;get the display page
	INT	10H
;
	POP	DI			;restore registers and return
	POP	SI
	POP	BX
	POP	AX
	RET
;
;
;
;call to get a char from the console keyboard without echo
;	Exit:	AX = char from keyboard, AH = 1 if exteneded keycode else 0
;		all other registers unchanged
GetC:	MOV	AH,8			;read keyboard without echo
	INT	21H
	CMP	AL,0			;is it an extended key?
	JZ	GetC1			;yes, go get the keycode
	MOV	AH,0			;no, AX = keyboard code
	RET
;
GetC1:	MOV	AH,8			;read keyboard without echo
	INT	21H
	MOV	AH,1			;AX = extended keycode
	RET
;
;
;
;jump here to return to DOS
GoToDOS:MOV	AH,4CH			;tell DOS to terminate process
	MOV	AL,0			;set ERRORLEVEL to 0
	INT	21H
;
;------------------------------------------------------------------------------
;				Misc functions
;------------------------------------------------------------------------------
;
;call to compute the length of a sting
;	Enter:	CS:SI -> string terminated by zero
;	Exit:	CX = string length
;		all other registers unchanges
StrLen:	PUSH	SI			;save registers
	MOV	CX,0			;zero the counter
;
StrLen1:CMP	BYTE PTR CS:[SI],0	;is this the termination character?
	JZ	StrLen2			;yes, all done
	INC	SI			;SI -> next char in string
	INC	CX			;inc the char counter
	JMP	StrLen1			;loop till done
;
StrLen2:POP	SI			;restore registers and return
	RET
;
;
;
;call to check for legal word character
;	Enter:	AL = char to test
;	Exit:	CF set if invalid char
;		all other registers unchanged
WordChr:PUSH	BX			;save BX
	MOV	BX,'az'			;is the char a lower case letter?
	CALL	ChrRang
	JNC	WordCh1			;yes, return with no carry
;
	MOV	BX,'AZ'			;is the char an upper case letter?
	CALL	ChrRang
	JNC	WordCh1			;yes, return with no carry
;
	MOV	BX,'09'			;is the char a number?
	CALL	ChrRang			;return with status in carry
WordCh1:POP	BX			;restore BX and return
	RET
;
;
;
;call to test if a character is between or included by two characters
;	Enter:	AL = char to check
;		BL = upper match char
;		BH = low match char
;	Exit:	CF clear if valid match
;		BL = undefined
;		all other registers unchanged
ChrRang:CMP	AL,BH			;is the char < BH
	JC	ChrRng1			;yes, no match, return with carry
	INC	BL			;is the char > BL
	CMP	AL,BL			;compare with upper limit
	CMC				;invert the carry flag
ChrRng1:RET
;
;------------------------------------------------------------------------------
;
;table of keys and their commands address
CmndTbl:DW	LtArrw,	  CursLf	;move cursor left one column
	DW	RtArrw,	  CursRt	;move cursor right one column
	DW	CntLf,	  WordL		;move cursor left word
	DW	CntRt,	  WordR		;move cursor right word
	DW	HomeKey,  BegLine	;move cursor to beginning of line
	DW	EndKey,	  EndLine	;move cursor to end of line
	DW	UpArrw,	  CursUp	;move cursor up one line
	DW	DnArrw,	  CursDn	;move cursor down one line
	DW	F3Key,	  CursUp	;move cursor up one line
;
	DW	BS,	  DelChrL	;delete char left of cursor
	DW	DelKey,	  DelChrR	;delete char right of cursor
	DW	'W'-CNTL, DelWrdL	;delete word left of cursor
	DW	AltW,	  DelWrdR	;delete word right of cursor
	DW	'L'-CNTL, DelLinL	;delete to beginning of line
	DW	AltL,	  DelLinR	;delete to end of line
	DW	AltK,	  DelLine	;delete all characters on line
        DW      ESCAPE,      DelLine       ;delete all characters on line
;
	DW	'U'-CNTL, Undelet	;undelete text from buffer
	DW	AltU,	  Undelet	;undelete text from buffer
;
	DW	0000H,	  NoMatch	;no match, place the char in buffer
;
;------------------------------------------------------------------------------
;
BublBeg	DB	MaxLin DUP (?)		;beginning of bubble buffer
BublEnd	DB	?			;end of bubble buffer
CursorB	DW	?			;pointer to beginning of cursor
CursorE	DW	?			;pointer to end cursor in text buffer
UnLeft	DW	?			;undelete data counter on the left
UnRight	DW	?			;undelete data counter on the right
;
ScrnWdt	DW	?			;screen width in columns
DispPag	DB	?			;display page
MaxChar	DW	?			;max number of char in line
;
;screen relative variables
LeftX	DW	?			;X position of the left most character
EdCursX	DW	?			;X position of the editor cursor
CursorX	DW	?			;X position of the cursor
CursorY	DB	?			;Y position of the cursor
;
;display line relative variables
LineLen	DW	?			;length of display line
UpdateX	DW	?			;X position for character update
;
;text line relative variables
StartX	DW	?			;column # of start of the display line
;
ScrnImg	DB	MaxSWdt DUP (?)		;image of screen display line
;
DOSVect	DD	?			;contains pointer to DOS vector
OldSP	DD	?			;contains old stack pointer
CodeSeg	DW	?			;code segment for this program
;
;storage for the command recording functions
CmdTop	DB	CmdLen-1 DUP (?)	;ring buffer for line entries
CmdBot	DB	?			;end of ring buffer
CmdOld	DW	?			;pointer to oldest command
CmdNew	DW	?			;pointer to newest command
CmdPnt	DW	?			;pointer to current command
;
StckBot	DB	200 DUP (?)		;storage for the stack
StckTop	DB	?			;top of the stack
;
;
;end of program storage
ProgEnd	DB	?			;end of this program
;
;------------------------------------------------------------------------------
;
CODSEG	ENDS
;
;-------------------------------- End of NDE ----------------------------------
;
	END	START

