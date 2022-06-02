#ifndef CHIP_H
#define CHIP_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef enum {
	UNINITIALIZED,
	RUNNING,
	PAUSED,
	EXCEPTION,
	ERROR
} c8_status;

typedef enum {
	ALL_OK,
	STACK_POINTER_OOB,
	PROGRAM_COUNTER_OOB,
	INDEX_OUT_OF_BOUNDS,
	UNKNOWN_OPCODE
} c8_exception;

typedef struct chip8_state chip8_state;
typedef struct chip8_system chip8_system;

typedef void (*op)(chip8_system*);

typedef struct chip8_state {
	uint16_t i, pc;//, o;
	uint8_t reg[16];
	uint8_t keypad[16];
	uint16_t stack[16];
	uint8_t sp, delay, sound;
	//uint8_t ox, oy;
	uint8_t* ram;
	uint32_t* vram;
} chip8_state;

typedef struct chip8_system {
	c8_status st;
	c8_exception ex;
	op instr;
	uint16_t o;
	uint8_t ox, oy;
	float delay_freq;
	uint32_t cycles_per_sec;
	uint16_t prgm_start;
	uint16_t prgm_size;
	uint16_t font_start;
	uint8_t font[16*5];
	uint8_t scrw, scrh;
	uint32_t color0, color1;
	op opcodes_LUT[0x10];
	op opcode0_LUT[0x10];
	op opcode8_LUT[0x10];
	op opcodeE_LUT[0x10];
	op opcodeF_LUT[0x100];
	chip8_state state;
} chip8_system;

// Initialization
int c8_init(chip8_system* c8, char* rom, char* font);
chip8_system* c8_create_system();
void c8_init_op_lut(chip8_system* c8);
int c8_load_font(chip8_system* c8, char* filename);
int c8_load_rom(chip8_system* c8, char* filename);

// Running
void c8_cycle(chip8_system* c8);
void c8_fetch(chip8_system* c8);
void c8_update_timer(chip8_system* c8);

// Deinitialization
void c8_deinit(chip8_system* c8);

// Opcodes
void c8_INVOP(chip8_system* c8);

void c8_00E0(chip8_system* c8);
void c8_00EE(chip8_system* c8);
void c8_0NNN(chip8_system* c8);

void c8_1NNN(chip8_system* c8);
void c8_2NNN(chip8_system* c8);
void c8_3XKK(chip8_system* c8);
void c8_4XKK(chip8_system* c8);
void c8_5XY0(chip8_system* c8);
void c8_6XKK(chip8_system* c8);
void c8_7XKK(chip8_system* c8);

void c8_8XY0(chip8_system* c8);
void c8_8XY1(chip8_system* c8);
void c8_8XY2(chip8_system* c8);
void c8_8XY3(chip8_system* c8);
void c8_8XY4(chip8_system* c8);
void c8_8XY5(chip8_system* c8);
void c8_8XY6(chip8_system* c8);
void c8_8XY7(chip8_system* c8);
void c8_8XYE(chip8_system* c8);

void c8_9XY0(chip8_system* c8);
void c8_ANNN(chip8_system* c8);
void c8_BNNN(chip8_system* c8);
void c8_CXKK(chip8_system* c8);
void c8_DXYN(chip8_system* c8);

void c8_EX9E(chip8_system* c8);
void c8_EXA1(chip8_system* c8);

void c8_FX07(chip8_system* c8);
void c8_FX0A(chip8_system* c8);
void c8_FX15(chip8_system* c8);
void c8_FX18(chip8_system* c8);
void c8_FX1E(chip8_system* c8);
void c8_FX29(chip8_system* c8);
void c8_FX33(chip8_system* c8);
void c8_FX55(chip8_system* c8);
void c8_FX65(chip8_system* c8);

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

//extern c8_exception c8_ex;
//extern op c8_INSTR;

//extern float c8_delay_freq;
//extern uint32_t c8_cps;

/*
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
*/

#endif
