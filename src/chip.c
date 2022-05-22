#include "chip.h"

uint16_t c8_I, c8_PC;
uint8_t c8_VX[16];
uint8_t c8_SP, c8_DELAY, c8_SOUND;
uint16_t c8_STACK[16];
uint8_t c8_keypad[16];

uint8_t c8_font[16*5] = {
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

int c8_init(){
	srand(time(NULL));

	c8_PC = 0x0200;	c8_I = 0x0000;
	c8_SP = 0x00; c8_DELAY = 0x00; c8_SOUND = 0x00;
	int i; for(i = 0; i < 16; ++i){
		c8_VX[i] = 0x00;
		c8_STACK[i] = 0x0000;
		c8_keypad[i] = 0x00;
	}

	c8_op_init();

	c8_scrh = 32;
	c8_scrw = 64;

	c8_0_color = 0x00000000;
	c8_1_color = 0x00FF7F00;

	if(c8_RAM){ free(c8_RAM); }
	if(c8_VRAM){ free(c8_VRAM); }
	c8_RAM = (uint8_t*)calloc(0x1000, sizeof(uint8_t));
	if(!c8_RAM){ printf("RAM not free\n"); return -1; }
	c8_VRAM = (uint32_t*)calloc(c8_scrw*c8_scrh, sizeof(uint32_t));
	if(!c8_VRAM){ printf("VRAM not free\n"); return -1; }

	memcpy(c8_RAM + c8_font_start, c8_font, sizeof(c8_font));

	return 0;
}

void c8_load_font(char* filename){
	FILE* fp = fopen(filename, "r");
	
	fclose(fp);
}

void c8_load_file(char* filename){
	FILE* fp = fopen(filename, "r");
	if(!fp){ printf("Failed to open file %s\n", filename); return; }
	fseek(fp, 0, SEEK_SET);
	fread(c8_RAM + 0x200, sizeof(uint8_t), 0xE00, fp);
	fclose(fp);
}

void c8_op_init(){

	c8_opcodes_lut[0x0] = c8_op0;
	c8_opcodes_lut[0x1] = c8_1NNN;
	c8_opcodes_lut[0x2] = c8_2NNN;
	c8_opcodes_lut[0x3] = c8_3XKK;
	c8_opcodes_lut[0x4] = c8_4XKK;
	c8_opcodes_lut[0x5] = c8_5XY0;
	c8_opcodes_lut[0x6] = c8_6XKK;
	c8_opcodes_lut[0x7] = c8_7XKK;
	c8_opcodes_lut[0x8] = c8_op8;
	c8_opcodes_lut[0x9] = c8_9XY0;
	c8_opcodes_lut[0xA] = c8_ANNN;
	c8_opcodes_lut[0xB] = c8_BNNN;
	c8_opcodes_lut[0xC] = c8_CXKK;
	c8_opcodes_lut[0xD] = c8_DXYN;
	c8_opcodes_lut[0xE] = c8_opE;
	c8_opcodes_lut[0xF] = c8_opF;

	c8_opcode0_lut[0x0] = c8_00E0;
	c8_opcode0_lut[0xE] = c8_00EE;

	c8_opcode8_lut[0x0] = c8_8XY0;
	c8_opcode8_lut[0x1] = c8_8XY1;
	c8_opcode8_lut[0x2] = c8_8XY2;
	c8_opcode8_lut[0x3] = c8_8XY3;
	c8_opcode8_lut[0x4] = c8_8XY4;
	c8_opcode8_lut[0x5] = c8_8XY5;
	c8_opcode8_lut[0x6] = c8_8XY6;
	c8_opcode8_lut[0x7] = c8_8XY7;
	c8_opcode8_lut[0xE] = c8_8XYE;

	c8_opcodeE_lut[0x1] = c8_EXA1;
	c8_opcodeE_lut[0xE] = c8_EX9E;

	c8_opcodeF_lut[0x07] = c8_FX07;
	c8_opcodeF_lut[0x0A] = c8_FX0A;
	c8_opcodeF_lut[0x15] = c8_FX15;
	c8_opcodeF_lut[0x18] = c8_FX18;
	c8_opcodeF_lut[0x1E] = c8_FX1E;
	c8_opcodeF_lut[0x29] = c8_FX29;
	c8_opcodeF_lut[0x33] = c8_FX33;
	c8_opcodeF_lut[0x55] = c8_FX55;
	c8_opcodeF_lut[0x65] = c8_FX65;
}

void c8_op0(){c8_opcode0_lut[(c8_OP & 0x000F)]();}
void c8_op8(){c8_opcode8_lut[(c8_OP & 0x000F)]();}
void c8_opE(){c8_opcodeE_lut[(c8_OP & 0x000F)]();}
void c8_opF(){c8_opcodeF_lut[(c8_OP & 0x00FF)]();}

void c8_INVOP(){ printf("bruh moment\n"); }

void c8_0NNN(){ printf("bro you just posted cringe\n"); }
void c8_00E0(){ memset(c8_VRAM, 0, c8_scrw*c8_scrh*sizeof(uint32_t)); }
void c8_00EE(){ --c8_SP; c8_PC = c8_STACK[c8_SP]; }

void c8_1NNN(){ c8_PC = (c8_OP & 0x0FFF); }
void c8_2NNN(){ c8_STACK[c8_SP] = c8_PC; ++c8_SP; c8_PC = (c8_OP & 0x0FFF); }
void c8_3XKK(){ if(c8_VX[c8_X] == (c8_OP & 0xFF)) c8_PC += 2; }
void c8_4XKK(){ if(c8_VX[c8_X] != (c8_OP & 0xFF)) c8_PC += 2; }
void c8_5XY0(){ if(c8_VX[c8_X] == c8_VX[c8_Y])    c8_PC += 2; }
void c8_6XKK(){ c8_VX[c8_X] =  (c8_OP & 0xFF); }
void c8_7XKK(){ c8_VX[c8_X] += (c8_OP & 0xFF); }

void c8_8XY0(){ c8_VX[c8_X] = c8_VX[c8_Y]; }
void c8_8XY1(){ c8_VX[c8_X] = c8_VX[c8_X] | c8_VX[c8_Y]; }
void c8_8XY2(){ c8_VX[c8_X] = c8_VX[c8_X] & c8_VX[c8_Y]; }
void c8_8XY3(){ c8_VX[c8_X] = c8_VX[c8_X] ^ c8_VX[c8_Y]; }
void c8_8XY4(){ 
	uint16_t res = c8_VX[c8_X] + c8_VX[c8_Y];
	if(res > 0xFF) c8_VX[0xF] = 1;
	c8_VX[c8_X] = (res & 0xFF);
}
void c8_8XY5(){
	if(c8_VX[c8_X] > c8_VX[c8_Y]) c8_VX[0xF] = 1; else c8_VX[0xF] = 0;
	c8_VX[c8_X] = c8_VX[c8_X] - c8_VX[c8_Y];
}
void c8_8XY6(){
	if((c8_VX[c8_X] & 0x01)) c8_VX[0xF] = 1; else c8_VX[0xF] = 0;
	c8_VX[c8_X] = (c8_VX[c8_X] >> 1);
}
void c8_8XY7(){
	if(c8_VX[c8_Y] > c8_VX[c8_X]) c8_VX[0xF] = 1; else c8_VX[0xF] = 0;
	c8_VX[c8_X] = c8_VX[c8_X] - c8_VX[c8_Y];
}
void c8_8XYE(){
	if((c8_VX[c8_X] & 0x80)) c8_VX[0xF] = 1; else c8_VX[0xF] = 0;
	c8_VX[c8_X] = (c8_VX[c8_X] << 1);
}

void c8_9XY0(){ if(c8_VX[c8_X] != c8_VX[c8_Y]) c8_PC += 2; }
void c8_ANNN(){ c8_I = (c8_OP & 0x0FFF); }
void c8_BNNN(){ c8_PC = (c8_OP & 0x0FFF) + c8_VX[0]; }
void c8_CXKK(){ c8_VX[c8_X] = (rand() & (c8_OP & 0xFF)); }
void c8_DXYN(){
	c8_VX[0xF] = 0;
	uint8_t r, c;
	for(r = 0; r < (c8_OP & 0xF); ++r){
		uint8_t font_row = c8_RAM[c8_I + r];
		for(c = 0; c < 8; ++c){
			uint8_t font_pixel = (font_row & (0x80 >> c));
			uint32_t i = (((c8_VX[c8_Y] + r) % c8_scrh) * c8_scrw) \
				+ ((c8_VX[c8_X] + c) % c8_scrw);
			if(font_pixel){
				if(c8_VRAM[i] == c8_1_color) c8_VX[0xF] = 1;
				c8_VRAM[i] ^= c8_1_color;
			}
		}
	}
}

void c8_EX9E(){ if( c8_keypad[c8_VX[c8_X]]) c8_PC += 2; }
void c8_EXA1(){ if(!c8_keypad[c8_VX[c8_X]]) c8_PC += 2; }

void c8_FX07(){ c8_VX[c8_X] = c8_DELAY; }
void c8_FX0A(){
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
void c8_FX15(){ c8_DELAY = c8_VX[c8_X]; }
void c8_FX18(){ c8_SOUND = c8_VX[c8_X]; }
void c8_FX1E(){ c8_I += c8_VX[c8_X]; }
void c8_FX29(){ c8_I = c8_font_start + (c8_VX[c8_X] * 5); }
void c8_FX33(){
	c8_RAM[c8_I]     = (c8_VX[c8_X] / 100);
	c8_RAM[c8_I + 1] = ((c8_VX[c8_X] % 100) / 10);
	c8_RAM[c8_I + 2] = (c8_VX[c8_X] % 10);
}
void c8_FX55(){
	uint8_t i;
	for(i = 0; i <= c8_X; ++i)
		*(c8_RAM + c8_I + i) = c8_VX[i];
}
void c8_FX65(){
	uint8_t i;
	for(i = 0; i <= c8_X; ++i)
		c8_VX[i] = *(c8_RAM + c8_I + i);
}

void c8_cycle(){
	c8_fetch();
	c8_PC += 2;
	c8_execute();
	if(c8_DELAY) --c8_DELAY;
	if(c8_SOUND) --c8_SOUND;
}

void c8_fetch(){
	c8_OP = ((c8_RAM[c8_PC] << 8) | c8_RAM[c8_PC + 1]);
	c8_X = ((c8_OP & 0x0F00) >> 8);
	c8_Y = ((c8_OP & 0x00F0) >> 4); 
}

void c8_execute(){
	c8_opcodes_lut[(c8_OP & 0xF000) >> 12]();
}

void c8_deinit(){
	if(c8_RAM){ free(c8_RAM); }
	if(c8_VRAM){ free(c8_VRAM); }
}
