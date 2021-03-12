    bits 32
    org 0x00400000

ehdr:
    db  0x7f, "ELF"  ; magic
    db  1, 1, 1      ; 32-bits, little endian, version 1

    ; use unused ELF field to store framebuffer target string
    ; this also use the ABI target / version field (may be unsafe ?)
    db "/dev/fb0", 0

    dw  2            ; e_type = executable
    dw  3            ; e_machine
    dd  1            ; e_version
    dd  entry        ; e_entry
    dd  phdr - $$    ; e_phoff
    dd  0            ; e_shoff
    dd  0            ; e_flags
    dw  ehdrsize     ; e_ehsize
    dw  phdrsize     ; e_phentsize
    dw  1            ; e_phnum
    dw  0, 0, 0      ; e_sh*

ehdrsize  equ  $ - ehdr

phdr:
    dd  1            ; p_type = loadable program segment
    dd  0            ; p_offset
    dd  $$, $$       ; p_vaddr, p_paddr
    dd  filesize     ; p_filesz
    dd  filesize     ; p_memsz
    dd  5            ; p_flags = rwx
    dd  0x1000       ; p_align

phdrsize equ  $ - phdr

; see float note in the C code
; dd 0.5

incbin "payload.bin"

filesize equ $ - ehdr

