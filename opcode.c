#include "render.h"
#include "types.h"

#define OP(n) void OP_##n(State *state)
#define FONT_START_ADDRESS 0x50

uint8_t generate_random_number() { return rand() % 256; }

OP(00E0) { CLEAR_DISPLAY(state); }
OP(00EE) {
    --state->sp;
    state->pc = state->stack[state->sp];
}
OP(1NNN) {
    uint16_t address = state->opcode & 0x0FFF;
    state->pc = address;
}
OP(2NNN) {
    uint16_t address = state->opcode & 0x0FFF;
    state->stack[state->sp++] = state->pc;
    state->pc = address;
}
OP(3XNN) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t byte = state->opcode & 0x00FF;

    if (state->V[x] == byte) {
        state->pc += 2;
    }
}
OP(4XNN) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t byte = state->opcode & 0x00FF;

    if (state->V[x] != byte) {
        state->pc += 2;
    }
}
OP(5XY0) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t y = (state->opcode & 0x00F0) >> 4;

    if (state->V[x] == state->V[y]) {
        state->pc += 2;
    }
}
OP(6XNN) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t byte = state->opcode & 0x00FF;

    state->V[x] = byte;
}
OP(7XNN) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t byte = state->opcode & 0x00FF;

    state->V[x] += byte;
}
OP(8XY1) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t y = (state->opcode & 0x00F0) >> 4;

    state->V[x] |= state->V[y];
}
OP(8XY2) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t y = (state->opcode & 0x00F0) >> 4;

    state->V[x] &= state->V[y];
}
OP(8XY3) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t y = (state->opcode & 0x00F0) >> 4;

    state->V[x] ^= state->V[y];
}
OP(8XY4) {
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
OP(8XY5) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t y = (state->opcode & 0x00F0) >> 4;

    if (state->V[x] > state->V[y]) {
        state->V[0xF] = 1;
    } else {
        state->V[0xF] = 0;
    }
    state->V[x] -= state->V[y];
}
OP(8XY6) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    state->V[0xF] = x & 0x1;
    state->V[x] >>= 1;
}
OP(8XY7) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t y = (state->opcode & 0x00F0) >> 4;

    if (state->V[y] > state->V[x]) {
        state->V[0xF] = 1;
    } else {
        state->V[0xF] = 0;
    }

    state->V[x] = state->V[y] - state->V[x];
}
OP(8XYE) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    state->V[0xF] = (state->V[x] & 0x80) >> 7;
    state->V[x] <<= 1;
}
OP(9XY0) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t y = (state->opcode & 0x00F0) >> 4;

    if (state->V[x] != state->V[y]) {
        state->pc += 2;
    }
}
OP(ANNN) {
    uint16_t address = state->opcode & 0x0FFF;
    state->I = address;
}
OP(BNNN) {
    uint16_t address = state->opcode & 0x0FFF;
    state->pc = state->V[0] + address;
}
OP(CXNN) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t byte = state->opcode & 0x00FF;
    state->V[x] = generate_random_number() & byte;
}
OP(DXYN) {
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
OP(EX9E) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t key = state->V[x];

    if (state->keypad[key]) {
        state->pc += 2;
    }
}
OP(EXA1) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    uint8_t key = state->V[x];

    if (!state->keypad[key]) {
        state->pc += 2;
    }
}
OP(FX07) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    state->V[x] = state->delay_timer;
}
OP(FX0A) {
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
OP(FX15) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    state->delay_timer = state->V[x];
}
OP(FX18) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    state->sound_timer = state->V[x];
}
OP(FX1E) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    state->I += state->V[x];
}
OP(FX29) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    state->I = FONT_START_ADDRESS + (5 * state->V[x]);
}
OP(FX33) {
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
OP(FX55) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    for (int i = 0; i <= x; ++i) {
        state->mem[state->I + i] = state->V[i];
    }
}
OP(FX65) {
    uint8_t x = (state->opcode & 0x0F00) >> 8;
    for (int i = 0; i <= x; ++i) {
        state->V[i] = state->mem[state->I + i];
    }
}
