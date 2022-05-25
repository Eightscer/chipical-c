#include "interface.h"
#include "chip.h"

SDL_Window* gfx_win;
SDL_Renderer* render;
SDL_Texture* texture;
size_t scaling;
uint32_t cycles_until_update;
WINDOW* debug_win;
int debug_enable;
int c8_pause = 0;

SDL_Keycode keypad_mapping[16] = {
	SDLK_x, SDLK_1, SDLK_2, SDLK_3,
	SDLK_q, SDLK_w, SDLK_e, SDLK_a,
	SDLK_s, SDLK_d, SDLK_z, SDLK_c,
	SDLK_4, SDLK_r, SDLK_f, SDLK_v
};

void interface_init(){
	scaling = 8;
	cycles_until_update = 1;
	initscr();
	if(COLS < 80 || LINES < 24){
		printf("Please resize terminal for debugging info (80x24)\n");
		printf("%d, %d\n", COLS, LINES);
		debug_enable = 0;
	} else {
		debug_win = new_win(24, 80, 0, 0);
		debug_enable = 1;
		init_debug();
	}
	SDL_Init(SDL_INIT_VIDEO);
	gfx_win = SDL_CreateWindow("Chipical", 0, 0, c8_scrw*scaling, c8_scrh*scaling, SDL_WINDOW_SHOWN);
	render = SDL_CreateRenderer(gfx_win, -1, SDL_RENDERER_ACCELERATED); //SDL_RENDERER_SOFTWARE
	texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, c8_scrw, c8_scrh);
}

void interface_deinit(){
	if(debug_win) rm_win(debug_win);
	endwin();
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(gfx_win);
	SDL_Quit();
}

void update_gfx(){
	if((c8_OP == 0x00E0) || (c8_OP & 0xF000) == 0xD000){
	SDL_UpdateTexture(texture, NULL, c8_VRAM, (sizeof(c8_VRAM[0])*c8_scrw));
	SDL_RenderClear(render);
	SDL_RenderCopy(render, texture, NULL, NULL);
	SDL_RenderPresent(render);
	}
}

int keypad_input(uint8_t* keypad){
	int q = 0;
	SDL_Event e;
	
	while(SDL_PollEvent(&e)){
		switch(e.type){
			case SDL_QUIT: {
				q = 1;
			} break;
			case SDL_KEYDOWN: {
				SDL_Keycode k = e.key.keysym.sym;
				uint8_t kp;
				if(k == SDLK_ESCAPE){ q = 1; break; }
				for(kp = 0; kp < 16; ++kp){
					if(k == keypad_mapping[kp]){
						keypad[kp] = 1;
						//break;
					}
				}
				//printf("press   %d\n", kp);
			} break;
			case SDL_KEYUP: {
				SDL_Keycode k = e.key.keysym.sym;
				uint8_t kp;
				for(kp = 0; kp < 16; ++kp){
					if(k == keypad_mapping[kp]){
						keypad[kp] = 0;
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

void init_debug(){
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
	
	refresh();
	//mvprintw()
}

void update_debug(){
	int i, r, c;
	r = 1; c = 5;
	++r;
	for(i = 0; i < 16; ++i){
		mvprintw(r+i, c+5, "%02X", c8_VX[i]);
	}

	r = 18;
	mvprintw(1+r, 7+c, "%03d", c8_DELAY);
	mvprintw(2+r, 7+c, "%03d", c8_SOUND);
	mvprintw(4+r, 5+c, "%04X", c8_I);

	r = 1; c = 20;
	//mvprintw(r, c, "STACK");
	++r;
	for(i = 0; i < 16; ++i){
		mvprintw(r+i, c, "%02X", c8_STACK[i]);
	}

	r = 18;
	mvprintw(2+r, 4+c, "%04X", c8_OP);
	mvprintw(3+r, 4+c, "%04X", c8_PC);

	c = 30; r = 5; i = 0;
	mvprintw(2+r, c, "%01X %01X %01X %01X",
		c8_keypad[0x1], c8_keypad[0x2], c8_keypad[0x3], c8_keypad[0xC]);
	mvprintw(4+r, c, "%01X %01X %01X %01X",
		c8_keypad[0x4], c8_keypad[0x5], c8_keypad[0x6], c8_keypad[0xD]);
	mvprintw(6+r, c, "%01X %01X %01X %01X",
		c8_keypad[0x7], c8_keypad[0x8], c8_keypad[0x9], c8_keypad[0xE]);
	mvprintw(8+r, c, "%01X %01X %01X %01X",
		c8_keypad[0xA], c8_keypad[0x0], c8_keypad[0xB], c8_keypad[0xF]);
	
	refresh();
}

void handle_exception(){
	char exmsg[32];
	switch(c8_ex){
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
	mvprintw(20, 30, "%s", exmsg);
	refresh();
	c8_pause = 1;
}
