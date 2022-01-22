    bits 32
    org 0x00010000

ehdr:
    db 0x7f, "ELF"
    dd 1
    dd 0
    dd $$
    dw 2
    dw 3
    dd entry
    dd entry
    dd 4
    db "/dev/fb0",0

    ; based on 'lintro' 128 bytes intro by frag/fsqrt: https://www.pouet.net/prod.php?which=58560
    mov ebx, 10020h
    inc ecx
    mov al, 5 ;e_shstrndx
    ; end of elf header

    ; open("/dev/fb0", O_RDRW);
    int 80h ; eax <- fd

    ; make more space on the stack for display buffer (note : max size may be limited)
    mov ebp, width * height * 4
    sub esp, ebp

incbin "payload.bin"

filesize equ $ - ehdr

