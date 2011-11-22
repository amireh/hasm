. SOURCE:
.   System Software, An Introduction to Systems Programming 3rd Ed. (L. L. Beck)
.   Figure 2.12, Page 81

COPY    START    0 . FOOBAR
FIRST   STL      RETADR
CLOOP   JSUB     RDREC
        LDA      LENGTH
        COMP     #0
        JEQ      ENDFIL
        JSUB     WRREC
        J        CLOOP
ENDFIL  LDA      =C'EOF'
        STA      BUFFER
        LDA      #3
        STA      LENGTH
        JSUB     WRREC
        J        @RETADR
        USE      CDATA
RETADR  RESW     1
LENGTH  RESW     1
        USE      CBLKS
BUFFER  RESB     4096
BUFEND  EQU      *
MAXLEN  EQU      BUFEND-BUFFER
.
. SUBROUTINE TO READ RECORD INTO BUFFER
.
        USE
RDREC   CLEAR   X
        CLEAR   A
        CLEAR   S
        +LDT    #MAXLEN
RLOOP   TD      INPUT
        JEQ     RLOOP
        RD      INPUT
        COMPR   A,S
        JEQ     EXIT
        STCH    BUFFER,X
        TIXR    T
        JLT     RLOOP
EXIT    STX     LENGTH
        RSUB
        USE     CDATA
INPUT   BYTE    X`F1`
.
. SUBROUTINE TO WRITE RECORD FROM BUFFER
.
        USE
WRREC   CLEAR   X
        LDT     LENGTH
WLOOP   TD      =X`05`
        JEQ     WLOOP
        LDCH    BUFFER,X
        WD      =X`05`
        TIXR    T
        JLT     WLOOP
        RSUB
        USE     CDATA
        LTORG
*       =C`EOF`
*       =X`05`
        END     FIRST