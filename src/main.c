#include "chip.h"
#include "interface.h"
#include <unistd.h>
#include <time.h>

int main(int argc, char** argv){
	//printf("init sdl\n");
	SDL_Init(SDL_INIT_VIDEO);
	//printf("init curses\n");
	initscr();
	int q = 0;
	printf("init emu\n");
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

	//printf("running\n");
	if(!emu->debug_enable){
		printf("no debug\n");
	}

	//c8_00E0(emu->c8);
	emu->c8->instr = c8_00E0;
	update_gfx(emu);
	
	while(!q){
		if(emu->c8->st == RUNNING){
			t_start = clock();
			//printf("thing\n");
			q = keypad_input(emu);
			c8_cycle(emu->c8);
			//printf("%04X:%04X ", emu->c8->state.pc, emu->c8->o);
			handle_exception(emu);
			if(emu->c8->st == EXCEPTION){
				if(emu->debug_enable) update_debug(emu);
				continue;
			}
			update_gfx(emu);
			if(emu->debug_enable){
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
			//refresh();
		} else { 
			usleep(1000); update_debug(emu); q = keypad_input(emu);
			//printf("you died\n"); 
		}
	}

	interface_deinit(emu);

	//printf("deinit ncurses\n");
	endwin();
	//printf("deinit sdl\n");
	SDL_Quit();
	
}

/*
int main(int argc, char** argv){
	if(c8_init()) return 1;
	interface_init();
	c8_00E0();
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

			// Messing with colors
			//c8_0_color += 0x100;
			//c8_1_color += 0x100;
			
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
*/
