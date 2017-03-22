;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³ ASYDVR  Driver for COM ports                      ³
;³						     ³
;³ Jonathan Gilmore, Computer Lab Jhb, 1991	     ³
;³						     ³
;³ Extracts from "Interrupt driven communications"   ³
;³ by Jim Kyle, 1987, "The MS-DOS Encyclopedia".     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
Title    ASYDVR  Driver for COM ports
;
Subttl   MS-DOS Driver Definitions
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
        Comment  *                This comments out the Dbg macro ....
Dbg	macro	ltr1,ltr2,ltr3	; used only to debug driver
	local	xxx
	push es 		; save all regs used
	push	di
	push	ax
	les	di,cs:dbgptr	; get ptr to CRT
	mov	ax,es:[di]
	mov	al,ltr1 	; move in letters
	stosw
	mov	al,ltr2
	stosw
	mov	al,ltr3
	stosw
	cmp	di,1600 	; top 10 lines only
	jb	xxx
	xor	di,di
xxx:	mov	word ptr cs:dbgptr,di
	pop	ax
	pop	di
	pop	es
	endm
        *                       ; asterisk ends commented out region
;
;		Device type codes
DevChr	equ	8000h		; This is a character device
DevBlk	equ	0000h		; This is a block device (disk)
DevIoc	equ	4000h		; This device accepts IOCTL requests
DevNon	equ	2000h		; non-IBM disk driver (block only)
DevOTB	equ	2000h		; MS-DOS 3.x out until busy supported (char)
DevOCR	equ	0800h		; MS-DOS 3.x open/close/rm supported
DevX32	equ	0040h		; MS-DOS 3.2 functions supported
DevSpc	equ	0010h		; accepts special interrupt 29h
DevClk	equ	0008h		; This is a CLOCK device
DevNul	equ	0004h		; This is a NUL device
DevSto	equ	0002h		; This is standard output
DevSti	equ	0001h		; This is standard input
;
;		Error Status Bits
StsErr	equ	8000h		; general error
StsBsy	equ	0200h		; device busy
StsDne	equ	0100h		; request completed
;
;		Error reason values for lower-order bits
ErrWp	equ	0		; write protect error
ErrUu	equ	1		; unknown unit
ErrDnr	equ	2		; drive not ready
ErrUc	equ	3		; unknown command
ErrCrc	equ	4		; CRC error
ErrBsl	equ	5		; bad drive request structure length
ErrSl	equ	6		; seek error
ErrUm	equ	7		; unknown media
ErrSnf	equ	8		; sector not found
ErrPop	equ	9		; printer out of paper
ErrWf	equ	10		; write fault
ErrRf	equ	11		; read fault
ErrGf	equ	12		; general failure
;
;		Structure of an I/O request packet header
;
pack	struc
Len	db	?		; length of record
Prtno	db	?		; unit code
Code	db	?		; command code
Stat    dw      ?               ; return status
Dosq    dd      ?               ; (unused MS-DOS queue link pointer)
Devq    dd      ?               ; (unused driver queue link pointer)
Media	db	?		; media code on read/write
Xfer    dw      ?               ; xfer address offset
Xseg    dw      ?               ; xfer address segment
Count   dw      ?               ; transfer byte count
Sector  dw      ?               ; starting sector value (block only)
pack	ends
;
;
;
Subttl	IBM-PC Hardware Driver Definitions
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
page
;
;		8259 data
PIC_b	equ	020h		; port for EOI
PIC_e	equ	021h		; port for Int enabling
EOI	equ	020h		; EOI control word
;
;		8250 port offsets
RxBuf	equ	0F8h		; base address
Baud1	equ	RxBuf+1 	; baud divisor high byte
IntEn	equ	RxBuf+1 	; interrupt enable register
IntId	equ	RxBuf+2 	; interrupt identification register
Lctrl	equ	RxBuf+3 	; line control register
Mctrl	equ	RxBuf+4 	; modem control register
Lstat	equ	RxBuf+5 	; line status register
Mstat	equ	RxBuf+6 	; modem status register
;
;		8250 LCR constants
Dlab	equ	10000000b	; divisor latch access bit
SetBrk	equ	01000000b	; send break control bit
StkPar	equ	00100000b	; stick parity control bit
EvnPar	equ	00010000b	; even parity bit
GenPar	equ	00001000b	; generate parity bit
Xstop	equ	00000100b	; extra stop bit
Wd8	equ	00000011b	; word length = 8
Wd7	equ	00000010b	; word length = 7
Wd6	equ	00000001b	; word length = 6
;
;		8250 LSR constants
xsre	equ	01000000b	; xmt SR enpty
xhre	equ	00100000b	; xmt HR empty
BrkRcv	equ	00010000b	; break received
FrmErr	equ	00001000b	; framing error
ParErr	equ	00000100b	; parity error
OveRun	equ	00000010b	; overrun error
rdta	equ	00000001b	; received data ready
AnyErr	equ	BrkRcv+FrmErr+ParErr+OveRun
;
;		8250 MCR constants
LpBk	equ	00010000b	; UART out loops to in (test)
Usr2	equ	00001000b	; Gates 8250 interrupts
Usr1	equ	00000100b	; aux user 1 output
SetRTS	equ	00000010b	; sets RTS output
SetDTR	equ	00000001b	; sets DTR output
;
;		8250 MSR constants
CDlvl   equ     10000000b       ; CD level
RIlvl   equ     01000000b       ; RI level
DSRlvl  equ     00100000b       ; DSR level
CTSlvl  equ     00010000b       ; CTS level
CDchg   equ     00001000b       ; CD change
RIchg   equ     00000100b       ; RI change
DSRchg  equ     00000010b       ; DSR change
CTSchg  equ     00000001b       ; CTS change
;
;		8250 IER constants
S_Int	equ	00001000b	; enable status interrupt
E_Int	equ	00000100b	; enable error interrupt
X_Int	equ	00000010b	; enable transmit interrupt
R_Int	equ	00000001b	; enable receive interrupt
Allint	equ	00001111b	; enable all interrupts
;
;
;
Subttl	Definitions for THIS Driver
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
page
;
;		Bit definitions for the output status byte
;		(this driver only)
LinIdl	equ	0ffh		; if all bits off, xmitter is idle
LinXof	equ	1		; output is suspended by XOFF
LinDSR	equ	2		; output is suspended until DSR comes on again
LinCTS  equ     4               ; output is suspended until CTS comes on again
;
;		Bit definitions for the input status byte
;		(this driver only)
BadInp	equ	1		; input line errors have been detected
LostDt	equ	2		; reciever buffer overflowed, data lost
OffLin  equ     4               ; device is offline now
;
;		Bit definitions for the special characteristics words
;		(this driver only)
;		InSpec controls how input from the UART is treated
InEpc	equ	0001h		; errors translate to codes with parity bit on
;
;		OutSpec controls how output to the UART is treated
OutDSR	equ	0001h		; DSR is used to throttle output data
OutCTS	equ	0002h		; CTS is used to throttle output data
OutXon	equ	0004h		; XON/XOFF is used to throttle output data
OutCdf	equ	0010h		; CD is offline signal
OutDrf	equ	0020h		; DSR is offline signal
;
unit    struc                   ; each unit has a structure defining its state
Port	dw	?		; I/O port address
Vect	dw	?		; interrupt vector offset (not interrupt number)
Isradr	dw	?		; offset to interrupt service routine
OtStat	db	Wd8		; default LCR bit settings during INIT,
				; output status bits after
