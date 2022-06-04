#include "chip.h"
#include "interface.h"
#include <unistd.h>
#include <time.h>

int main(int argc, char** argv){
	SDL_Init(SDL_INIT_VIDEO);
	initscr();
	int q = 0;
	chip8_emulator* emu = (chip8_emulator*)malloc(sizeof(chip8_emulator));
	if(interface_init(emu, argv[1])) return 1;
	if(argc < 2){
		printf("Please specify a ROM to load\n");
		return 1;
	}

	clock_t t_start, t_end, t_accum;
	clock_t cpf = CLOCKS_PER_SEC / (emu->c8->delay_freq*10);
	t_accum = 0;
	uint32_t dcc = 0;
	
	emu->c8->instr = c8_00E0;
	update_gfx(emu);
	
	while(!q){
		if(emu->c8->st == RUNNING){
			t_start = clock();
			q = keypad_input(emu);
			c8_cycle(emu->c8);
			handle_exception(emu);
			if(emu->c8->st == EXCEPTION){
				if(emu->debug_enable) update_debug(emu);
				continue;
			}
			update_gfx(emu);
			if(emu->debug_enable && emu->debug_toggle){
				++dcc;
				if(dcc >= emu->cycles_until_update){
					update_debug(emu);
					dcc = 0;
				}
			}
			t_end = clock();
			t_accum += t_end - t_start;
			if(t_accum >= cpf){
				c8_update_timer(emu->c8);
				t_accum -= cpf;
			}
			usleep(CLOCKS_PER_SEC/emu->c8->cycles_per_sec);
		} else { 
			usleep(1000);
			if(emu->debug_enable) update_debug(emu);
			q = keypad_input(emu);
		}
	}

	interface_deinit(emu);
	endwin();
	SDL_Quit();
}
