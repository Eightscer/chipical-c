#include "chip.h"

/*
uint16_t c8_I, c8_PC;
uint8_t c8_VX[16];
uint8_t c8_SP, c8_DELAY, c8_SOUND;
uint16_t c8_STACK[16];
uint8_t c8_keypad[16];

op c8_INSTR = c8_INVOP;
c8_exception c8_ex = ALL_OK;

float c8_delay_freq = 60;
uint32_t c8_cps = 1000;
*/

uint8_t c8_default_font[16*5] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0,
	0x20, 0x60, 0x20, 0x20, 0x70,
	0xF0, 0x10, 0xF0, 0x80, 0xF0,
	0xF0, 0x10, 0xF0, 0x10, 0xF0,
	0x90, 0x90, 0xF0, 0x10, 0x10,
	0xF0, 0x80, 0xF0, 0x10, 0xF0,
	0xF0, 0x80, 0xF0, 0x90, 0xF0,
	0xF0, 0x10, 0x20, 0x40, 0x40,
	0xF0, 0x90, 0xF0, 0x90, 0xF0,
	0xF0, 0x90, 0xF0, 0x10, 0xF0,
	0xF0, 0x90, 0xF0, 0x90, 0x90,
	0xE0, 0x90, 0xE0, 0x90, 0xE0,
	0xF0, 0x80, 0x80, 0x80, 0xF0,
	0xE0, 0x90, 0x90, 0x90, 0xE0,
	0xF0, 0x80, 0xF0, 0x80, 0xF0,
	0xF0, 0x80, 0xF0, 0x80, 0x80
};

op c8_opcodes_lut[0x10] = {
	c8_0NNN,  c8_1NNN,  c8_2NNN,  c8_3XKK,
	c8_4XKK,  c8_5XY0,  c8_6XKK,  c8_7XKK,
	c8_INVOP, c8_9XY0,  c8_ANNN,  c8_BNNN,
	c8_CXKK,  c8_DXYN,  c8_INVOP, c8_INVOP
};

/*
op c8_opcode0_lut[0x10] = {
	c8_00E0,  c8_0NNN,  c8_0NNN,  c8_0NNN,
	c8_0NNN,  c8_0NNN,  c8_0NNN,  c8_0NNN,
	c8_0NNN,  c8_0NNN,  c8_0NNN,  c8_0NNN,
	c8_0NNN,  c8_0NNN,  c8_00EE,  c8_0NNN
};
*/

op c8_opcode8_lut[0x10] = {
	c8_8XY0,  c8_8XY1,  c8_8XY2,  c8_8XY3,
	c8_8XY4,  c8_8XY5,  c8_8XY6,  c8_8XY7,
	c8_INVOP, c8_INVOP, c8_INVOP, c8_INVOP,
	c8_INVOP, c8_INVOP, c8_8XYE,  c8_INVOP
};

/*
uint16_t c8_font_start = 0x50;

uint16_t c8_OP;
uint8_t c8_X;
uint8_t c8_Y;
uint8_t* c8_RAM;
uint32_t* c8_VRAM;
uint8_t c8_scrw;
uint8_t c8_scrh;
uint32_t c8_0_color;
uint32_t c8_1_color;

op c8_opcodes_lut[0x10] = {c8_INVOP};
op c8_opcode0_lut[0x10] = {c8_0NNN};
op c8_opcode8_lut[0x10] = {c8_INVOP};
op c8_opcodeE_lut[0x10] = {c8_INVOP};
op c8_opcodeF_lut[0x100] = {c8_INVOP};
*/

/*
int c8_init(chip8_system* c8){
	srand(time(NULL));

	c8_PC = 0x0200;	c8_I = 0x0000;
	c8_SP = 0x00; c8_DELAY = 0x00; c8_SOUND = 0x00;
	int i; for(i = 0; i < 16; ++i){
		c8_VX[i] = 0x00;
		c8_STACK[i] = 0x0000;
		c8_keypad[i] = 0x00;
	}

	c8_init_op_lut();

	c8_scrh = 32;
	c8_scrw = 64;

	//c8_0_color = 0x7F00FF00;
	//c8_1_color = 0x00FF7F00;

	c8_0_color = 0x00000000;
	c8_1_color = 0xFFFFFF00;

	// TODO: Allow config file to alter some of these initial values
	
	if(c8_0_color == c8_1_color) ++c8_0_color;

	if(c8_RAM){ free(c8_RAM); }
	if(c8_VRAM){ free(c8_VRAM); }
	c8_RAM = (uint8_t*)calloc(0x1000, sizeof(uint8_t));
	if(!c8_RAM){ printf("RAM not free\n"); return -1; }
	c8_VRAM = (uint32_t*)calloc(c8_scrw*c8_scrh, sizeof(uint32_t));
	if(!c8_VRAM){ printf("VRAM not free\n"); return -1; }

	memcpy(c8_RAM + c8_font_start, c8_font, sizeof(c8_font));

	return 0;
}
*/

