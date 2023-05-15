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
}
