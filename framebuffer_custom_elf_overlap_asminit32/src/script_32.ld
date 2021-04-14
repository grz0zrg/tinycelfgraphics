SECTIONS {
    . = 0x0001002E;
  
    combined . : AT(0x0001002E) ALIGN(1) SUBALIGN(1) {
        *(.text*)
        *(.data*)
        *(.rodata*)
        *(.bss*)
    }
}