int c8_init(chip8_system* c8, char* rom, char* font){
	//printf("c8 init\n");
	if(c8->st != UNINITIALIZED) return 1;
	c8->ex = ALL_OK;
	c8->instr = c8_INVOP;
	c8->o = 0x0000;
	c8->ox = 0;
	c8->oy = 0;
	
	c8->delay_freq = 60.0;
	c8->cycles_per_sec = 1000;
	srand(time(NULL));
	
	c8->prgm_start = 0x200;
	c8->prgm_size = 0x1000;
	//printf("ram\n");
	if(c8->state.ram) free(c8->state.ram);
	c8->state.ram = (uint8_t*)calloc(c8->prgm_size, sizeof(uint8_t));
	c8_load_rom(c8, rom);

	//printf("font\n");
	c8->font_start = 0x050;
	c8_load_font(c8, font);

	c8->scrw = 64;
	c8->scrh = 32;
	//c8->color0 = 0x00000000;
	//c8->color1 = 0xFFFFFF00;
	c8->color0 = 0x00000000;
	//c8->color1 = 0xFF7F0000;
	c8->color1 = 0x7F00FF00;
	//printf("vram");
	if(c8->state.vram) free(c8->state.vram);
	c8->state.vram = (uint32_t*)calloc(c8->scrh*c8->scrw, sizeof(uint32_t));

	//printf("op lut init\n");
	c8_init_op_lut(c8);

	int n;
	c8->state.i = 0x0000;
	c8->state.pc = c8->prgm_start;
	c8->state.sp = 0x00;
	c8->state.delay = 0x00;
	c8->state.sound = 0x00;
	for(n = 0; n < 0x10; ++n){
		c8->state.reg[n] = 0x00;
		c8->state.keypad[n] = 0x00;
		c8->state.stack[n] = 0x0000;
	}

	c8->st = RUNNING;
	return 0;
}


chip8_system* c8_create_system(){
	//printf("create system\n");
	chip8_system* c8 = (chip8_system*)malloc(sizeof(chip8_system));
	c8->st = UNINITIALIZED;
	return c8;
}

int c8_load_font(chip8_system* c8, char* filename){
	if(filename != NULL){
		FILE* fp = fopen(filename, "r");
		// Do stuff
		fclose(fp);
	}
	memcpy(c8->state.ram + c8->font_start,
		c8_default_font, sizeof(uint8_t)*16*5);
	return 0;
}

int c8_load_rom(chip8_system* c8, char* filename){
	FILE* fp = fopen(filename, "r");
	if(!fp){ printf("Failed to open file %s\n", filename); return 1; }
	fseek(fp, 0, SEEK_SET);
	fread(c8->state.ram + c8->prgm_start,
		sizeof(uint8_t), c8->prgm_size - c8->prgm_start, fp);
	fclose(fp);
	return 0;
}

