#include "globals.h"
#include "opcode.c"
#include "render.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CALL_OPCODE(n) OPCODE_##n(state)

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
            CALL_OPCODE(00E0);
            break;
        case 0x000E:
            CALL_OPCODE(00EE);
            break;
        }
        break;
    case 0x1000:
        CALL_OPCODE(1NNN);
        break;
    case 0x2000:
        CALL_OPCODE(2NNN);
        break;
    case 0x3000:
        CALL_OPCODE(3XNN);
        break;
    case 0x4000:
        CALL_OPCODE(4XNN);
        break;
    case 0x5000:
        CALL_OPCODE(5XY0);
        break;
    case 0x6000:
        CALL_OPCODE(6XNN);
        break;
    case 0x7000:
        CALL_OPCODE(7XNN);
        break;
    case 0x8000:
        switch (state->opcode & 0x000F) {
        case 0x0001:
            CALL_OPCODE(8XY1);
            break;
        case 0x0002:
            CALL_OPCODE(8XY2);
            break;
        case 0x0003:
            CALL_OPCODE(8XY3);
            break;
        case 0x0004:
            CALL_OPCODE(8XY4);
            break;
        case 0x0005:
            CALL_OPCODE(8XY5);
            break;
        case 0x0006:
            CALL_OPCODE(8XY6);
            break;
        case 0x0007:
            CALL_OPCODE(8XY7);
            break;
        case 0x000E:
            CALL_OPCODE(8XYE);
            break;
        }
        break;
    case 0x9000:
        CALL_OPCODE(9XY0);
        break;

    case 0xA000:
        CALL_OPCODE(ANNN);
        break;
    case 0xB000:
        CALL_OPCODE(BNNN);
        break;
    case 0xC000:
        CALL_OPCODE(CXNN);
        break;
    case 0xD000:
        CALL_OPCODE(DXYN);
        break;
    case 0xE000:
        switch (state->opcode & 0x000F) {
        case 0x000E:
            CALL_OPCODE(EX9E);
            break;
        case 0x0001:
            CALL_OPCODE(EXA1);
            break;
        }
        break;
    case 0xF000:
        switch (state->opcode & 0x00FF) {
        case 0x0007:
            CALL_OPCODE(FX07);
            break;
        case 0x000A:
            CALL_OPCODE(FX0A);
            break;
        case 0x0015:
            CALL_OPCODE(FX15);
            break;
        case 0x0018:
            CALL_OPCODE(FX18);
            break;
        case 0x001E:
            CALL_OPCODE(FX1E);
            break;
        case 0x0029:
            CALL_OPCODE(FX29);
            break;
        case 0x0033:
            CALL_OPCODE(FX33);
            break;
        case 0x0055:
            CALL_OPCODE(FX55);
            break;
        case 0x0065:
            CALL_OPCODE(FX65);
            break;
        }
    }
}

void cpu_cycle(State *state) {
    fetch(state);
    decode_and_execute(state);
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
        cpu_cycle(&state);
        draw(&ctx, state.display);
        poll_events(&state, &ctx);
        WAIT(16);
    }

    quit_render(&ctx);

    return 0;
}
