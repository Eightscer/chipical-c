#ifndef INTERFACE_H
#define INTERFACE_H

//#include <ncurses.h>
#include <SDL2/SDL.h>

//SDL_Window* gfx_win;
//SDL_Renderer* render;
//SDL_Texture* texture;

extern SDL_Keycode keypad_mapping[16];
extern size_t scaling;

void interface_init();
void interface_deinit();

void update_gfx();
int keypad_input(uint8_t* keypad);


#endif