InStat	db	Usr2+SetRTS+SetDTR	; MCR bit settings during INIT
				; input status bits after
InSpec	dw	InEpc		; special mode bits for INPUT
OutSpec dw	OutXon		; special mode bits for OUTPUT
Baud	dw	48              ; current baud rate divisor value (2400bps), Jonnie:was 96 (1200bps)
Ifirst	dw	0		; offset to first character in input buffer
Iavail	dw	0		; offset to next available byte
Ibuf    dw      ?               ; pointer to input buffer
Ofirst  dw      0               ; offset to first character in output buffer
Oavail  dw      0               ; offset to next available byte in output buffer
Obuf    dw      ?               ; pointer to output buffer
unit	ends
;
;
;	Beginning of driver code and data
;
driver	segment
	assume	cs:driver, ds:driver, es:driver
	org	0		; driver starts at 0
;
	dw	Async2,-1	; pointer to next device
	dw	DevChr + DevIoc ; character device with IOCTL
	dw	Strtegy 	; offset of strategy routine
	dw	Request1	; offset to interrupt entry point 1
	db	'ASY1    '	; device 1 name
Async2:
	dw	-1,-1		; pointer to next device: MS-DOS fills in
	dw	DevChr + DevIoc ; character device with IOCTL
	dw	Strtegy 	; offset of strategy routine
        dw      Request2        ; offset to interrupt entry point 2
	db	'ASY2    '	; device 2 name
