#include "chip.h"

int c8_init(){
	c8_PC = 0x0000;	c8_I = 0x0000;
	c8_SP = 0x00; c8_DELAY = 0x00; c8_SOUND = 0x00;
	int i; for(i = 0; i < 16; ++i){ c8_VX[i] = 0x00; c8_STACK[i] = 0x0000; }

	c8_op_init();

	if(c8_RAM){
		free(c8_RAM);
	}
	c8_RAM = (uint8_t*)calloc(0x1000, sizeof(uint8_t));
	if(!c8_RAM){
		return -1;
	}
	return 0;
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
void c8_00E0(){}
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
void c8_8XY5(){}
void c8_8XY6(){}
void c8_8XY7(){}
void c8_8XYE(){}

void c8_9XY0(){ if(c8_VX[c8_X] != c8_VX[c8_Y]) c8_PC += 2; }
void c8_ANNN(){ c8_I = (c8_OP & 0x0FFF); }
void c8_BNNN(){ c8_PC = (c8_OP & 0x0FFF) + c8_VX[0]; }
void c8_CXKK(){}
void c8_DXYN(){}

void c8_EX9E(){}
void c8_EXA1(){}

void c8_FX07(){ c8_VX[c8_X] = c8_DELAY; }
void c8_FX0A(){}
void c8_FX15(){ c8_DELAY = c8_VX[c8_X]; }
void c8_FX18(){ c8_SOUND = c8_VX[c8_X]; }
void c8_FX1E(){ c8_I += c8_VX[c8_X]; }
void c8_FX29(){}
void c8_FX33(){}
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

void c8_execute(){
	c8_X = ((c8_OP & 0x0F00) >> 8);
	c8_Y = ((c8_OP & 0x00F0) >> 4);
	c8_opcodes_lut[(c8_OP & 0xF000) >> 12]();
}

void c8_deinit(){
	if(c8_RAM){
		free(c8_RAM);
	}
}
