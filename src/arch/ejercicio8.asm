OTRO:   MOV [4], 0; Contador de 1s
        MOV EDX, DS
        MOV CH, 4
        MOV CL, 1
        MOV AL, 0x01
        SYS 1
        CMP [EDX], 0
        JN FIN
ITER:   CMP [EDX], 0
        JZ ESCRIB
        MOV EFX, [EDX]
        AND EFX, 1
        ADD [4], EFX
        SHR [EDX], 1
        JMP ITER
ESCRIB: MOV EDX, DS
        ADD EDX, 4
        MOV CH, 4
        MOV CL, 1
        MOV AL, 0x01
        SYS 2
        JMP OTRO
FIN:    STOP