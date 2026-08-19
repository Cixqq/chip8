#ifndef _GLOBALS_H
#define _GLOBALS_H
#include <stdbool.h>
#include <stdint.h>

#define ROM_START_ADDRESS 0x200
#define FONT_START_ADDRESS 0x50
#define FONTSET_SIZE 80

#define COLS 64
#define ROWS 32

#define CELL_SIZE 8
#define BORDER_THICKNESS 0

#define DISPLAY_WIDTH COLS *CELL_SIZE
#define DISPLAY_HEIGHT ROWS *CELL_SIZE

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
#endif // _GLOBALS_H
