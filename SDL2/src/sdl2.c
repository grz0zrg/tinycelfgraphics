#include "SDL2/SDL.h"

// defined in build.sh
//#define WIDTH 1920
//#define HEIGHT 1080

void _start() {
#ifdef __x86_64
    // 64 bits fix due to XauReadAuth (libxau.so) segfault when compiled with SSE4 / AVX2
    __asm__("sub $8, %rsp\n");
#endif

    //SDL_Init(SDL_INIT_VIDEO); // probably uneeded for pixels stuff, try to enable it if it crash !
    SDL_Window * window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Surface * window_surface = SDL_GetWindowSurface(window);

    int x = WIDTH / 2;
    int y = HEIGHT / 2;

    for (;;) {
        // put white pixel
        Uint8 *p = (Uint8 *)window_surface->pixels + 300 * window_surface->pitch + 400 * 4;
        *(Uint32 *)p = 16777215;

        SDL_UpdateWindowSurface(window);
    }
}
