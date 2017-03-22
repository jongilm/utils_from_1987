{$INCLUDE: 'PORT.INT'}

program testdtr(input,output);
USES PORT;

VAR
   RS232Base [ORIGIN 16#0040:16#0000] : ARRAY [1..4] OF WORD;

   PROCEDURE RaiseDTRandRTS(PortNo : INTEGER);
   !ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   !³                                      ³
   !ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
   VAR
      PortAdr    : WORD;
      ModemCtl   : BYTE;

   BEGIN
      PortAdr    := RS232Base[PortNo] + 4; ! Modem control register
      ModemCtl   := INP(PortAdr);
      OUTP(PortAdr,ModemCtl OR #03);    ! raise dtr and rts
   END;

   PROCEDURE RaiseDTR(PortNo : INTEGER);
   !ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   !³                                      ³
   !ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
   VAR
      PortAdr    : WORD;
      ModemCtl   : BYTE;

   BEGIN
      PortAdr    := RS232Base[PortNo] + 4; ! Modem control register
      ModemCtl   := INP(PortAdr);
      OUTP(PortAdr,ModemCtl OR #01);    ! raise dtr and rts
   END;

   PROCEDURE DropDTR(PortNo : INTEGER);
   !ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   !³                                      ³
   !ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
   VAR
      PortAdr    : WORD;
      ModemCtl   : BYTE;

   BEGIN
      PortAdr    := RS232Base[PortNo] + 4; ! Modem control register
      ModemCtl   := INP(PortAdr);
      OUTP(PortAdr,ModemCtl AND (NOT #01));    ! raise dtr and rts
   END;

   PROCEDURE RaiseRTS(PortNo : INTEGER);
   !ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   !³                                      ³
   !ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
   VAR
      PortAdr    : WORD;
      ModemCtl   : BYTE;

   BEGIN
      PortAdr    := RS232Base[PortNo] + 4; ! Modem control register
      ModemCtl   := INP(PortAdr);
      OUTP(PortAdr,ModemCtl OR #02);    ! raise dtr and rts
   END;

   PROCEDURE DropRTS(PortNo : INTEGER);
   !ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   !³                                      ³
   !ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
   VAR
      PortAdr    : WORD;
      ModemCtl   : BYTE;

   BEGIN
      PortAdr    := RS232Base[PortNo] + 4; ! Modem control register
      ModemCtl   := INP(PortAdr);
      OUTP(PortAdr,ModemCtl AND (NOT #02));    ! raise dtr and rts
   END;

!   PROCEDURE RaiseDTR; BEGIN PutBSPort( pBSWA, 5, #80, #80 );             END;
!   PROCEDURE DropDTR;  BEGIN PutBSPort( pBSWA, 5, #80, #00 ); Delay(100); END;
!   PROCEDURE RaiseTX;  BEGIN PutBSPort( pBSWA, 5, #10, #10 );             END;
!   PROCEDURE DropTX;   BEGIN PutBSPort( pBSWA, 5, #10, #00 );             END;
!   PROCEDURE RaiseRTS; BEGIN PutBSPort( pBSWA, 5, #02, #02 );             END;
!   PROCEDURE DropRTS;  BEGIN PutBSPort( pBSWA, 5, #02, #00 );             END;

BEGIN
   RaiseDTR(2);
   readln;
   RaiseRTS(2);
   readln;
   DropDTR(2);
   readln;
   DropRTS(2);
END.