/*
void c8_init_op_lut(chip8_system* c8){
	int i;

	c8->opcodes_LUT[0x0] = c8_0NNN;
	c8->opcodes_LUT[0x1] = c8_1NNN;
	c8->opcodes_LUT[0x2] = c8_2NNN;
	c8->opcodes_LUT[0x3] = c8_3XKK;
	c8->opcodes_LUT[0x4] = c8_4XKK;
	c8->opcodes_LUT[0x5] = c8_5XY0;
	c8->opcodes_LUT[0x6] = c8_6XKK;
	c8->opcodes_LUT[0x7] = c8_7XKK;
	c8->opcodes_LUT[0x8] = c8_INVOP;
	c8->opcodes_LUT[0x9] = c8_9XY0;
	c8->opcodes_LUT[0xA] = c8_ANNN;
	c8->opcodes_LUT[0xB] = c8_BNNN;
	c8->opcodes_LUT[0xC] = c8_CXKK;
	c8->opcodes_LUT[0xD] = c8_DXYN;
	c8->opcodes_LUT[0xE] = c8_INVOP;
	c8->opcodes_LUT[0xF] = c8_INVOP;
	
	for(i = 0; i < 0x10; ++i){
		c8->opcode0_LUT[i] = c8_0NNN;
		c8->opcode8_LUT[i] = c8_INVOP;
		c8->opcodeE_LUT[i] = c8_INVOP;		
	}
	for(i = 0; i < 0x100; ++i){
		c8->opcodeF_LUT[i] = c8_INVOP;
	}

	c8->opcode0_LUT[0x0] = c8_00E0;
	c8->opcode0_LUT[0xE] = c8_00EE;

	c8->opcode8_LUT[0x0] = c8_8XY0;
	c8->opcode8_LUT[0x1] = c8_8XY1;
	c8->opcode8_LUT[0x2] = c8_8XY2;
	c8->opcode8_LUT[0x3] = c8_8XY3;
	c8->opcode8_LUT[0x4] = c8_8XY4;
	c8->opcode8_LUT[0x5] = c8_8XY5;
	c8->opcode8_LUT[0x6] = c8_8XY6;
	c8->opcode8_LUT[0x7] = c8_8XY7;
	c8->opcode8_LUT[0xE] = c8_8XYE;

	c8->opcodeE_LUT[0x1] = c8_EXA1;
	c8->opcodeE_LUT[0xE] = c8_EX9E;

	c8->opcodeF_LUT[0x07] = c8_FX07;
	c8->opcodeF_LUT[0x0A] = c8_FX0A;
	c8->opcodeF_LUT[0x15] = c8_FX15;
	c8->opcodeF_LUT[0x18] = c8_FX18;
	c8->opcodeF_LUT[0x1E] = c8_FX1E;
	c8->opcodeF_LUT[0x29] = c8_FX29;
	c8->opcodeF_LUT[0x33] = c8_FX33;
	c8->opcodeF_LUT[0x55] = c8_FX55;
	c8->opcodeF_LUT[0x65] = c8_FX65;
}
*/

void c8_init_op_lut(chip8_system* c8){
	int i;
	memcpy(c8->opcodes_LUT, c8_opcodes_lut, sizeof(op)*16);
	memcpy(c8->opcode8_LUT, c8_opcode8_lut, sizeof(op)*16);
	for(i = 0; i < 16; ++i){
		c8->opcode0_LUT[i] = c8_0NNN;
		c8->opcodeE_LUT[i] = c8_INVOP;
	}
	for(i = 0; i < 256; ++i){
		c8->opcodeF_LUT[i] = c8_INVOP;
	}
	
	c8->opcode0_LUT[0x0] = c8_00E0;
	c8->opcode0_LUT[0xE] = c8_00EE;

	c8->opcodeE_LUT[0x1] = c8_EXA1;
	c8->opcodeE_LUT[0xE] = c8_EX9E;

	c8->opcodeF_LUT[0x07] = c8_FX07;
	c8->opcodeF_LUT[0x0A] = c8_FX0A;
	c8->opcodeF_LUT[0x15] = c8_FX15;
	c8->opcodeF_LUT[0x18] = c8_FX18;
	c8->opcodeF_LUT[0x1E] = c8_FX1E;
	c8->opcodeF_LUT[0x29] = c8_FX29;
	c8->opcodeF_LUT[0x33] = c8_FX33;
	c8->opcodeF_LUT[0x55] = c8_FX55;
	c8->opcodeF_LUT[0x65] = c8_FX65;
}

void c8_cycle(chip8_system* c8){
	c8_fetch(c8);
	//printf("%04X:%04X ", c8->state.pc, c8->o);
	if(c8->ex != ALL_OK) return;
	if(c8->instr == c8_INVOP){ c8->ex = UNKNOWN_OPCODE; return; }
	if(c8->state.pc > 0x0FFE){ c8->ex = PROGRAM_COUNTER_OOB; return; }
	c8->state.pc += 2;
	(c8->instr)(c8);
	//if(c8_DELAY) --c8_DELAY;
	//if(c8_SOUND) --c8_SOUND;
}