;
;dbgptr dd      0b0000000h      ; Mono
;dbgptr dd      0b8000000h      ; Colour
;
;	Following is the storage area for the request packet pointer
;
PackHd	dd	0
;
;	Baud rate conversion table
Asy_baudt dw	50,2304 	; first value is desired baud rate
	  dw	75,1536 	; second is divisor register value
	  dw   110,1047
	  dw   134, 857
	  dw   150, 786
	  dw   300, 384
	  dw   600, 192
	  dw  1200,  96
	  dw  1800,  64
	  dw  2000,  58
	  dw  2400,  48
	  dw  3600,  32
	  dw  4800,  24
	  dw  7200,  16
	  dw  9600,  12
;
; table of structures
;	ASY1 defaults to the COM1 port, int 0Ch vector, XON, 1200,n,8,1
Asy_tab1:
	unit	<3F8h,30h,asy1isr,,,,,,,,in1buf,,,out1buf>
;
;	ASY2 defaults to the COM2 port, int 0Bh vector, XON, 1200,n,8,1
Asy_tab2:
	unit	<2F8h,2Ch,asy2isr,,,,,,,,in2buf,,,out2buf>
;
Bufsiz	equ	256		; input buffer size
Bufmsk	=	Bufsiz-1	; mask for calculating offsets modulo bufsiz
In1buf	db	Bufsiz dup (?)
Out1buf db	Bufsiz dup (?)
In2buf	db	Bufsiz dup (?)
Out2buf db	Bufsiz dup (?)
;
;	Following is a table of offsets to all the driver functions
;
Asy_funcs:
	dw	Init		; 0 Initialize driver
	dw	Mchek		; 1 media check (block only)
	dw	BldBPB		; 2 build BPB (block only)
	dw	Ioctlin 	; 3 IOCTL read
	dw	Read		; 4 read
	dw	Ndread		; 5 nondestructive read
	dw	Rxstat		; 6 input status
	dw	Inflush 	; 7 flush input buffer
	dw	Write		; 8 write
	dw	Write		; 9 write with verify
	dw	Txstat		; 10 output status
	dw	Txflush 	; 11 flush output buffer
	dw	Ioctlout	; 12 IOCTL write
;	Following are not used in this driver
	dw	Zexit		; 13 open (3.x only, not used)
	dw	Zexit		; 14 close (3.x only, not used)
	dw	Zexit		; 15 rem,med (3.x only, not used)
	dw	Zexit		; 16 out until busy (3.x only, not used)
	dw	Zexit		; 17
	dw	Zexit		; 18
	dw	Zexit		; 19 generic IOCTL request (3.2 only)
	dw	Zexit		; 20
	dw	Zexit		; 21
	dw	Zexit		; 22
	dw	Zexit		; 23 get logical drive map (3.2 only)
	dw	Zexit		; 24 set logical drive map (3.2 only)
;
Subttl	Driver Code
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
page
;
;	The STRATEGY ROUTINE itself:
;
Strtegy proc	far
;       dbg     'S','R',' '
	mov	word ptr cs:PackHd  , bx	; store the offset
	mov	word ptr cs:PackHd+2, es	; store the segment
	ret
Strtegy endp
;

Request1:
	push	si		; save si
	lea	si, Asy_tab1	; get the device unit table address
;       dbg     'R','1',' '
	jmp	short	Gen_request

Request2:
	push	si		; save si
	lea	si, Asy_tab2	; get the device unit table address
;       dbg     'R','2',' '

Gen_request:
;       dbg     'R','R',' '
	pushf			; save all regs
	cld
	push	ax
	push	bx
	push	cx
	push	dx
	push	di
	push	bp
	push	ds
	push	es
	push	cs		; set ds = cs
	pop	ds
	les	bx, PackHd	; get packet pointer
	lea	di, asy_funcs	; point di to jump table
	mov	al, es:code[bx] ; command code
	cbw
	add	ax, ax		; double to word
	add	di, ax
	jmp	[di]		; go do it
