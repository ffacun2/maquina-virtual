MAIN:   PUSH BP
        MOV BP, SP
        PUSH 0

OTRO:   CMP [BP-4], 10
        JZ SIGUE
            MOV EDX, BP
            SUB EDX, 4
            MOV CL, 1
            MOV CH, 4
            MOV AL, 1
            SYS 2
        ADD [BP-4], 1
        JMP OTRO

SIGUE:  SYS 7; Limpiar la pantalla

        ADD SP, 4
        MOV SP, BP
        POP BP
        RET
        STOP