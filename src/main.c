#include "chip.h"
#include "interface.h"
#include <unistd.h>

int main(int argc, char** argv){
	if(c8_init()) return 1;
	interface_init();
	update_gfx();
	if(argc > 1){
		if(c8_load_file(argv[1])) return 1;
	} else {
		if(c8_load_file("test.ch8")) return 1;		
	}
	int q = 0;
	//float t_elap_cycle = 0;
	float t_elap_delay = 0;
	while(!q){
		uint32_t t_start = SDL_GetPerformanceCounter();
		q = keypad_input(c8_keypad);
		c8_cycle();
		update_gfx();
		uint32_t t_end = SDL_GetPerformanceCounter();
		float elap = (t_end - t_start) / (float)SDL_GetPerformanceFrequency();

		//t_elap_cycle = elap;
		t_elap_delay += elap;

		if(t_elap_delay > 1.0 / c8_delay_freq){
			if(c8_DELAY) --c8_DELAY;
			if(c8_SOUND) --c8_SOUND;
			t_elap_delay -= 1.0 / c8_delay_freq;
			printf("delay hit\n");
		}

		//uint32_t wtf = (uint32_t)(((float)SDL_GetPerformanceFrequency() / (float)c8_cps)) - t_elap_cycle;

		//usleep((uint32_t)(((float)SDL_GetPerformanceFrequency() / (float)c8_cps)) - t_elap_cycle);
		usleep((1000000*c8_cps) / SDL_GetPerformanceFrequency());

		//printf("%f     %f     %f\n", t_elap_cycle, t_elap_delay, elap);
		//printf("%d\n", wtf);
		//SDL_Delay((uint32_t)(16.666f - t_elapsed));
		//printf("%04x    %04x\n", c8_PC, c8_OP);
	}
	printf("deinit interface\n");
	interface_deinit();
	printf("deinit chip8\n");
	c8_deinit();
	return 0;
}
