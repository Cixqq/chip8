#include <SDL2/SDL.h>
#include <stdbool.h>

#define COLS 64
#define ROWS 32

#define CELL_SIZE 8
#define BORDER_THICKNESS 1

#define DISPLAY_WIDTH COLS *CELL_SIZE
#define DISPLAY_HEIGHT ROWS *CELL_SIZE

#define WAIT(ms) SDL_Delay(ms)

#define DRAW_PIXEL(s, x, y)                                                    \
    do {                                                                       \
        if ((x) >= 0 && (x) < COLS && (y) >= 0 && (y) < ROWS) {                \
            (s)->display[(y) * COLS + (x)] = true;                             \
        }                                                                      \
    } while (0)

#define CLEAR_PIXEL(s, x, y)                                                   \
    do {                                                                       \
        if ((x) >= 0 && (x) < COLS && (y) >= 0 && (y) < ROWS) {                \
            (s)->display[(y) * COLS + (x)] = false;                            \
        }                                                                      \
    } while (0)

typedef struct {
    SDL_Window *win;
    SDL_Surface *winSurface;
    SDL_Event ev;
    bool running;
    Uint32 colors[3];
} RenderCtx;

enum Colors {
    BLACK,
    WHITE,
    GRAY,
};

bool init_render(RenderCtx *ctx);
void quit_render(RenderCtx *ctx);
void draw(RenderCtx *ctx, uint8_t *display);
void poll_events(RenderCtx *ctx);
