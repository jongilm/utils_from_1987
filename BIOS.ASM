include bios.mac

PUBLIC  _OutChar
PUBLIC  _ClrWin
PUBLIC  _Window

EXTRN   VideoSeg:WORD

_TEXT    SEGMENT BYTE PUBLIC 'CODE'
        ASSUME DS:_TEXT, SS:_TEXT ,CS:_TEXT ,ES:_TEXT

;*****************************
;*** OutChar(X,Y,Char,Qty) ***
;*****************************
_OutChar PROC NEAR
	push	bp
	mov	bp,sp
	sub	sp,0
	push	di
        push    si

;       Qty  = 10
;       Char = 8
;       Y    = 6
;       X    = 4
        mov     ax,WORD PTR [bp+6]      ;Y
        mov     bx,80
        mul     bx
        add     ax,WORD PTR [bp+4]      ;X
        shl     ax,1
        mov     di,ax                   ;di=(Y*80+X)*2
        mov     es,WORD PTR VideoSeg    ;es=0B800h
        mov     ax,WORD PTR [bp+8]      ;Char
        mov     ah,al
        xor     al,al                   ;ah=Char
        mov     cx,WORD PTR [bp+10]     ;cx=Qty
OutChr1:mov     es:[di],ah
        add     di,2
        dec     cx
        cmp     cx,0
        ja      OutChr1

	pop	si
	pop	di
	mov	sp,bp
	pop	bp
	ret	
_OutChar ENDP

;**********************************
;*** ClrWin(X1,Y1,X2,Y2,Colour) ***
;**********************************
_ClrWin PROC NEAR
	push	bp
	mov	bp,sp
	sub	sp,0
	push	di
        push    si

;       Colour= 12
;       Y2    = 10
;       X2    = 8
;       Y1    = 6
;       X1    = 4

        mov     ax,WORD PTR [bp+8]     ;X2
        sub     ax,WORD PTR [bp+4]     ;X1
        inc     ax
        xor     al,al                  ;AL=X2-X1+1
        mov     ah,6
        mov     bh,BYTE PTR [bp+12]    ;Colour
        mov     ch,BYTE PTR [bp+6]     ;Y1
        mov     cl,BYTE PTR [bp+4]     ;X1
        mov     dh,BYTE PTR [bp+10]    ;Y2
        mov     dl,BYTE PTR [bp+8]     ;X2
        int     10h

	pop	si
	pop	di
	mov	sp,bp
	pop	bp
	ret	
_ClrWin ENDP


;*****************************************
;*** Window(X1,Y1,X2,Y2,Colour,Shadow) ***
;*****************************************
_Window PROC NEAR
	push	bp
	mov	bp,sp
	sub	sp,0
	push	di
        push    si

;       Shadow= 14
;       Colour= 12
;       Y2    = 10
;       X2    = 8
;       Y1    = 6
;       X1    = 4
IF 1
        mov     ax,WORD PTR [bp+14]     ;Shadow
        push    ax
        mov     ax,WORD PTR [bp+10]     ;Y2
        add     ax,1
        push    ax
        mov     ax,WORD PTR [bp+8]      ;X2
        add     ax,2
        push    ax
        mov     ax,WORD PTR [bp+6]      ;Y1
        add     ax,1
        push    ax
        mov     ax,WORD PTR [bp+4]      ;X1
        add     ax,2
        push    ax
        call    _ClrWin                 ;Display offset box in shadow colour
        add     sp,10