;
;	Exit from driver request
;
ExitP	proc	far
BsyExit:
	mov	ax, StsBsy
	jmp	short	Exit

Mchek:
BldBPB:
Zexit:	xor	ax, ax
Exit:	les	bx, PackHd	; get packet pointer
	or	ax, StsDne
        mov     es:Stat[bx],ax  ; set return status
	pop	es
	pop	ds
	pop	bp
	pop	di
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	popf
	pop	si
	ret
ExitP	endp
;
;
Subttl	Driver Service Routines
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
page
;
;
;	Read data from device
;
Read:
;       dbg     'R','d',' '
	mov	cx,es:count[bx] ; get requested nbr
	mov	di,es:xfer[bx]	; get target pointer
	mov	dx,es:xseg[bx]
	push	bx		; save for count fixup
	push	es
	mov	es,dx
	test	instat[si],BadInp or LostDt
	je	No_1err 	; no error so far
	add	sp,4		; error, flush SP
	and	instat[si], not (BadInp or LostDt)
	mov	ax,ErrRf	; error, report it
	jmp	Exit
No_1err:
	call	get_in		; go for one
	or	ah,ah
	jnz	Got_All 	; none to get now
	stosb			; store it
	loop	No_1err 	; go for more
Got_All:
	pop	es
	pop	bx
	sub	di,es:xfer[bx]	; calc number stored
	mov	es:count[bx],di ; return as count
	jmp	ZExit
;
;  Non-destructive read from device
;
Ndread:
	mov	di,ifirst[si]
	cmp	di,iavail[si]
	jne	Ndget
	jmp	Bsyexit 	; buffer empty
Ndget:
	push	bx
	mov	bx,ibuf[si]
	mov	al,[bx+di]
	pop	bx
	mov	es:media[bx],al ; return char
	jmp	ZExit
;
; Input status request
;
Rxstat:
	mov	di,ifirst[si]
	cmp	di,iavail[si]
	jne	Rxful
	jmp	Bsyexit 	; buffer empty
Rxful:
	jmp	ZExit		; have data
;
; Input flush request
;
Inflush:
	mov	ax,iavail[si]
	mov	ifirst[si],ax
	jmp	ZExit
;
; Ouput data to device
;
Write:
;       dbg     'W','r',' '
	mov	cx,es:count[bx]
	mov	di,es:xfer[bx]
	mov	ax,es:xseg[bx]
	mov	es,ax
Wlup:
        mov     al,es:[di]      ; get the byte
        inc     di
Wwait:
        call    put_out         ; put away
        cmp     ah,0
        jne     Wwait           ; wait for room
        call    Start_output    ; get it going
        loop    Wlup

        jmp     Zexit
;
;       Output status request
;
Txstat:
        mov     ax,ofirst[si]
        dec     ax
        and     ax,bufmsk
        cmp     ax,oavail[si]
        jne     txroom
        jmp     Bsyexit         ; buffer full
Txroom:
        jmp     Zexit           ; room exists
;
;       IOCTL read request, return line parameters
;
Ioctlin:
        mov     cx,es:count[bx]
        mov     di,es:xfer[bx]
        mov     dx,es:xseg[bx]
        mov     es,dx
        cmp     cx,10
        je      Doiocin
        mov     ax,errbsl
        jmp     Exit
Doiocin:
        mov     dx,port[si]     ; base port
        mov     dl,Lctrl        ; line status
        mov     cx,4            ; LCR, MCR, LSR, MSR
Getport:
        in      al,dx
        stos    byte ptr [di]
        inc     dx
        loop    Getport
        mov     ax,inspec[si]   ; spec in flags
        stos    word ptr [di]
        mov     ax,Outspec[si]  ; out flags
        stos    word ptr [di]
        mov     ax,baud[si]     ; baud rate
        mov     bx,di
        mov     di,offset Asy_baudt+2
        mov     cx,15
Baudcin:
        cmp     [di],ax
        je      Yesinb
        add     di,4
        loop    Baudcin
