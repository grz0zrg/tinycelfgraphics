SECTIONS {
    . = 0x00010029;
  
    combined . : AT(0x00010029) ALIGN(1) SUBALIGN(1) {
        *(.text*)
        *(.data*)
        *(.rodata*)
        *(.bss*)
    }
}
