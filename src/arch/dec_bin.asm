mov ebx, 0
mov edx, [0]
mov ch,4
mov cl, 1
mov al, 1
sys 1
mov eax,[edx]
sigo: add efx, [4]
div eax, 2
mov [efx], ac
add ebx, 1
cmp eax, 0
jnz sigo
muestro: mov edx, efx
mov ch, 4
mov cl, 1
mov al, 1
sys 2
sub efx, 4
sub ebx, 1
cmp ebx, 0
jnz muestro
stop