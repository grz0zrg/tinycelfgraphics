SECTIONS {
    . = 0x400056;
  
    combined . : AT(0x400056) ALIGN(1) SUBALIGN(1) {
        *(.text*)
        *(.data*)
        *(.rodata*)
        *(.bss*)
    }
}
