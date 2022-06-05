#include "interface.h"

SDL_Keycode default_keypad_mapping[16] = {
	SDLK_x, SDLK_1, SDLK_2, SDLK_3,
	SDLK_q, SDLK_w, SDLK_e, SDLK_a,
	SDLK_s, SDLK_d, SDLK_z, SDLK_c,
	SDLK_4, SDLK_r, SDLK_f, SDLK_v
};

int interface_init(chip8_emulator* emu, char* filename){
	emu->gfx_scaling = 8;
	emu->cycles_until_update = 1;
	
	emu->prev_pressed = 0;
	emu->prev_release = 0;
	emu->pause_rel = 1;
	emu->debug_rel = 1;
	emu->debug_toggle = 1;
	emu->ff_hold = 0;
	emu->slow_hold = 0;

	memcpy(emu->keypad_mapping, default_keypad_mapping, sizeof(SDL_Keycode)*16);
	emu->c8 = c8_create_system();
	if(c8_init(emu->c8, filename, NULL)) printf("error with c8 init\n");
	if(COLS < 80 || LINES < 24){
		printf("Please resize terminal for debugging info (80x24)\n");
		printf("%d, %d\n", COLS, LINES);
		emu->debug_enable = 0;
	} else {
		emu->debug_win = new_win(24, 80, 0, 0);
		emu->debug_enable = 1;
		init_debug(emu);
	}

	emu->orig_cpc = emu->c8->cycles_per_sec;
	emu->orig_freq = emu->c8->delay_freq;
	emu->speed_scaling = 1.0;
	emu->speed_factor = 4.0;
	
	emu->gfx_win = SDL_CreateWindow("Chipical", 0, 0, emu->c8->scrw*emu->gfx_scaling,
		emu->c8->scrh*emu->gfx_scaling, SDL_WINDOW_SHOWN);
	emu->render = SDL_CreateRenderer(emu->gfx_win, -1, SDL_RENDERER_ACCELERATED); //SDL_RENDERER_SOFTWARE
	emu->texture = SDL_CreateTexture(emu->render, SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING, emu->c8->scrw, emu->c8->scrh);
	return 0;
}

void interface_deinit(chip8_emulator* emu){
	if(emu){
		c8_deinit(emu->c8);
		if(emu->debug_win) rm_win(emu->debug_win);
		SDL_DestroyTexture(emu->texture);
		SDL_DestroyRenderer(emu->render);
		SDL_DestroyWindow(emu->gfx_win);
		free(emu);
		emu = NULL;
	}
}

void update_gfx(chip8_emulator* emu){
	if(emu->c8->instr == c8_00E0 || emu->c8->instr == c8_DXYN){
		SDL_UpdateTexture(emu->texture, NULL, emu->c8->state.vram,
			(sizeof(uint32_t)*emu->c8->scrw));
		SDL_RenderClear(emu->render);
		SDL_RenderCopy(emu->render, emu->texture, NULL, NULL);
		SDL_RenderPresent(emu->render);
	}
}

int keypad_input(chip8_emulator* emu){
	int q = 0;
	
	SDL_Event e;
	
	while(SDL_PollEvent(&e)){
		switch(e.type){
		
			case SDL_QUIT: {
				q = 1;
			} break;
			
			case SDL_KEYDOWN: {
				SDL_Keycode k = e.key.keysym.sym;
				emu->prev_pressed = k;

				// Pause toggle
				if(k == SDLK_p && emu->pause_rel){
					if(emu->c8->st == RUNNING){
						emu->c8->st = PAUSED;
						emu->pause_rel = 0;
						break;
					}
					if(emu->c8->st == PAUSED){
						emu->c8->st = RUNNING;
						emu->pause_rel = 0;
						break;
					}
				}

				// Debug toggle
				if(k == SDLK_y && emu->debug_rel){
					if(emu->debug_toggle == 1){
						emu->debug_toggle = 0;
						emu->debug_rel = 0;
						break;
					}
					if(emu->debug_toggle == 0){
						emu->debug_toggle = 1;
						emu->debug_rel = 0;
						break;
					}
				}

				// Fast forward hold
				if(k == SDLK_l && emu->ff_hold == 0){
					emu->ff_hold = 1;
					emu->speed_scaling *= emu->speed_factor;
					rescale_speed(emu, emu->speed_scaling);
					break;
				}

				// Slow down hold
				if(k == SDLK_k && emu->slow_hold == 0){
					emu->slow_hold = 1;
					emu->speed_scaling /= emu->speed_factor;
					rescale_speed(emu, emu->speed_scaling);
					break;
				}

				uint8_t kp;
				if(k == SDLK_ESCAPE){ q = 1; break; }
				for(kp = 0; kp < 16; ++kp){
					if(k == emu->keypad_mapping[kp]){
						emu->c8->state.keypad[kp] = 1;
						emu->change[S_KEY] = 1;
					}
				}
			} break;
			
			case SDL_KEYUP: {
				SDL_Keycode k = e.key.keysym.sym;
				emu->prev_release = k;
				
				if(k == SDLK_p) emu->pause_rel = 1;
				if(k == SDLK_y) emu->debug_rel = 1;
				if(k == SDLK_l){
					emu->ff_hold = 0;
					emu->speed_scaling /= emu->speed_factor;
					rescale_speed(emu, emu->speed_scaling);
					//break;
				}
				if(k == SDLK_k){
					emu->slow_hold = 0;
					emu->speed_scaling *= emu->speed_factor;
					rescale_speed(emu, emu->speed_scaling);
					//break;
				}
				
				uint8_t kp;
				for(kp = 0; kp < 16; ++kp){
					if(k == emu->keypad_mapping[kp]){
						emu->c8->state.keypad[kp] = 0;
						emu->change[S_KEY] = 1;
						//break;
					}
				}
			} break;
			
		}
	}

	return q;
}