ENDIF
;        ClrWin WORD PTR [bp+4],WORD PTR [bp+6],WORD PTR [bp+8],WORD PTR [bp+10],WORD PTR [bp+14]

        mov     ax,WORD PTR [bp+12]     ;Colour
        push    ax
        mov     ax,WORD PTR [bp+10]     ;Y2
        push    ax
        mov     ax,WORD PTR [bp+8]      ;X2
        push    ax
        mov     ax,WORD PTR [bp+6]      ;Y1
        push    ax
        mov     ax,WORD PTR [bp+4]      ;X1
        push    ax
        call    _ClrWin                 ;Display box in colour
        add     sp,10

        mov     ax,1
        push    ax                      ;Qty=1
        mov     ax,218
        push    ax                      ;Char='Ú'
        mov     ax,WORD PTR [bp+6]
        push    ax                      ;Y=Y1
        mov     ax,WORD PTR [bp+4]
        push    ax                      ;X=X1
        call    _OutChar                 ;Display top left corner
        add     sp,8

        mov     ax,WORD PTR [bp+8]      ;X2
        sub     ax,WORD PTR [bp+4]      ;X1
        dec     ax
        push    ax                      ;Qty=X2-X1-1
        mov     ax,196
        push    ax                      ;Char='Ä'
        mov     ax,WORD PTR [bp+6]      ;Y=Y1
        push    ax
        mov     ax,WORD PTR [bp+4]
        inc     ax
        push    ax                      ;X=X1+1
        call    _OutChar                 ;Display top row
        add     sp,8

        mov     ax,1
        push    ax                      ;Qty=1
        mov     ax,191
        push    ax                      ;Char='¿'
        mov     ax,WORD PTR [bp+6]
        push    ax                      ;Y=Y1
        mov     ax,WORD PTR [bp+8]
        push    ax                      ;X=X2
        call    _OutChar                 ;Display top right corner
        add     sp,8



        mov     ax,WORD PTR [bp+10]     ;Y2
        sub     ax,WORD PTR [bp+6]      ;Y1
        dec     ax
        mov     dx,ax                   ;dx=Y2-Y1-1

        mov     ax,WORD PTR [bp+6]      ;Y1
        inc     ax
        mov     si,ax                   ;si=Y1+1

Window1:
        push    dx

        push    si
        mov     ax,1
        push    ax                      ;Qty=1
        mov     ax,179
        push    ax                      ;Char='³'
        push    si                      ;Y=si
        mov     ax,WORD PTR [bp+4]
        push    ax                      ;X=X1
        call    _OutChar                 ;Display left column
        add     sp,8
        pop     si

        push    si
        mov     ax,1
        push    ax                      ;Qty=1
        mov     ax,179
        push    ax                      ;Char='³'
        push    si                      ;Y=si
        mov     ax,WORD PTR [bp+8]
        push    ax                      ;X=X2
        call    _OutChar                 ;Display right column
        add     sp,8
        pop     si

        pop     dx
        inc     si
        dec     dx
        cmp     dx,0
        ja      Window1

        mov     ax,1
        push    ax                      ;Qty=1
        mov     ax,192
        push    ax                      ;Char='À'
        mov     ax,WORD PTR [bp+10]
        push    ax                      ;Y=Y2
        mov     ax,WORD PTR [bp+4]
        push    ax                      ;X=X1
        call    _OutChar                 ;Display bottom left corner
        add     sp,8

        mov     ax,WORD PTR [bp+8]      ;X2
        sub     ax,WORD PTR [bp+4]      ;X1
        dec     ax
        push    ax                      ;Qty=X2-X1-1
        mov     ax,196
        push    ax                      ;Char='Ä'
        mov     ax,WORD PTR [bp+10]
        push    ax                      ;Y=Y2
        mov     ax,WORD PTR [bp+4]
        inc     ax
        push    ax                      ;X=X1+1
        call    _OutChar                 ;Display bottom row
        add     sp,8

        mov     ax,1
        push    ax                      ;Qty=1
        mov     ax,217
        push    ax                      ;Char='Ù'
        mov     ax,WORD PTR [bp+10]
        push    ax                      ;Y=Y2
        mov     ax,WORD PTR [bp+8]
        push    ax                      ;X=X2
        call    _OutChar                 ;Display bottom right corner
        add     sp,8

	pop	si
	pop	di
	mov	sp,bp
	pop	bp
	ret	
_Window ENDP

        _TEXT   ENDS

END
