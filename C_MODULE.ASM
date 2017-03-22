; *** Public declaration of subroutines and data
              PUBLIC    XXXXXX

; *** External data needed by these subroutines
              EXTRN

; *** External subroutines called by these subroutines
              EXTRN

_TEXT         SEGMENT BYTE PUBLIC 'CODE'
              ASSUME    CS:_TEXT

; *** Declaration of Equates
PARMD         EQU       12
PARMC         EQU       10
PARMB         EQU       08
PARMA         EQU       06

; *** Declaration of data
DATA1         DW        0000

;旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
;�                                                                            �
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸
XXXXXX        PROC      FAR
              push      bp
              mov       bp,sp
              mov       ax,[bp]+PARMA
              mov       bx,[bp]+PARMB
              mov       cx,[bp]+PARMC



              pop       bp
              ret
XXXXXX        ENDP
;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
_TEXT         ENDS
              END
