;DOCUMENTACIÓN
; [0]: cantidad de elementos del primer vector (N)
; [4]: cantidad de elementos del segundo vector
; [8]: contendrá la dirección de memoria del segundo vector
; [12]: primer vector
; EBX: cantidad de elementos que debería tener el segundo vector

; Ingreso los 2 vectores
        MOV [0], 0; 		[0] es la cantidad de elementos del vector
        MOV EDX, DS
        ADD EDX, 12; 		12 será la dirección de memoria base del primer vector
OTRO:   MOV CH, 4
        MOV CL, 1
        MOV AL, 1
        SYS 1
        CMP [EDX], 0
        JN SIGUE
        ADD EDX, 4
        ADD [0], 1
        JMP OTRO
SIGUE:  MOV [8], EDX; 	[8] contendrá la dirección de memoria base del segundo vector
        MOV [4], 0; 		[4] contará los elementos del otro vector
        MOV EBX, [0]
        SUB EBX, 1;		EBX tiene la cant de elementos que debe tener v2
OTRO2:  MOV CH, 4
        MOV CL, 1
        MOV AL, 1
        SYS 1
        ADD [4], 1
        ADD EDX, 4
        CMP [4], EBX
        JN OTRO2
			; Ahora veo qué número de v1 no está en v2
       			 ; for (i:=0, i<N, i++)
        MOV EEX, 0;		 EEX es mi variable de control para el ciclo for (i:=0)
OTRO3:  CMP EEX, [0]
        JZ FIN
       			 ; for (j:=0, j<N-1, j++)
        MOV EFX, 0; 		EFX es variable de control para otro ciclo for (j:=0)
OTRO4:          CMP EFX, [0]
                JZ SIGUE4
                			; Apunto EBX a v1[i] donde v1[i] es un entero
                MOV EBX, [12]; 	EBX ahora apunta a un byte del primer vector
                MOV AC, 4
                MUL AC, EEX
                ADD EBX, AC
            			    ; Apunto EAX a v2[j]
                MOV EAX, [8]; 	EAX ahora apunta a un byte del segundo vector
                MOV AC, 4
                MUL AC, EFX
                ADD EAX, AC
              			  ; for(k:=0, k<4, k++)
                MOV EDX, 0; 	EDX ahora es variable de control para otro ciclo for (k:=0)
OTRO5:  CMP EDX, 4
                JZ FINCICLO3
                CMP [EAX], [EBX];	 Comparo v1[i][k] con v2[j][k]
                 JNZ FINCICLO4
                 ADD EAX, 1
                ADD EBX, 1
                ADD EDX, 1
                JMP OTRO5
FINCICLO4:      MOV EDX, 0
                ADD EFX, 1
                JMP OTRO4
SIGUE4: CMP EFX, [4]
        JZ FIN
FINCICLO3: MOV EFX, 0
        ADD EEX, 1
FIN:    MOV EDX, DS
        ADD EDX, 12
        MUL EEX, 4
        ADD EDX, EEX
        MOV CH, 4
        MOV CL, 1
        MOV AL, 1
        SYS 2
        STOP