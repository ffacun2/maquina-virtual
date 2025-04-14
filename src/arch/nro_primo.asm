mov edx, ds
mov ch, 4
mov cl, 1
mov al, 1
sys 1
mov ecx, [edx]
mov eax, ecx
cmp eax, 1
jnp noprimo
mov ebx, 2
divido: mov eax, ecx
div eax, ebx
cmp ac, 0
jz noprimo
add ebx, 1
cmp ecx, ebx
jz esprimo
jmp divido

noprimo: mov [edx], 0
jmp imprimo

esprimo: mov [edx], 1

imprimo: mov ch, 4
mov cl, 1
mov al, 1
sys 2
stop