Yesinb:
        mov     ax,-2[di]
        mov     di,bx
        stos    word ptr [di]
        jmp     Zexit
;
;       Flush output buffer request
;
Txflush:
        mov     ax,oavail[si]
        mov     ofirst[si],ax
        jmp     Zexit
;
;       IOCTL request, change line parameters for this driver
;
Ioctlout:
        mov     cx,es:count[bx]
        mov     di,es:xfer[bx]
        mov     dx,es:xseg[bx]
        mov     es,dx
        cmp     cx,10
        je      Doiocout
        mov     ax,errbsl
        jmp     Exit
Doiocout:
        mov     dx,port[si]     ; base port
        mov     dl,Lctrl        ; line status
        mov     al,es:[di]
        inc     di
        or      al,Dlab         ; set baud
        out     dx,al
        clc
        jnc     $+2
        inc     dx              ; mdm   ctrl
        mov     al,es:[di]
        or      al,Usr2         ; int gate
        out     dx,al
        add     di,3            ; skip LSR,MSR
        mov     ax,es:[di]
        add     di,2
        mov     inspec[si],ax
        mov     ax,es:[di]
        add     di,2
        mov     Outspec[si],ax
        mov     ax,es:[di]      ; set baud
        mov     bx,di
        mov     di,offset Asy_baudt
        mov     cx,15
Baudcout:
        cmp     [di],ax
        je      Yesoutb
        add     di,4
        loop    Baudcout

        mov     dl,Lctrl        ; line ctrl
        in      al,dx           ; get LCR data
        and     al,not Dlab     ; strip
        clc
        jnc     $+2
        out     dx,al           ; put back
        mov     ax,ErrUm        ; "Unknown media"
        jmp     Exit
Yesoutb:
        mov     ax,2[di]        ; get divisor
        mov     baud[si],ax     ; save to report later
        mov     dx,port[si]     ; set divisor
        out     dx,al
        clc
        jnc     $+2
        inc     dx
        mov     al,ah
        out     dx,al
        clc
        jnc     $+2
        mov     dl,Lctrl        ; line ctrl
        in      al,dx           ; get LCR data
        and     al,not Dlab     ; strip
        clc
        jnc     $+2
        out     dx,al           ; put back
        jmp     Zexit
;
Subttl  Ring Buffer Routines
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
page
;
Put_out proc    near    ; puts al into output buffer ring
        push    cx
        push    di
        pushf
        cli
        mov     cx,oavail[si]   ; put ptr
        mov     di,cx
        inc     cx              ; bump
        and     cx,bufmsk
        cmp     cx,ofirst[si]   ; overflow ?
        je      Poerr           ; yes, don't
        add     di,obuf[si]     ; no
        mov     [di],al         ; put in buffer
        mov     oavail[si],cx
;       dbg     'p','o',' '
        mov     ah,0
        jmp     short   Poret
Poerr:
        mov     ah,-1
Poret:
        popf
        pop     di
        pop     cx
        ret
Put_out endp

Get_out proc    near    ; gets next char from output buffer ring
        push    cx
        push    di
        pushf
        cli
        mov     di,ofirst[si]   ; get ptr
        cmp     di,oavail[si]   ; put ptr
        jne     Ngoerr
        mov     ah,-1           ; empty
        jmp     short   Goret
Ngoerr:
;       dbg     'g','o',' '
        mov     cx,di
        add     di,obuf[si]
        mov     al,[di]         ; get char
        mov     ah,0
        inc     cx              ; bump ptr
        and     cx,bufmsk       ; wrap
        mov     ofirst[si],cx
Goret:
        popf
        pop     di
        pop     cx
        ret
Get_out endp

Put_in  proc    near    ; puts al into input ring buffer
        push    cx
        push    di
        pushf
        cli
        mov     di,iavail[si]
        mov     cx,di
        inc     cx
        and     cx,bufmsk
        cmp     cx,ifirst[si]
        jne     Npierr
        mov     ah,-1
        jmp     short   Piret
Npierr:
        add     di,ibuf[si]
        mov     [di],al
        mov     iavail[si],cx
;       dbg     'p','i',' '
        mov     ah,0
Piret:
        popf
        pop     di
        pop     cx
        ret
Put_in  endp

