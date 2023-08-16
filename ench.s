; script to enable both adlib gold channels in pcem
; (maybe real hardware? the official drivers never
; actually set it so i really dont know)
pusha

mov dx, 0x38A
mov al, 0xFF
out dx, al

; select control register 8
mov al, 8
out dx, al

inc dx; 0x38B
in al, dx
or al, 6 ; both channels enabled
out dx, al

popa
ret
