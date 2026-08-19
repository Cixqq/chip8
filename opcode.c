#include "globals.h"
#include "render.h"

#define OPCODE(n) void OPCODE_##n(State *state)

OPCODE(00E0) { CLEAR_DISPLAY(state); }
OPCODE(00EE) {
    --state->sp;
    state->pc = state->stack[state->sp];
}
OPCODE(1NNN) {
    uint16_t address = state->opcode & 0x0FFF;
    state->pc = address;
}
OPCODE(2NNN) {
    uint16_t address = state->opcode & 0x0FFF;
    state->stack[state->sp++] = state->pc;
    state->pc = address;
}
OPCODE(3XNN) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t byte = state->opcode & 0x00FF;

    if (state->V[x] == byte) {
        state->pc += 2;
    }
}
OPCODE(4XNN) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t byte = state->opcode & 0x00FF;

    if (state->V[x] != byte) {
        state->pc += 2;
    }
}
OPCODE(5XY0) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t y = (state->opcode & 0x00F0) >> 4;

    if (state->V[x] == state->V[y]) {
        state->pc += 2;
    }
}
OPCODE(6XNN) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t byte = state->opcode & 0x00FF;

    state->V[x] = byte;
}
OPCODE(7XNN) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t byte = state->opcode & 0x00FF;

    state->V[x] += byte;
}
OPCODE(8XY1) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t y = (state->opcode & 0x00F0) >> 4;

    state->V[x] |= state->V[y];
}
OPCODE(8XY2) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t y = (state->opcode & 0x00F0) >> 4;

    state->V[x] &= state->V[y];
}
OPCODE(8XY3) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t y = (state->opcode & 0x00F0) >> 4;

    state->V[x] ^= state->V[y];
}
OPCODE(8XY4) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t y = (state->opcode & 0x00F0) >> 4;

    uint16_t sum = state->V[x] + state->V[y];
    if (sum > 255) {
        state->V[0xF] = 1;
    } else {
        state->V[0xF] = 0;
    }
    state->V[x] = sum & 0xFF;
}
OPCODE(8XY5) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t y = (state->opcode & 0x00F0) >> 4;

    if (state->V[x] > state->V[y]) {
        state->V[0xF] = 1;
    } else {
        state->V[0xF] = 0;
    }
    state->V[x] -= state->V[y];
}
OPCODE(8XY6) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    state->V[0xF] = x & 0x1;
    state->V[x] >>= 1;
}
OPCODE(8XY7) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t y = (state->opcode & 0x00F0) >> 4;

    if (state->V[y] > state->V[x]) {
        state->V[0xF] = 1;
    } else {
        state->V[0xF] = 0;
    }

    state->V[x] = state->V[y] - state->V[x];
}
OPCODE(8XYE) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    state->V[0xF] = (state->V[x] & 0x80) >> 7;
    state->V[x] <<= 1;
}
OPCODE(9XY0) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t y = (state->opcode & 0x00F0) >> 4;

    if (state->V[x] != state->V[y]) {
        state->pc += 2;
    }
}
OPCODE(ANNN) {
    uint16_t address = state->opcode & 0x0FFF;
    state->I = address;
}
OPCODE(BNNN) {
    uint16_t address = state->opcode & 0x0FFF;
    state->pc = state->V[0] + address;
}
OPCODE(CXNN) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t byte = state->opcode & 0x00FF;
    state->V[x] = (rand() % 256) & byte;
}
OPCODE(DXYN) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t y = (state->opcode & 0x00F0) >> 4;
    uint8_t height = state->opcode & 0x000F;

    x = state->V[x] % COLS;
    y = state->V[y] % ROWS;

    state->V[0xF] = 0;

    for (int i = 0; i < height; ++i) {
        uint8_t y_pos = (y + i) % ROWS;
        uint8_t sprite_byte = state->mem[state->I + i];

        for (int j = 0; j < 8; ++j) {
            uint8_t x_pos = (x + j) % COLS;
            uint8_t sprite_pixel = (sprite_byte & (0x80 >> j));

            if (sprite_pixel != 0) {
                int index = (y_pos * COLS) + x_pos;

                if (state->display[index] == true) {
                    state->V[0xF] = 1;
                }

                DRAW_PIXEL(state, x_pos, y_pos);
            }
        }
    }
}
OPCODE(EX9E) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t key = state->V[x];

    if (state->keypad[key]) {
        state->pc += 2;
    }
}
OPCODE(EXA1) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t key = state->V[x];

    if (!state->keypad[key]) {
        state->pc += 2;
    }
}
OPCODE(FX07) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    state->V[x] = state->delay_timer;
}
OPCODE(FX0A) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    bool found = false;

    for (int i = 0; i < 16; ++i) {
        if (state->keypad[i]) {
            state->V[x] = i;
            found = true;
            break;
        }
    }

    if (!found)
        state->pc -= 2;
}
OPCODE(FX15) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    state->delay_timer = state->V[x];
}
OPCODE(FX18) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    state->sound_timer = state->V[x];
}
OPCODE(FX1E) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    state->I += state->V[x];
}
OPCODE(FX29) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    state->I = FONT_START_ADDRESS + (5 * state->V[x]);
}
OPCODE(FX33) {
    uint8_t x = (state->opcode & 0x0F00u) >> 8u;
    uint8_t value = state->V[x];

    // Ones-place
    state->mem[state->I + 2] = value % 10;
    value /= 10;

    // Tens-place
    state->mem[state->I + 1] = value % 10;
    value /= 10;

    // Hundreds-place
    state->mem[state->I] = value % 10;
}
OPCODE(FX55) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    for (int i = 0; i <= x; ++i) {
        state->mem[state->I + i] = state->V[i];
    }
}
OPCODE(FX65) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    for (int i = 0; i <= x; ++i) {
        state->V[i] = state->mem[state->I + i];
    }
}
