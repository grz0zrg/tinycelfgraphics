#include <clutter/clutter.h>

// defined in build.sh
//#define WIDTH 1920
//#define HEIGHT 1080

void _start() {
#ifdef __x86_64
    // 64 bits fix due to XauReadAuth (libxau.so) segfault when compiled with SSE4 / AVX2
    __asm__("sub $8, %rsp\n");
#endif

    clutter_init(0, 0);

    ClutterActor *stage = clutter_stage_new();
    ClutterEffect *shader = clutter_shader_effect_new(CLUTTER_FRAGMENT_SHADER);

    clutter_shader_effect_set_shader_source((ClutterShaderEffect *)shader, "#define main cogl_color_out=vec4(1.)");
    clutter_actor_add_effect(stage, shader);

    clutter_actor_show(stage);

    clutter_main();
}
