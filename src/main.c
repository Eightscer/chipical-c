#include "chip.h"
#include "interface.h"

int main(){
	if(c8_init()) return 1;
	interface_init();
	update_gfx();
	c8_load_file("test.ch8");
	int q = 0;
	while(!q){
		q = keypad_input(c8_keypad);
		c8_cycle();
		update_gfx();
		printf("%04x    %04x\n", c8_PC, c8_OP);
	}
	printf("deinit interface\n");
	interface_deinit();
	printf("deinit chip8\n");
	c8_deinit();
	return 0;
}
