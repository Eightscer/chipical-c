#include "interface.h"
//#include "chip.h"

/*
SDL_Window* gfx_win;
SDL_Renderer* render;
SDL_Texture* texture;
size_t scaling;
uint32_t cycles_until_update;
WINDOW* debug_win;
int debug_enable;
int c8_pause = 0;
*/

SDL_Keycode default_keypad_mapping[16] = {
	SDLK_x, SDLK_1, SDLK_2, SDLK_3,
	SDLK_q, SDLK_w, SDLK_e, SDLK_a,
	SDLK_s, SDLK_d, SDLK_z, SDLK_c,
	SDLK_4, SDLK_r, SDLK_f, SDLK_v
};

int interface_init(chip8_emulator* emu, char* filename){
	emu->gfx_scaling = 8;
	emu->cycles_until_update = 1;
	emu->prev_key = 0;
	emu->curr_key = 0;
	//printf("keypad mapping init\n");
	memcpy(emu->keypad_mapping, default_keypad_mapping, sizeof(SDL_Keycode)*16);
	//printf("chip8 init\n");
	emu->c8 = c8_create_system();
	if(c8_init(emu->c8, filename, NULL)) printf("error with c8 init\n");
	//initscr();
	//printf("debug init\n");
	if(COLS < 80 || LINES < 24){
		printf("Please resize terminal for debugging info (80x24)\n");
		printf("%d, %d\n", COLS, LINES);
		emu->debug_enable = 0;
	} else {
		emu->debug_win = new_win(24, 80, 0, 0);
		emu->debug_enable = 1;
		init_debug(emu);
	}
	//SDL_Init(SDL_INIT_VIDEO);
	//printf("sdl gfx window init\n");
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
	//endwin();
	//SDL_Quit();
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
				emu->prev_key = emu->curr_key;
				emu->curr_key = e.key.keysym.sym;
				uint8_t kp;
				if(emu->curr_key == SDLK_ESCAPE){ q = 1; break; }
				for(kp = 0; kp < 16; ++kp){
					if(emu->curr_key == emu->keypad_mapping[kp]){
						emu->c8->state.keypad[kp] = 1;
						//break;
					}
				}
				//printf("press   %d\n", kp);
			} break;
			case SDL_KEYUP: {
				SDL_Keycode k = e.key.keysym.sym;
				uint8_t kp;
				for(kp = 0; kp < 16; ++kp){
					if(k == emu->keypad_mapping[kp]){
						emu->c8->state.keypad[kp] = 0;
						//break;
					}
				}
				//printf("release %d\n", kp);
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
	int i, r, c;
	r = 1; c = 5;
	mvprintw(r, c, "REGISTERS");
	++r;
	for(i = 0; i < 16; ++i){
		mvprintw(r+i, c, "V%01X:", i);
		//mvprintw(r+i, c+5, "%02X", c8_VX[i]);
	}

	r = 18;
	mvprintw(1+r, c, "DELAY:");
	mvprintw(2+r, c, "SOUND:");
	mvprintw(4+r, c, "  I:");

	r = 1; c = 20;
	mvprintw(r, c, "STACK");

	r = 18;
	mvprintw(2+r, c, "OP:");
	mvprintw(3+r, c, "PC:");

	c = 30; r = 4; i = 0;
	mvprintw(r, c, "KEYPAD");
	
	wrefresh(emu->debug_win);
	refresh();
	//mvprintw()
}

void update_debug(chip8_emulator* emu){
	int i, r, c;
	r = 1; c = 5;
	++r;
	for(i = 0; i < 16; ++i){
		mvprintw(r+i, c+5, "%02X", emu->c8->state.reg[i]);
	}

	r = 18;
	mvprintw(1+r, 7+c, "%03d", emu->c8->state.delay);
	mvprintw(2+r, 7+c, "%03d", emu->c8->state.sound);
	mvprintw(4+r, 5+c, "%04X", emu->c8->state.i);

	r = 1; c = 20;
	//mvprintw(r, c, "STACK");
	++r;
	for(i = 0; i < 16; ++i){
		mvprintw(r+i, c, "%02X", emu->c8->state.stack[i]);
	}

	r = 18;
	mvprintw(2+r, 4+c, "%04X", emu->c8->o);
	mvprintw(3+r, 4+c, "%04X", emu->c8->state.pc);

	c = 30; r = 5; i = 0;
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