Get_in  proc    near    ; gets char from input buffer ring into al
        push    cx
        push    di
        pushf
        cli
        mov     di,ifirst[si]
        cmp     di,iavail[si]
        je      Gierr
        mov     cx,di
        add     di,ibuf[si]
        mov     al,[di]
        mov     ah,0
;       dbg     'g','i',' '
        inc     cx
        and     cx,bufmsk
        mov     ifirst[si],cx
        jmp     short   Giret
Gierr:
        mov     ah,-1
Giret:
        popf
        pop     di
        pop     cx
        ret
Get_in  endp
;
Subttl  Interrupt Dispatcher Routines
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
page
;
Asy1isr:
        sti
        push    si
        lea     si,asy_tab1
        jmp     short   Int_serve
Asy2isr:
        sti
        push    si
        lea     si,asy_tab2

Int_serve:
        push    ax
        push    bx
        push    cx
        push    dx
        push    di
        push    ds
        push    cs      ; set DS = CS
        pop     ds
Int_exit:
;       dbg     'I','x',' '
        mov     dx,port[si]     ; base address
        mov     dl,IntId        ; check int id
        in      al,dx
        cmp     al,00h          ; dispatch filter
        je      Int_modem
        jmp     short   Int_mo_no
Int_modem:
;       dbg     'M','S',' '
        mov     dl,Mstat
        in      al,dx           ; read MSR content
        test    al,CDlvl        ; CD present ?
        jnz     Msdsr           ; yes, test for DSR
        test    Outspec[si],Outcdf      ; no, is CD offline ?
        jz      Msdsr
        or      Instat[si],Offlin
Msdsr:
        test    al,DSRlvl       ; DSR present ?
        jnz     Dsron           ; yes, handle it
        test    Outspec[si],Outdsr      ; no, is DSR throttle ?
        jz      Dsroff
        or      Otstat[si],LinDSR       ; yes, throttle down
Dsroff:
        test    Outspec[si],Outdrf      ; is DSR off line ?
        jz      Mscts
        or      Instat[si],Offlin       ; yes, set flag
        jmp     short   Mscts
Dsron:
        test    Otstat[si],LinDSR       ; throttled for DSR ?
        jz      Mscts
        xor     Otstat[si],LinDSR       ; yes, clear it out
        call    Start_output
Mscts:
        test    al,CTSlvl       ; CTS present ?
        jnz     Ctson           ; yes, handle it
        test    Outspec[si],OutCTS      ; no, is CTS throttle ?
        jz      Int_exit2
        or      Otstat[si],LinCTS       ; yes, shut it down
        jmp     short   Int_exit2
Ctson:
        test    Otstat[si],LinCTS       ; throttled for CTS ?
        jz      Int_exit2
        xor     Otstat[si],LinCTS       ; yes, clear it out
        jmp     short   Int_exit1
Int_mo_no:
        cmp     al,02h
        jne     Int_tx_no
Int_txmit:
;       dbg     'T','x',' '
Int_exit1:
        call    Start_output    ; try to send another
Int_exit2:
        jmp     Int_exit
Int_tx_no:
        cmp     al,04h
        jne     Int_rec_no
Int_receive:
;       dbg     'R','x',' '
        mov     dx,port[si]
        in      al,dx           ; take char from 8250
        test    Outspec[si],OutXon      ; is XON/XOFF enabled ?
        jz      Stuff_in        ; no
        cmp     al,'S' and 01FH ; yes, is this XOFF ?
        jne     Isq             ; no, check for XON
        or      Otstat[si],LinXof       ; yes, disable output
        jmp     Int_exit2       ; don't store this one
Isq:
        cmp     al,'Q' and 01FH ; is this XON ?
        jne     Stuff_in        ; no, save it
        test    Otstat[si],LinXof       ; yes, waiting ?
        jz      Int_exit2       ; no, ignore it
        xor     Otstat[si],LinXof       ; yes, clear the XOFF bit
        jmp     Int_exit1       ; and try to resume xmt
Int_rec_no:
        cmp     al,06h
        jne     Int_done
Int_rxstat:
;       dbg     'E','R',' '
        mov     dl,Lstat
        in      al,dx
        test    Inspec[si],InEpc        ; return them as codes ?
        jz      Nocode          ; no, just set error alarm
        and     al,AnyErr       ; yes, mask off all but error bits
        or      al,080h