WINDOW* new_win(int h, int w, int sy, int sx){
	WINDOW* win = newwin(h, w, sy, sx);
	box(win, 0, 0);
	wrefresh(win);
	return win;
}

void rm_win(WINDOW* win){
	wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(win);
	delwin(win);
}

void init_debug(chip8_emulator* emu){

	int i;
	emu->prev_state.i = 0;
	emu->prev_state.pc = 0;
	emu->prev_state.sp = 0;
	emu->prev_state.delay = 0;
	emu->prev_state.sound = 0;
	for(i = 0; i < 16; ++i){
		emu->prev_state.reg[i] = 0;
		emu->prev_state.keypad[i] = 0;
		emu->prev_state.stack[i] = 0;
	}

	int r, c;
	r = 1; c = 5;
	mvprintw(r, c, "REGISTERS");
	++r;
	for(i = 0; i < 16; ++i){
		mvprintw(r+i, c, "V%01X:", i);
		mvprintw(r+i, c+5, "%02X", emu->c8->state.reg[i]);
		//mvprintw(r+i, c+5, "%02X", c8_VX[i]);
	}

	r = 18;
	mvprintw(1+r, c, "DELAY:");
	mvprintw(2+r, c, "SOUND:");
	mvprintw(4+r, c, "  I:");
	mvprintw(1+r, 7+c, "%03d", emu->c8->state.delay);
	mvprintw(2+r, 7+c, "%03d", emu->c8->state.sound);
	mvprintw(4+r, 5+c, "%04X", emu->c8->state.i);

	r = 1; c = 20;
	mvprintw(r, c, "STACK");
	++r;
	for(i = 0; i < 16; ++i){
		mvprintw(r+i, c, "%02X", emu->c8->state.stack[i]);
	}

	r = 18; c = 20;
	mvprintw(2+r, 4+c, "%04X", emu->c8->o);
	mvprintw(3+r, 4+c, "%04X", emu->c8->state.pc);
	r = 18;
	mvprintw(2+r, c, "OP:");
	mvprintw(3+r, c, "PC:");

	c = 30; r = 4;
	mvprintw(r, c, "KEYPAD");
	c = 30; r = 5;
	mvprintw(2+r, c, "%01X %01X %01X %01X",
		emu->c8->state.keypad[0x1], emu->c8->state.keypad[0x2],
		emu->c8->state.keypad[0x3], emu->c8->state.keypad[0xC]);
	mvprintw(4+r, c, "%01X %01X %01X %01X",
		emu->c8->state.keypad[0x4], emu->c8->state.keypad[0x5],
		emu->c8->state.keypad[0x6], emu->c8->state.keypad[0xD]);
	mvprintw(6+r, c, "%01X %01X %01X %01X",
		emu->c8->state.keypad[0x7], emu->c8->state.keypad[0x8],
		emu->c8->state.keypad[0x9], emu->c8->state.keypad[0xE]);
	mvprintw(8+r, c, "%01X %01X %01X %01X",
		emu->c8->state.keypad[0xA], emu->c8->state.keypad[0x0],
		emu->c8->state.keypad[0xB], emu->c8->state.keypad[0xF]);

	wrefresh(emu->debug_win);
	refresh();
	//mvprintw()
}

