#include "interface.h"
#include "chip.h"

SDL_Window* gfx_win;
SDL_Renderer* render;
SDL_Texture* texture;
size_t scaling;

SDL_Keycode keypad_mapping[16] = {
	SDLK_x, SDLK_1, SDLK_2, SDLK_3,
	SDLK_q, SDLK_w, SDLK_e, SDLK_a,
	SDLK_s, SDLK_d, SDLK_z, SDLK_c,
	SDLK_4, SDLK_r, SDLK_f, SDLK_v
};

void interface_init(){
	scaling = 8;
	SDL_Init(SDL_INIT_VIDEO);
	gfx_win = SDL_CreateWindow("Chipical", 0, 0, c8_scrw*scaling, c8_scrh*scaling, SDL_WINDOW_SHOWN);
	render = SDL_CreateRenderer(gfx_win, -1, SDL_RENDERER_SOFTWARE);
	texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, c8_scrw, c8_scrh);
}

void interface_deinit(){
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
						break;
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
						break;
					}
				}
				//printf("release %d\n", kp);
			} break;
		}
	}

	return q;
}
