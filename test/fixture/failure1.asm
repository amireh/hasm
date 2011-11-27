COPY    START
FIRST   STL       RETADR
CLOOP   +JSUB     RDREC
        LDA       LENGTH
        COMP      #0
        JEQ       ENDFIL
        +JSUB     WRREC
        J         CLOOP
ENDFIL  LDA       =C'EOF'
        STA       BUFFER
        LDA       #3
        BYTE      FOO-5
        STA       LENGTH
        +JSUB     WRREC
        J         @RETADR
        RETADR    RESW 1
        LENGTH    RESW 1
        LTORG
;~ *       =C'EOF'
BUFFER  RESB      4096
BUFEND  EQU       *
MAXLEN  EQU       BUFEND-BUFFER
