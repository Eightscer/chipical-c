#ifndef INTERFACE_H
#define INTERFACE_H

#include <ncurses.h>
#include <SDL2/SDL.h>

//SDL_Window* gfx_win;
//SDL_Renderer* render;
//SDL_Texture* texture;

WINDOW* new_win(int h, int w, int sy, int sx);
void rm_win(WINDOW* win);
void update_debug();
void init_debug();

extern SDL_Keycode keypad_mapping[16];
extern size_t scaling;
extern uint32_t cycles_until_update;
extern WINDOW* debug_win;
extern int debug_enable;

void interface_init();
void interface_deinit();

void update_gfx();
int keypad_input(uint8_t* keypad);


#endif