void c8_fetch(chip8_system* c8){
	c8->o = ((c8->state.ram[c8->state.pc] << 8) | c8->state.ram[c8->state.pc + 1]);
	c8->ox = ((c8->o & 0x0F00) >> 8);
	c8->oy = ((c8->o & 0x00F0) >> 4);
	uint16_t msb = ((c8->o & 0xF000) >> 12);
	switch(msb){
		case 0x0: c8->instr = c8->opcode0_LUT[(c8->o & 0xF)];  break;
		case 0x8: c8->instr = c8->opcode8_LUT[(c8->o & 0xF)];  break;
		case 0xE: c8->instr = c8->opcodeE_LUT[(c8->o & 0xF)];  break;
		case 0xF: c8->instr = c8->opcodeF_LUT[(c8->o & 0xFF)]; break;
		default:  c8->instr = c8->opcodes_LUT[msb];			   break;
	}
}

void c8_update_timer(chip8_system* c8){
	if(c8->state.delay) --c8->state.delay;
	if(c8->state.sound) --c8->state.sound;
}

void c8_deinit(chip8_system* c8){
	if(!c8) return;
	if(c8->state.ram ){ free(c8->state.ram);  c8->state.ram = NULL;  }
	if(c8->state.vram){ free(c8->state.vram); c8->state.vram = NULL; }
	free(c8); c8 = NULL;
}

void c8_INVOP(chip8_system* c8){
	c8->ex = UNKNOWN_OPCODE;
}

void c8_0NNN(chip8_system* c8){
	c8->ex = UNKNOWN_OPCODE;
}

void c8_00E0(chip8_system* c8){
	memset(c8->state.vram, c8->color0, sizeof(uint32_t)*c8->scrh*c8->scrw);
}

void c8_00EE(chip8_system* c8){
	//--c8->state.sp;
	c8->state.sp -= 1;
	if(c8->state.sp > 0xF){ c8->ex = STACK_POINTER_OOB; return; }
	c8->state.pc = c8->state.stack[c8->state.sp];
}

void c8_1NNN(chip8_system* c8){
	c8->state.pc = (c8->o & 0x0FFF);
}

void c8_2NNN(chip8_system* c8){
	if(c8->state.sp > 0xF){ c8->ex = STACK_POINTER_OOB; return; }
	c8->state.stack[c8->state.sp] = c8->state.pc;
	c8->state.sp += 1;
	c8->state.pc = (c8->o & 0x0FFF);
}

void c8_3XKK(chip8_system* c8){
	if(c8->state.reg[c8->ox] == (c8->o & 0xFF))
		c8->state.pc += 2;
}

void c8_4XKK(chip8_system* c8){
	if(c8->state.reg[c8->ox] != (c8->o & 0xFF))
		c8->state.pc += 2;
}

void c8_5XY0(chip8_system* c8){
	if(c8->state.reg[c8->ox] == c8->state.reg[c8->oy])
		c8->state.pc += 2;
}

void c8_6XKK(chip8_system* c8){
	c8->state.reg[c8->ox] = (c8->o & 0xFF);
}

void c8_7XKK(chip8_system* c8){
	c8->state.reg[c8->ox] += (c8->o & 0xFF);
}

void c8_8XY0(chip8_system* c8){
	c8->state.reg[c8->ox] = c8->state.reg[c8->oy];
}

void c8_8XY1(chip8_system* c8){
	c8->state.reg[c8->ox] = c8->state.reg[c8->ox] | c8->state.reg[c8->oy];
}

void c8_8XY2(chip8_system* c8){
	c8->state.reg[c8->ox] = c8->state.reg[c8->ox] & c8->state.reg[c8->oy];
}

void c8_8XY3(chip8_system* c8){
	c8->state.reg[c8->ox] = c8->state.reg[c8->ox] ^ c8->state.reg[c8->oy];	
}

void c8_8XY4(chip8_system* c8){
	uint16_t res = c8->state.reg[c8->ox] + c8->state.reg[c8->oy];
	if(res > 0xFF) c8->state.reg[0xF] = 1;
	c8->state.reg[c8->ox] = (res & 0xFF);
}

