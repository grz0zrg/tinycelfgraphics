    bits 64
    org 0x100000000

ehdr:
    db  0x7f, "ELF"  ; magic
    db  2, 1, 1;, 0  ; 64-bits, little endian, version 1

    ; use unused ELF field to store framebuffer target string
    ; this also use the ABI target / version field (may be unsafe ?)
    db "/dev/fb0", 0

    dw  2           ; e_type = executable
    dw  0x3e        ; e_machine = x86-64
    dd  1           ; e_version
    dd  entry       ; e_entry
phdr:
    dd	1			; p_type
	dd	phdr - $$	; e_phoff	; p_flags
	dd	0			; p_offset
	dd	0			; e_shoff
	dq	$$			; p_vaddr
	; e_flags
	dw	0x40		; e_ehsize	; p_paddr
	dw	0x38		; e_phentsize
	dw	1			; e_phnum
	dw	0			; e_shentsize
	dq	filesize	; e_shnum	; p_filesz
	; e_shstrndx
	dq	filesize	; p_memsz
	dq	0x00200000	; p_align


; any data can also go here and are accessible in the C code through pointers but the 'entry' should be correcty updated (see Makefile) so it must have an offset added corresponding to the size of the data
; note : if you use float constants in your C code, youll most likely want to load them from here manually through pointers address
;        because float constants are loaded from memory and since we don't use any sections any float constants will vanish from the resulting binary
;        unless they are defined here and referenced via pointers in the C code

; dd 0.5

incbin "payload.bin"

filesize equ $ - ehdr

