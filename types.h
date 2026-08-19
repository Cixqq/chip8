#ifndef _TYPES_H
#define _TYPES_H
#include "render.h"
#include <stdint.h>

typedef struct {
    uint8_t mem[4096];
    uint8_t V[16];
    uint8_t stack[12];
    uint8_t sp;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint16_t opcode;
    uint16_t I;
    uint16_t pc;
    bool display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
    bool keypad[16];
} State;
#endif // _TYPES_H