void c8_8XY5(chip8_system* c8){
	if(c8->state.reg[c8->ox] > c8->state.reg[c8->oy])
		c8->state.reg[0xF] = 1;
	else
		c8->state.reg[0xF] = 0;
	c8->state.reg[c8->ox] = c8->state.reg[c8->ox] - c8->state.reg[c8->oy];
}

void c8_8XY6(chip8_system* c8){
	if((c8->state.reg[c8->ox] & 0x01))
		c8->state.reg[0xF] = 1;
	else
		c8->state.reg[0xF] = 0;
	c8->state.reg[c8->ox] = (c8->state.reg[c8->ox] >> 1);
}

void c8_8XY7(chip8_system* c8){
	if(c8->state.reg[c8->ox] < c8->state.reg[c8->oy])
		c8->state.reg[0xF] = 1;
	else
		c8->state.reg[0xF] = 0;
	c8->state.reg[c8->ox] = c8->state.reg[c8->ox] - c8->state.reg[c8->oy];
}

void c8_8XYE(chip8_system* c8){
	if((c8->state.reg[c8->ox] & 0x80))
		c8->state.reg[0xF] = 1;
	else
		c8->state.reg[0xF] = 0;
	c8->state.reg[c8->ox] = (c8->state.reg[c8->ox] << 1);
}

void c8_9XY0(chip8_system* c8){
	if(c8->state.reg[c8->ox] != c8->state.reg[c8->oy]) c8->state.pc += 2;
}

void c8_ANNN(chip8_system* c8){
	c8->state.i = (c8->o & 0x0FFF);
}

void c8_BNNN(chip8_system* c8){
	c8->state.pc = (c8->o & 0x0FFF) + c8->state.reg[0];
}

void c8_CXKK(chip8_system* c8){
	c8->state.reg[c8->ox] = (rand() & (c8->o & 0xFF));
}

void c8_DXYN(chip8_system* c8){
	c8->state.reg[0xF] = 0;
	uint8_t r, c;
	for(r = 0; r < (c8->o & 0xF); ++r){
		if(c8->state.i + r > 0xFFF){ c8->ex = INDEX_OUT_OF_BOUNDS; return; }
		uint8_t font_row = c8->state.ram[c8->state.i + r];
		for(c = 0; c < 8; ++c){
			uint8_t font_pixel = (font_row & (0x80 >> c));
			uint32_t px = (((c8->state.reg[c8->oy]+r)%c8->scrh)*c8->scrw) \
				+ ((c8->state.reg[c8->ox]+c)%c8->scrw);
			if(font_pixel){
				if(c8->state.vram[px] == c8->color1){
					c8->state.reg[0xF] = 1;
					c8->state.vram[px] = c8->color0;
				} else { c8->state.vram[px] = c8->color1; }
			}
		}
	}
}

void c8_EX9E(chip8_system* c8){
	if(c8->state.reg[c8->ox] > 0xF){ c8->ex = INDEX_OUT_OF_BOUNDS; return; }
	if(c8->state.keypad[c8->state.reg[c8->ox]]) c8->state.pc += 2;
}

void c8_EXA1(chip8_system* c8){
	if(c8->state.reg[c8->ox] > 0xF){ c8->ex = INDEX_OUT_OF_BOUNDS; return; }
	if(!c8->state.keypad[c8->state.reg[c8->ox]]) c8->state.pc += 2;
}

void c8_FX07(chip8_system* c8){
	c8->state.reg[c8->ox] = c8->state.delay;
}

void c8_FX0A(chip8_system* c8){
	int waiting = 1; uint8_t k;
	for(k = 0; k < 16; ++k){
		if(c8->state.keypad[k]){
			c8->state.reg[c8->ox] = k;
			waiting = 0;
			break;
		}
	}
	if(waiting) c8->state.pc -= 2;
}

void c8_FX15(chip8_system* c8){
	c8->state.delay = c8->state.reg[c8->ox];
}

void c8_FX18(chip8_system* c8){
	c8->state.sound = c8->state.reg[c8->ox];
}

void c8_FX1E(chip8_system* c8){
	c8->state.i += c8->state.reg[c8->ox];
}

void c8_FX29(chip8_system* c8){
	c8->state.i = c8->font_start + (c8->state.reg[c8->ox] * 5);
}

