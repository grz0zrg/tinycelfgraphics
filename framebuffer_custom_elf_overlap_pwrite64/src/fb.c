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
    // see elf32.s (all C framebuffer init code was replaced by optimized x86 assembly, the buffer pointer is into sp at this point)
    register unsigned int *buffer __asm__("sp");
#endif

    unsigned int x = WIDTH / 2;
    unsigned int y = HEIGHT / 2;

    unsigned int index = x + y * WIDTH;

    buffer[index] = 0xffffff; // white

    /* note : can do some cheap scrolling (screen offset) / glitch effects by passing different parameters instead of zeroing out registers
original:
        mov ecx,esp  ; buffer ptr
        mov edx,ebp  ; screen size
        xor esi,esi  ; seek to beginning of screen
        xor edi,edi  
        mov ebx,3    ; fd of framebuffer
        mov eax,0xb5 ; pwrite64
        int 0x80     ; pwrite64 to framebuffer

optimized:
            ; pwrite64
            mov ecx,esp  ; buffer ptr
            mov edx,ebp  ; screen size
            xor esi,esi  ; seek to beginning of screen
            xor edi,edi
            push 3
            pop ebx
            xor eax,eax
            mov al,0xb5
            int 0x80     ; pwrite64 to framebuffer
    */
#ifdef __i386__
    __asm__ volatile (".byte 0x89,0xe1,0x89,0xea,0x31,0xf6,0x31,0xff,0x6a,0x03,0x5b,0x31,0xc0,0xb0,0xb5,0xcd,0x80");
#endif
}
