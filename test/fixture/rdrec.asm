;
; SUBROUTINE TO READ RECORD INTO BUFFER
;
RDREC   CSECT
        EXTREF    BUFFER,LENGTH,BUFEND
        CLEAR     X
        CLEAR     A
        CLEAR     S
        LDT       MAXLEN
RLOOP   TD        INPUT
        JEQ       RLOOP
        RD        INPUT
        COMPR     A,S
        JEQ       EXIT
        +STCH     BUFFER,X
        TIXR      T
        JLT       RLOOP
EXIT    +STX      LENGTH
        RSUB
INPUT   BYTE      X'F1'
MAXLEN  WORD      BUFEND-BUFFER
