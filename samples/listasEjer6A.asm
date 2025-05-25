; listas dinámicas enlasadas de enteros
; [0] valor entero
; [4] puntero a siguiente elemento

; Estructura del nodo:
;    +------------+
;  0 |   entero   |  (4 bytes)
;    +------------+
; +4 | *siguiente |  (4 bytes)
;    +------------+
nodo_size   equ     8
val         equ     0
sig         equ     4


;----------------------------------------
; crea un nuevo nodo de la lista 
; parámetros: +8 valor entero
;----------------------------------------
; invocación:
; push  <valor entero>
; call  nodo_nuevo
; add   sp, 4
; << eax puntero al nuevo nodo
;----------------------------------------
new_nodo:   push    bp
            mov     bp,sp
            
            push    8
            call    alloc
            add     sp,4

            mov     [eax], [bp+8]
            mov     [eax+4], null

            mov     sp,bp
            pop     bp
            ret     

; insertar un nodo en una lista ordenada
; parametro +4: doble puntero a la lista
; parametro +8: puntero al nodo a insertar
insert_nodo:    push    bp
                mov     bp,sp
                push    eax
                push    ebx
                push    edx
                
                mov     edx, [bp+8]     ; doble puntero a la lista
                mov     ebx, [edx]      ; puntero a la lista
                mov     eax, [bp+12]    ; puntero al nodo a insertar

                cmp     ebx, null
                jz      insert_actual

                cmp     [eax], [ebx]    ; si el nodo a insertar es menor que el primero
                jnp     insert_actual   ; insertar al principio

                add     ebx, 4
                push    eax
                push    ebx
                call    insert_nodo
                add     sp,8
                jmp     insert_fin

insert_actual:  mov     [eax+4], [edx]   ; insertar al principio
                mov     [edx], eax
                jmp     insert_fin
insert_fin:     pop     edx
                pop     ebx
                pop     eax
                mov     sp,bp
                pop     bp
                ret

intercalar:     push bp
                mov  bp, sp
                push eax
                push ebx
                push ecx
                push edx

                mov  ebx, [bp+8]      ; lista1
                mov  ecx, [bp+12]     ; lista2
                mov  edx, [bp+16]     ; puntero doble a lista3

loop:           cmp  ebx, null
                jz   lista1_vacia
                cmp  ecx, null
                jz   lista2_vacia

                cmp  [ebx+val], [ecx+val]
                jnp  usar_lista1       ; si lista1[i] <= lista2[i] → usar lista1

usar_lista2:    push [ecx+val]
                call new_nodo
                add  sp, 4
                push eax
                push edx               ; edx = &lista3
                call insert_nodo
                add  sp, 8
                mov  ecx, [ecx+sig]
                jmp  loop

usar_lista1:    push [ebx+val]
                call new_nodo
                add  sp, 4
                push eax
                push edx
                call insert_nodo
                add  sp, 8
                mov  ebx, [ebx+sig]
                jmp  loop

lista1_vacia:   cmp  ecx, null
                jz   fin
.next2:         push [ecx+val]
                call new_nodo
                add  sp, 4
                push eax
                push edx
                call insert_nodo
                add  sp, 8
                mov  ecx, [ecx+sig]
                cmp  ecx, null
                jnz  .next2
                jmp  fin

lista2_vacia:   cmp  ebx, null
                jz   fin
.next1:         push [ebx+val]
                call new_nodo
                add  sp, 4
                push eax
                push edx
                call insert_nodo
                add  sp, 8
                mov  ebx, [ebx+sig]
                cmp  ebx, null
                jnz  .next1

fin:            pop edx
                pop ecx
                pop ebx
                pop eax
                mov sp, bp
                pop bp
                ret

         


                
                

                