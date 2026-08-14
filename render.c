#include "render.h"

bool init_render(RenderCtx *ctx) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *win = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED, DISPLAY_WIDTH,
                                       DISPLAY_HEIGHT, SDL_WINDOW_SHOWN);
    if (!win)
        return false;

    SDL_Surface *winSurface = SDL_GetWindowSurface(win);

    ctx->win = win;
    ctx->winSurface = winSurface;
    ctx->running = true;

    Uint32 black = SDL_MapRGB(ctx->winSurface->format, 0, 0, 0);
    Uint32 gray = SDL_MapRGB(ctx->winSurface->format, 90, 90, 90);
    Uint32 white = SDL_MapRGB(winSurface->format, 255, 255, 255);

    SDL_FillRect(ctx->winSurface, NULL, gray);
    SDL_UpdateWindowSurface(ctx->win);

    ctx->colors[BLACK] = black;
    ctx->colors[WHITE] = white;
    ctx->colors[GRAY] = gray;

    return true;
}

void draw(RenderCtx *ctx, uint8_t *display) {
    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            SDL_Rect r = {x * CELL_SIZE, y * CELL_SIZE,
                          CELL_SIZE - BORDER_THICKNESS,
                          CELL_SIZE - BORDER_THICKNESS};
            SDL_FillRect(ctx->winSurface, &r,
                         (display[y * COLS + x] == true) ? ctx->colors[WHITE]
                                                         : ctx->colors[BLACK]);
        }
    }
    SDL_UpdateWindowSurface(ctx->win);
}

void poll_events(RenderCtx *ctx) {
    while (SDL_PollEvent(&ctx->ev) != false) {
        switch (ctx->ev.type) {
        case SDL_QUIT:
            ctx->running = false;
            break;
        case SDL_KEYDOWN:
            switch (ctx->ev.key.keysym.sym) {
            case SDLK_ESCAPE:
                ctx->running = false;
                break;
            }
        }
    }
}

void quit_render(RenderCtx *ctx) {
    SDL_DestroyWindow(ctx->win);
    ctx->win = NULL;
    ctx->winSurface = NULL;
    SDL_Quit();
}
