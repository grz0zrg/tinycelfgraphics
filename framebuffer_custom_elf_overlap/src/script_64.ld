SECTIONS {
    . = 0x100000054;
  
    combined . : AT(0x100000054) ALIGN(1) SUBALIGN(1) {
        *(.text*)
        *(.data*)
        *(.rodata*)
        *(.bss*)
    }
}
