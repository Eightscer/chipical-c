#include "chip.h"
#include "interface.h"
#include <unistd.h>
#include <time.h>

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
	//float t_elap_delay = 0;
	clock_t t_start, t_end, t_accum;
	clock_t cpf = CLOCKS_PER_SEC / (c8_delay_freq*10);
	t_accum = 0;
	uint32_t dcc = 0;
	//uint32_t delay_per_sec;
	//uint32_t avg_cps;
	while(!q){
		if(!c8_pause){
			t_start = clock();
			q = keypad_input(c8_keypad);
			c8_cycle();
			handle_exception();
			if(c8_pause){ if(debug_enable) update_debug(); continue; }
			update_gfx();
			//clock_t delay = (CLOCKS_PER_SEC/c8_cps) - (clock() - t_start);
			//printf("%zu\n", delay);
			if(debug_enable){
				++dcc;
				if(dcc >= cycles_until_update){
					update_debug();
					dcc = 0;		
				}
			}

			t_end = clock();
			t_accum += t_end - t_start;
			if(t_accum > cpf){
				c8_update_timer();
				//printf("%zu, %zu\n", t_accum, cpf);
				t_accum -= cpf;
			}

			//t_accum += clock() - t_start;
			usleep(CLOCKS_PER_SEC/c8_cps);
		} else { usleep(1000); q = keypad_input(c8_keypad); }
	}
	printf("deinit interface\n");
	interface_deinit();
	printf("deinit chip8\n");
	c8_deinit();
	return 0;
}
