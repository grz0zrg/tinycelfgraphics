#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#ifdef __x86_64
    #include <asm/unistd_64.h>
#else
    #include <asm/unistd_32.h>
#endif

// define a RGB PPM (Portable PixMap) file
#define STRINGIZE(A) #A

#define _PPM_HEADER(W,H) "P6 "W" "H" 255 "
#define PPM_HEADER(W,H) _PPM_HEADER(STRINGIZE(W),STRINGIZE(H))

struct ppm {
    char header[sizeof(PPM_HEADER(WIDTH, HEIGHT)) - 1];
    char buffer[WIDTH * HEIGHT * 3];
} image = { PPM_HEADER(WIDTH, HEIGHT), 0 };
//

inline static size_t sys_write(int fd, const void *buf, size_t size) {
    size_t r;
#ifdef __x86_64
    __asm__ volatile("syscall"
        : "=a" (r)
        : "0"(__NR_write), "D"(fd), "S"(buf), "d"(size)
        : "rcx", "r11", "memory");
#else
    __asm__ volatile("int $0x80"
        : "=a" (r)
        : "0"(__NR_write), "b"(fd), "c"(buf), "d"(size)
        : "memory");
#endif
    return r;
}

inline static int sys_open(const char *filepath, int flags, int mode) {
    long r;
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

void _start() {
    // create a .ppm output file (Note : standard version)
    //int fd = sys_open(".ppm", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    char *buffer = &image.buffer[0];

    // plot a white pixel at the center of the image
    int x = WIDTH / 2;
    int y = HEIGHT / 2;

    unsigned int index = (x + y * WIDTH) * 3;

    buffer[index + 0] = 255;
    buffer[index + 1] = 255;
    buffer[index + 2] = 255;
    //

    // write buffer to the opened file (Note : standard version)
    //sys_write(fd, &image, sizeof(struct ppm));

    // shortcut: write to stdout & let the shell script write the file
    sys_write(1, &image, sizeof(struct ppm));
}
