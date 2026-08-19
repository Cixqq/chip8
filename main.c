#include "opcode.c"
#include "render.h"
#include "types.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROM_START_ADDRESS 0x200
#define FONT_START_ADDRESS 0x50
#define FONTSET_SIZE 80

#define OPCODE(n) OP_##n(state)

bool load_rom(State *state, const char *path) {
    FILE *fp = fopen(path, "rb");
    int ch;

    if (fp == NULL) {
        return false;
    }
    for (int i = 0; (ch = fgetc(fp)) != EOF; ++i) {
        state->mem[ROM_START_ADDRESS + i] = ch;
    }

    fclose(fp);
    state->pc = ROM_START_ADDRESS;
    return true;
}

void load_fontset(State *state) {
    const uint8_t fontset[FONTSET_SIZE] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    for (int i = 0; i < FONTSET_SIZE; ++i) {
        state->mem[FONT_START_ADDRESS + i] = fontset[i];
    }
}

void fetch(State *state) {
    state->opcode = (state->mem[state->pc] << 8) | state->mem[state->pc + 1];
    state->pc += 2;
}

void decode_and_execute(State *state) {
    switch (state->opcode & 0xF000) {
    case 0x0000:
        switch (state->opcode & 0x000F) {
        case 0x0000:
            OPCODE(00E0);
            break;
        case 0x000E:
            OPCODE(00EE);
            break;
        }
        break;
    case 0x1000:
        OPCODE(1NNN);
        break;
    case 0x2000:
        OPCODE(2NNN);
        break;
    case 0x3000:
        OPCODE(3XNN);
        break;
    case 0x4000:
        OPCODE(4XNN);
        break;
    case 0x5000:
        OPCODE(5XY0);
        break;
    case 0x6000:
        OPCODE(6XNN);
        break;
    case 0x7000:
        OPCODE(7XNN);
        break;
    case 0x8000:
        switch (state->opcode & 0x000F) {
        case 0x0001:
            OPCODE(8XY1);
            break;
        case 0x0002:
            OPCODE(8XY2);
            break;
        case 0x0003:
            OPCODE(8XY3);
            break;
        case 0x0004:
            OPCODE(8XY4);
            break;
        case 0x0005:
            OPCODE(8XY5);
            break;
        case 0x0006:
            OPCODE(8XY6);
            break;
        case 0x0007:
            OPCODE(8XY7);
            break;
        case 0x000E:
            OPCODE(8XYE);
            break;
        }
        break;
    case 0x9000:
        OPCODE(9XY0);
        break;

    case 0xA000:
        OPCODE(ANNN);
        break;
    case 0xB000:
        OPCODE(BNNN);
        break;
    case 0xC000:
        OPCODE(CXNN);
        break;
    case 0xD000:
        OPCODE(DXYN);
        break;
    case 0xE000:
        switch (state->opcode & 0x000F) {
        case 0x000E:
            OPCODE(EX9E);
            break;
        case 0x0001:
            OPCODE(EXA1);
            break;
        }
        break;
    case 0xF000:
        switch (state->opcode & 0x00FF) {
        case 0x0007:
            OPCODE(FX07);
            break;
        case 0x000A:
            OPCODE(FX0A);
            break;
        case 0x0015:
            OPCODE(FX15);
            break;
        case 0x0018:
            OPCODE(FX18);
            break;
        case 0x001E:
            OPCODE(FX1E);
            break;
        case 0x0029:
            OPCODE(FX29);
            break;
        case 0x0033:
            OPCODE(FX33);
            break;
        case 0x0055:
            OPCODE(FX55);
            break;
        case 0x0065:
            OPCODE(FX65);
            break;
        }
    }
}

int main(int argc, char **argv) {
    srand(time(0));

    RenderCtx ctx;
    State state;

    if (argc < 2) {
        printf("Invalid usage: %s <ROM path>.\n", argv[0]);
        return 1;
    }

    if (!load_rom(&state, argv[1])) {
        printf("Couldn't find the ROM.\n");
        return 1;
    }

    load_fontset(&state);

    if (!init_render(&ctx))
        return 1;

    while (ctx.running) {
        fetch(&state);
        decode_and_execute(&state);

        draw(&ctx, state.display);
        poll_events(&ctx);
        WAIT(16);
    }

    quit_render(&ctx);

    return 0;
}