void update_debug(chip8_emulator* emu){
	int i, r, c;
	r = 1; c = 5;

	update_prev_state(emu);

	if(emu->change[S_REG]){
		++r;
		for(i = 0; i < 16; ++i){
			mvprintw(r+i, c+5, "%02X", emu->c8->state.reg[i]);
		}	
	}

	if(emu->change[S_TIMER]){
		r = 18;
		mvprintw(1+r, 7+c, "%03d", emu->c8->state.delay);
		mvprintw(2+r, 7+c, "%03d", emu->c8->state.sound);
	}

	if(emu->change[S_I]){
		r = 18;
		mvprintw(4+r, 5+c, "%04X", emu->c8->state.i);
	}

	if(emu->change[S_SP]){
		r = 1; c = 20;
		++r;
		for(i = 0; i < 16; ++i){
			mvprintw(r+i, c, "%02X", emu->c8->state.stack[i]);
		}
	}
	
	r = 18; c = 20;
	mvprintw(2+r, 4+c, "%04X", emu->c8->o);
	mvprintw(3+r, 4+c, "%04X", emu->c8->state.pc);

	if(emu->change[S_KEY]){
		c = 30; r = 5;
		mvprintw(2+r, c, "%01X %01X %01X %01X",
			emu->c8->state.keypad[0x1], emu->c8->state.keypad[0x2],
			emu->c8->state.keypad[0x3], emu->c8->state.keypad[0xC]);
		mvprintw(4+r, c, "%01X %01X %01X %01X",
			emu->c8->state.keypad[0x4], emu->c8->state.keypad[0x5],
			emu->c8->state.keypad[0x6], emu->c8->state.keypad[0xD]);
		mvprintw(6+r, c, "%01X %01X %01X %01X",
			emu->c8->state.keypad[0x7], emu->c8->state.keypad[0x8],
			emu->c8->state.keypad[0x9], emu->c8->state.keypad[0xE]);
		mvprintw(8+r, c, "%01X %01X %01X %01X",
			emu->c8->state.keypad[0xA], emu->c8->state.keypad[0x0],
			emu->c8->state.keypad[0xB], emu->c8->state.keypad[0xF]);
	}	

	for(i = 0; i < 8; ++i){ emu->change[i] = 0; }

	wrefresh(emu->debug_win);
	refresh();
}

void handle_exception(chip8_emulator* emu){
	if(emu->c8->ex == ALL_OK) return;
	if(emu->debug_enable){
		char exmsg[32];
		switch(emu->c8->ex){
			case ALL_OK: return;
			case INDEX_OUT_OF_BOUNDS:
				strncpy(exmsg, "INDEX OUT OF BOUNDS\0", 32);
				break;
			case STACK_POINTER_OOB:
				strncpy(exmsg, "STACK POINTER OOB\0", 32);
				break;
			case PROGRAM_COUNTER_OOB:
				strncpy(exmsg, "PROGRAM COUNTER OOB\0", 32);
				break;
			case UNKNOWN_OPCODE:
				strncpy(exmsg, "UNKNOWN OPCODE\0", 32);
				break;
			default: return;
		}
		mvprintw(20, 30, "Exception occurred:");
		mvprintw(21, 30, "%s", exmsg);
		wrefresh(emu->debug_win);
		refresh();
	}
	emu->c8->st = EXCEPTION;
}

void update_prev_state(chip8_emulator* emu){
	int i;
	if(emu->c8->state.i != emu->prev_state.i){
		emu->prev_state.i = emu->c8->state.i;
		emu->change[S_I] = 1;
	}
	if(emu->c8->state.pc != emu->prev_state.pc){
		emu->prev_state.pc = emu->c8->state.pc;
		emu->change[S_PC] = 1;
	}
	if(emu->c8->state.sp != emu->prev_state.sp){
		emu->prev_state.sp = emu->c8->state.sp;
		emu->change[S_SP] = 1;
		for(i = 0; i < 16; ++i){
			emu->prev_state.stack[i] = emu->c8->state.stack[i];
		}
	}
	if(emu->c8->state.delay != emu->prev_state.delay){
		emu->prev_state.delay = emu->c8->state.delay;
		emu->change[S_TIMER] = 1;
	}
	if(emu->c8->state.sound != emu->prev_state.sound){
		emu->prev_state.sound = emu->c8->state.sound;
		emu->change[S_TIMER] = 1;
	}
	for(i = 0; i < 16; ++i){
		if(emu->c8->state.reg[i] != emu->prev_state.reg[i]){
			emu->prev_state.reg[i] = emu->c8->state.reg[i];
			emu->change[S_REG] = 1;
		}
		/*
		if(emu->c8->state.keypad[i] != emu->prev_state.keypad[i]){
			emu->prev_state.keypad[i] = emu->c8->state.keypad[i];
			emu->change[S_KEY] = 1;
		}
		*/
	}
}

void rescale_speed(chip8_emulator* emu, float factor){
	emu->c8->cycles_per_sec = emu->orig_cpc*factor;
	emu->c8->delay_freq = emu->orig_freq/factor;
	//printf("sc: %f, f: %f", emu->speed_scaling, emu->speed_factor);
}
