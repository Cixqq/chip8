#include "render.h"
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>

bool init_render(RenderCtx *ctx) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *win = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED, DISPLAY_WIDTH,
                                       DISPLAY_HEIGHT, SDL_WINDOW_SHOWN);
    if (!win)
        return false;

    SDL_Surface *win_surface = SDL_GetWindowSurface(win);

    ctx->win = win;
    ctx->win_surface = win_surface;
    ctx->running = true;

    Uint32 black = SDL_MapRGB(ctx->win_surface->format, 0, 0, 0);
    Uint32 gray = SDL_MapRGB(ctx->win_surface->format, 90, 90, 90);
    Uint32 white = SDL_MapRGB(win_surface->format, 255, 255, 255);

    SDL_FillRect(ctx->win_surface, NULL, gray);
    SDL_UpdateWindowSurface(ctx->win);

    ctx->colors[BLACK] = black;
    ctx->colors[WHITE] = white;
    ctx->colors[GRAY] = gray;

    return true;
}

void draw(RenderCtx *ctx, bool *display) {
    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            SDL_Rect r = {x * CELL_SIZE, y * CELL_SIZE,
                          CELL_SIZE - BORDER_THICKNESS,
                          CELL_SIZE - BORDER_THICKNESS};
            SDL_FillRect(ctx->win_surface, &r,
                         (display[y * COLS + x] == true) ? ctx->colors[WHITE]
                                                         : ctx->colors[BLACK]);
        }
    }
    SDL_UpdateWindowSurface(ctx->win);
}

void poll_events(State *state, RenderCtx *ctx) {
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

    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    const SDL_Scancode scancodes[16] = {
        SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
        SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
        SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
        SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V};

    for (int i = 0; i < 16; i++) {
        state->keypad[i] = keys[scancodes[i]] ? 1 : 0;
    }
}

void quit_render(RenderCtx *ctx) {
    SDL_DestroyWindow(ctx->win);
    ctx->win = NULL;
    ctx->win_surface = NULL;
    SDL_Quit();
}
