#include "SDL/SDL.h"

// defined in build.sh
//#define WIDTH 1920
//#define HEIGHT 1080

void _start() {
#ifdef __x86_64
    // 64 bits fix due to XauReadAuth (libxau.so) segfault when compiled with SSE4 / AVX2
    __asm__("sub $8, %rsp\n");
#endif

    //SDL_Init(SDL_INIT_VIDEO); // probably uneeded for pixels stuff, try to enable it if it crash !
    SDL_Surface *screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_SWSURFACE);

    int x = WIDTH / 2;
    int y = HEIGHT / 2;

    for (;;) {
        // put white pixel
        Uint8 *p = (Uint8 *)screen->pixels + 300 * screen->pitch + 400 * 4;
        *(Uint32 *)p = 16777215;

        SDL_Flip(screen);
    }
}
