#include "render.h"
#include <stdint.h>

typedef struct {
    unsigned char mem[4096];
    uint8_t V[16];
    uint8_t stack[12];
    uint8_t timer[2];
    uint8_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
} State;

int main() {
    RenderCtx ctx;
    State state;

    if (!init_render(&ctx))
        return 1;

    DRAW_PIXEL(&state, 4, 4);

    while (ctx.running) {
        draw(&ctx, state.display);
        poll_events(&ctx);
        WAIT(100);
    }

    quit_render(&ctx);

    return 0;
}
