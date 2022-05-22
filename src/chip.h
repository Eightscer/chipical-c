#ifndef CHIP_H
#define CHIP_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

extern float c8_delay_freq;
extern uint32_t c8_cps;

extern uint16_t c8_I, c8_PC;
extern uint8_t c8_VX[16];
extern uint8_t c8_SP, c8_DELAY, c8_SOUND;
extern uint16_t c8_STACK[16];
extern uint8_t c8_keypad[16];
extern uint8_t c8_font[16*5];
extern uint16_t c8_font_start;

extern uint16_t c8_OP;
extern uint8_t c8_X;
extern uint8_t c8_Y;

typedef void (*op)();
extern op c8_opcodes_lut[0x10];
extern op c8_opcode0_lut[0x10];
extern op c8_opcode8_lut[0x10];
extern op c8_opcodeE_lut[0x10];
extern op c8_opcodeF_lut[0x100];

extern uint8_t* c8_RAM;
extern uint32_t* c8_VRAM;

extern uint8_t c8_scrw;
extern uint8_t c8_scrh;

extern uint32_t c8_0_color;
extern uint32_t c8_1_color;

int c8_init();
void c8_op_init();
void c8_load_font(char* filename);
int c8_load_file(char* filename);

void c8_cycle();
void c8_fetch();
void c8_decode();
void c8_execute();

void c8_deinit();

void c8_INVOP();

void c8_op0();
void c8_00E0();
void c8_00EE();
void c8_0NNN();

void c8_1NNN();
void c8_2NNN();
void c8_3XKK();
void c8_4XKK();
void c8_5XY0();
void c8_6XKK();
void c8_7XKK();

void c8_op8();
void c8_8XY0();
void c8_8XY1();
void c8_8XY2();
void c8_8XY3();
void c8_8XY4();
void c8_8XY5();
void c8_8XY6();
void c8_8XY7();
void c8_8XYE();

void c8_9XY0();
void c8_ANNN();
void c8_BNNN();
void c8_CXKK();
void c8_DXYN();

void c8_opE();
void c8_EX9E();
void c8_EXA1();

void c8_opF();
void c8_FX07();
void c8_FX0A();
void c8_FX15();
void c8_FX18();
void c8_FX1E();
void c8_FX29();
void c8_FX33();
void c8_FX55();
void c8_FX65();

/*
void sc48_00CN();
void sc48_00FB();
void sc48_00FC();
void sc48_00FD();
void sc48_00FE();
void sc48_00FF();
void sc48_DXY0();
void sc48_FX30();
void sc48_FX75();
void sc48_FX85();
*/

#endif