Stuff_in:
        call    Put_in          ; put input char in buffer
        cmp     ah,0            ; did it fit ?
        je      Int_exit3       ; yes, all OK
        or      Instat[si],LostDt       ; no, set data lost bit
Int_exit3:
        jmp     Int_exit
Nocode:
        or      Instat[si],BadInp
        jmp     Int_exit3
Int_done:
        clc
        jnc     $+2
        mov     al,EOI          ; all done now
        out     PIC_b,al
        pop     ds              ; restore regs
        pop     di
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        pop     si
        iret

Start_output    proc    near
        test    Otstat[si],LinIdl       ; blocked ?
        jnz     Dont_start      ; yes, no output
        mov     dx,port[si]     ; no, check UART
        mov     dl,Lstat
        in      al,dx
        test    al,xhre         ; empty ?
        jz      Dont_start      ; no
        call    Get_out         ; yes, anything waiting ?
        or      ah,ah
        jnz     Dont_start      ; no
        mov     dl,RxBuf        ; yes, send it out
        out     dx,al
;       dbg     's','o',' '
Dont_start:
        ret
Start_output    endp
;
Subttl  Initialisation Request Routine
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
page
;
Init:
        lea     di,$            ; release rest ...
        mov     es:xfer[bx],di
        mov     es:xseg[bx],cs

        mov     dx,port[si]     ; base port
        mov     dl,Lctrl
        mov     al,Dlab         ; enable divisor
        out     dx,al
        clc
        jnc     $+2
        mov     dl,RxBuf
        mov     ax,Baud[si]     ; set baud
        out     dx,al
        clc
        jnc     $+2
        inc     dx
        mov     al,ah
        out     dx,al
        clc
        jnc     $+2

        mov     dl,Lctrl        ; set LCR
        mov     al,OtStat[si]   ; from table
        out     dx,al
        mov     OtStat[si],0    ; clear status
        clc
        jnc     $+2
        mov     dl,IntEn        ; IER
        mov     al,AllInt       ; enable ints in 8250
        out     dx,al
        clc
        jnc     $+2
        mov     dl,Mctrl        ; set MCR
        mov     al,InStat[si]   ; from table
        out     dx,al
        mov     InStat[si],0    ; clear status
ClRgs:
        mov     dl,LStat        ; clear LSR
        in      al,dx
        mov     dl,RxBuf        ; clear RX reg
        in      al,dx
        mov     dl,MStat        ; clear MSR
        in      al,dx
        mov     dl,IntId        ; IID reg
        in      al,dx
        in      al,dx
        test    al,1            ; int pending ?
        jz      ClRgs           ; yes, repeat

        cli
        xor     ax,ax           ; set int vec
        mov     es,ax
        mov     di,vect[si]
        mov     ax,IsrAdr[si]   ; from table
        stosw
        mov     es:[di],cs

        in      al,PIC_e        ; get 8259
        and     al,0E7h         ; com 1/2 mask
        clc
        jnb     $+2
        out     PIC_e,al
        sti

        mov     al,EOI          ; now send EOI, just in case
        out     PIC_b,al

;       dbg     'D','I',' '
; Jonnie: added notice from here
SigStart:
        test    MsgDone,1
        jnz     Sig2
        mov     MsgDone,1
        mov     ah,9
        mov     dx,offset Msg1
        int     21h
        mov     ah,9
        mov     dx,offset Msg2
        int     21h
        mov     ah,9
        mov     dx,offset Msg3
        int     21h
        jmp     short   SigEnd
Sig2:
        mov     ah,9
        mov     dx,offset Msg4
        int     21h
SigEnd:
; Jonnie: added notice to here
        jmp     Zexit

; Jonnie: added notice from here
MsgDone DB      0
Msg1    DB      'ASYDVR - Asynchronous Communications Driver Version 1.00',0ah,0dh,'$'
Msg2    DB      'Copyright (c) 1991 Computer Lab',0ah,0dh,'$'
Msg3    DB      'ASY1: installed',0ah,0dh,'$'
Msg4    DB      'ASY2: installed',0ah,0dh,'$'
; Jonnie: added notice to here
Driver  ends
        end
