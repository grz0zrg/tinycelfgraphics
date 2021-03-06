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

inline static int sys_open(const char *filepath, int flags, int mode) {
    int r;
#ifdef __x86_64
    __asm__ volatile("syscall"
        : "=a"(r)
        : "0"(__NR_open), "D"(filepath), "S"(flags), "d"(mode)
        : "cc", "rcx", "r11", "memory");
#else
    __asm__ volatile("int $0x80"
        : "=a"(r)
        : "0"(__NR_open), "b"(filepath), "c"(flags), "d"(mode)
        : "cc", "edi", "esi", "memory");
#endif
  return r;
}

inline static unsigned int *sys_mmap(unsigned int *addr, unsigned long length, unsigned long prot, unsigned long flags, unsigned long fd) {
#ifdef __x86_64
    register volatile int r10 __asm__ ("r10") = flags; register volatile int r8 __asm__ ("r8") = fd; unsigned int *r;
    __asm__ volatile ("syscall" : "=a" (r) : "a" (__NR_mmap), "D" (addr), "S" (length), "d" (prot), "r" (r10), "r" (r8) : "cc", "memory", "r11", "rcx");
    return r;
#else
    unsigned int args[2] = { (unsigned int)addr, 0 };
    unsigned int *r;

    __asm__ __volatile__("push %%ebp\n"
                        "movl 4(%%ebx), %%ebp\n"
                        "movl 0(%%ebx), %%ebx\n"
                        "int $0x80\n"
                        "pop %%ebp\n"
                        : "=a"(r)
                        : "a"(__NR_mmap2), "b"(&args),
                            "c"(length), "d"(prot), "S"(flags), "D"(fd));
    
    return r;
#endif
}

void _start() {
    int fbfd = sys_open("/dev/fb0", O_RDWR, 0);
    unsigned int *buffer = (unsigned int *)sys_mmap(0, FRAMEBUFFER_LENGTH, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd);

    unsigned int x = WIDTH / 2;
    unsigned int y = HEIGHT / 2;

    unsigned int index = x + y * WIDTH;

    buffer[index] = 16777215; // white
}
