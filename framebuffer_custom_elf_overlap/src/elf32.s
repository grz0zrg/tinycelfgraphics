    bits 32
    org 0x00010000

ehdr:
    db 0x7f, "ELF"  ; magic
    dd 1
    dd 0
    dd $$
    dw 2
    dw 3
    dd entry
    dd entry
    dd 4
    db "/dev/fb0"
    dw 0 ; dw 0x34 ; needed for end of string, does it break compat ?
    dw 0x20
    db 1
    db 0 ; note : seem mandatory
    ; dw 0
    ; dw 0
    ; dw 0

; see float note in the C code
; dd 0.5

incbin "payload.bin"

filesize equ $ - ehdr