void c8_FX33(chip8_system* c8){
	if(c8->state.i > 0xFFD){ c8->ex = INDEX_OUT_OF_BOUNDS; return; }
	c8->state.ram[c8->state.i]     = (c8->state.reg[c8->ox] / 100);
	c8->state.ram[c8->state.i + 1] = ((c8->state.reg[c8->ox] % 100) / 10);
	c8->state.ram[c8->state.i + 2] = (c8->state.reg[c8->ox] % 10);
}

void c8_FX55(chip8_system* c8){
	uint8_t n;
	for(n = 0; n <= c8->ox; ++n){
		if(c8->state.i + n > 0xFFF){ c8->ex = INDEX_OUT_OF_BOUNDS; return; }
		*(c8->state.ram + c8->state.i + n) = c8->state.reg[n];
	}
}

void c8_FX65(chip8_system* c8){
	uint8_t n;
	for(n = 0; n <= c8->ox; ++n){
		if(c8->state.i + n > 0xFFF){ c8->ex = INDEX_OUT_OF_BOUNDS; return; }
		c8->state.reg[n] = *(c8->state.ram + c8->state.i + n);
	}
}

/*

void c8_FX33(chip8_system* c8){
	if(c8_I > 0xFFD){ c8_ex = INDEX_OUT_OF_BOUNDS; return; }
	c8_RAM[c8_I]     = (c8_VX[c8_X] / 100);
	c8_RAM[c8_I + 1] = ((c8_VX[c8_X] % 100) / 10);
	c8_RAM[c8_I + 2] = (c8_VX[c8_X] % 10);
}
void c8_FX55(chip8_system* c8){
	uint8_t i;
	for(i = 0; i <= c8_X; ++i){
		if(c8_I + i > 0xFFF){ c8_ex = INDEX_OUT_OF_BOUNDS; return; }
		*(c8_RAM + c8_I + i) = c8_VX[i];
	}
}
void c8_FX65(chip8_system* c8){
	uint8_t i;
	for(i = 0; i <= c8_X; ++i){
		if(c8_I + i > 0xFFF){ c8_ex = INDEX_OUT_OF_BOUNDS; return; }
		c8_VX[i] = *(c8_RAM + c8_I + i);		
	}
}

*/

/*
void c8_EX9E(chip8_system* c8){ if( c8_keypad[c8_VX[c8_X]]) c8_PC += 2; }
void c8_EXA1(chip8_system* c8){ if(!c8_keypad[c8_VX[c8_X]]) c8_PC += 2; }

void c8_FX07(chip8_system* c8){ c8_VX[c8_X] = c8_DELAY; }
void c8_FX0A(chip8_system* c8){
	int waiting = 1; uint8_t k;
	for(k = 0; k < 16; ++k){
		if(c8_keypad[k]){
			c8_VX[c8_X] = k;
			waiting = 0;
			break;
		}
	}
	if(waiting) c8_PC -= 2;
}
void c8_FX15(chip8_system* c8){ c8_DELAY = c8_VX[c8_X]; }
void c8_FX18(chip8_system* c8){ c8_SOUND = c8_VX[c8_X]; }
void c8_FX1E(chip8_system* c8){ c8_I += c8_VX[c8_X]; }
void c8_FX29(chip8_system* c8){ c8_I = c8_font_start + (c8_VX[c8_X] * 5); }
*/

/*
void c8_9XY0(chip8_system* c8){ if(c8_VX[c8_X] != c8_VX[c8_Y]) c8_PC += 2; }
void c8_ANNN(chip8_system* c8){ c8_I = (c8_OP & 0x0FFF); }
void c8_BNNN(chip8_system* c8){ c8_PC = (c8_OP & 0x0FFF) + c8_VX[0]; }
void c8_CXKK(chip8_system* c8){ c8_VX[c8_X] = (rand() & (c8_OP & 0xFF)); }
void c8_DXYN(chip8_system* c8){
	c8_VX[0xF] = 0;
	uint8_t r, c;
	for(r = 0; r < (c8_OP & 0xF); ++r){
		if(c8_I + r > 0xFFF){ c8_ex = INDEX_OUT_OF_BOUNDS; return; }
		uint8_t font_row = c8_RAM[c8_I + r];
		for(c = 0; c < 8; ++c){
			uint8_t font_pixel = (font_row & (0x80 >> c));
			uint32_t i = (((c8_VX[c8_Y] + r) % c8_scrh) * c8_scrw) \
				+ ((c8_VX[c8_X] + c) % c8_scrw);
			if(font_pixel){
				if(c8_VRAM[i] == c8_1_color){
					c8_VX[0xF] = 1;
					c8_VRAM[i] = c8_0_color;
				} else { c8_VRAM[i] = c8_1_color; }
			}
		}
	}
}
*/

