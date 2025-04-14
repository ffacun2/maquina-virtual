mov [3],'a'
mov [2],'l'
mov [1],'o'
mov [0],'H'
mov edx, ds
add edx, 3
mov ch, 1
mov cl, 4
mov al, 2
sys 2
stop