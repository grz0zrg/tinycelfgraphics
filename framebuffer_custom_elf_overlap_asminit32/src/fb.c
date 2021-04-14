#include <sys/mman.h>
#include <fcntl.h>
#ifdef __x86_64
    #include <asm/unistd_64.h>
#else
    #include <asm/unistd_32.h>
#endif

// framebuffer settings (width / height / components should match actual framebuffer settings)
#define FRAMEBUFFER_COMPONENTS 4
#define FRAMEBUFFER_LENGTH (WIDTH * HEIGHT * FRAMEBUFFER_COMPONENTS)

void _start() {
#ifdef __i386__
    // see elf32.s (all C framebuffer init code was replaced by optimized x86 assembly, the buffer pointer is into eax at this point)
    register unsigned int *buffer __asm__("eax");
#endif

    unsigned int x = WIDTH / 2;
    unsigned int y = HEIGHT / 2;

    unsigned int index = x + y * WIDTH;

    buffer[index] = 0xffffff; // white

    // note for float : if you use float constants in your C code, youll most likely want to load the data through pointers address (see below)
    //                  because float constants are loaded from memory and since we don't keep sections any float constants will vanish from the resulting binary
    //                  unless they are defined in elf64.s or elf32.s (just before the payload) and referenced via pointers in the C code, this allow even more fine controls over the data
    // Example (note : you must uncomment the constant definition line in elf64.s / elf32.s and add an offset of 4 in the Makefile) :
    // note : this is for 64 bits, the address must be changed for 32 bits
    // float my_float_constant = *(float*)0x100000054;
}