/*
void c8_8XY0(chip8_system* c8){ c8_VX[c8_X] = c8_VX[c8_Y]; }
void c8_8XY1(chip8_system* c8){ c8_VX[c8_X] = c8_VX[c8_X] | c8_VX[c8_Y]; }
void c8_8XY2(chip8_system* c8){ c8_VX[c8_X] = c8_VX[c8_X] & c8_VX[c8_Y]; }
void c8_8XY3(chip8_system* c8){ c8_VX[c8_X] = c8_VX[c8_X] ^ c8_VX[c8_Y]; }
void c8_8XY4(chip8_system* c8){ 
	uint16_t res = c8_VX[c8_X] + c8_VX[c8_Y];
	if(res > 0xFF) c8_VX[0xF] = 1;
	c8_VX[c8_X] = (res & 0xFF);
}
void c8_8XY5(chip8_system* c8){
	if(c8_VX[c8_X] > c8_VX[c8_Y]) c8_VX[0xF] = 1; else c8_VX[0xF] = 0;
	c8_VX[c8_X] = c8_VX[c8_X] - c8_VX[c8_Y];
}
void c8_8XY6(chip8_system* c8){
	if((c8_VX[c8_X] & 0x01)) c8_VX[0xF] = 1; else c8_VX[0xF] = 0;
	c8_VX[c8_X] = (c8_VX[c8_X] >> 1);
}
void c8_8XY7(chip8_system* c8){
	if(c8_VX[c8_Y] > c8_VX[c8_X]) c8_VX[0xF] = 1; else c8_VX[0xF] = 0;
	c8_VX[c8_X] = c8_VX[c8_X] - c8_VX[c8_Y];
}
void c8_8XYE(chip8_system* c8){
	if((c8_VX[c8_X] & 0x80)) c8_VX[0xF] = 1; else c8_VX[0xF] = 0;
	c8_VX[c8_X] = (c8_VX[c8_X] << 1);
}
*/

/*
void c8_3XKK(chip8_system* c8){ if(c8_VX[c8_X] == (c8_OP & 0xFF)) c8_PC += 2; }
void c8_4XKK(chip8_system* c8){ if(c8_VX[c8_X] != (c8_OP & 0xFF)) c8_PC += 2; }
void c8_5XY0(chip8_system* c8){ if(c8_VX[c8_X] == c8_VX[c8_Y])    c8_PC += 2; }
void c8_6XKK(chip8_system* c8){ c8_VX[c8_X] =  (c8_OP & 0xFF); }
void c8_7XKK(chip8_system* c8){ c8_VX[c8_X] += (c8_OP & 0xFF); }
*/

//void c8_INVOP(chip8_system* c8){ printf("bruh moment\n"); }

/*
void c8_0NNN(chip8_system* c8){ printf("bro you just posted cringe\n"); }
void c8_00E0(chip8_system* c8){
	//memset(c8_VRAM, c8_0_color, c8_scrw*c8_scrh*sizeof(uint32_t));
	uint32_t i;
	for(i = 0; i < c8_scrw*c8_scrh; ++i){
		c8_VRAM[i] = c8_0_color;
	}
}
void c8_00EE(chip8_system* c8){
	--c8_SP;
	if(c8_SP > 0xF){ c8_ex = STACK_POINTER_OOB; return; }
	c8_PC = c8_STACK[c8_SP];
}

void c8_1NNN(chip8_system* c8){ c8_PC = (c8_OP & 0x0FFF); }
void c8_2NNN(chip8_system* c8){
	if(c8_SP > 0xF){ c8_ex = STACK_POINTER_OOB; return; }
	c8_STACK[c8_SP] = c8_PC;
	++c8_SP;
	c8_PC = (c8_OP & 0x0FFF);
}
*/
