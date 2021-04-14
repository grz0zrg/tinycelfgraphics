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

    ; based on 'lintro' 128 bytes intro by frag/fsqrt: https://www.pouet.net/prod.php?which=58560
	mov ebx, 10020h
	mov cl, 2	;e_shnum ;O_RDWR
	mov al, 5	;e_shstrndx
	; end of elf header

	;open("/dev/fb0", O_RDRW);
	int 80h		;eax <- fd

	;mmap(NULL, buflen, PROT_WRITE, MAP_SHARED, fd, 0);
	push edx	      ;edx = 0
	push eax	      ;fd
	push byte 1	      ;MAP_SHARED
	mov al, 90
	push eax	      ;we need to set second bit for PROT_WRITE, 90 = 01011010 and setting PROT_WRITE automatically set PROT_READ
	push width*height*4  ;buffer size
	push edx	      ;NULL
	mov ebx, esp	  ;args pointer
	int 80h		      ;eax <- buffer pointer

; see float note in the C code
; dd 0.5

incbin "payload.bin"

filesize equ $ - ehdr

