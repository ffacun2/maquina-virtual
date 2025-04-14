mov efx, 0
xor [4],[4]
otro: mov edx, ds
mov ch, 4
mov cl, 1
mov al, 1
sys 1
cmp [edx], 0
jn sigue
add, [4], [edx]
add efx, 1
jmp otro
sigue: cmp efx,0
jz fin
div [4], efx
fin: mov edx, ds
add edx, 4
mov ch, 4
mov cl, 1
mov al, 1
sys 2
stop