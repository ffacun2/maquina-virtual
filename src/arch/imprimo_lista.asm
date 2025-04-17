mov edx, ds
xor efx, efx
otro: mov ch, 2
mov cl, 1
mov al, 1
sys 1
cmp efx, 5
jz imprimo
add edx, 2
add efx, 1
jmp otro
imprimo: mov edx, ds
aca: mov ch, 2
mov cl, 1
mov al, 9
sys 2
cmp efx, 0
jz fin
sub efx,1
sub edx, 2
jmp aca
fin: stop