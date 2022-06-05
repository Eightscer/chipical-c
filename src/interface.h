#ifndef INTERFACE_H
#define INTERFACE_H

#include <string.h>
#include "chip.h"
#include <ncurses.h>
#include <SDL2/SDL.h>

//SDL_Window* gfx_win;
//SDL_Renderer* render;
//SDL_Texture* texture;

typedef struct {
	// Graphics
	SDL_Window* gfx_win;
	SDL_Renderer* render;
	SDL_Texture* texture;
	SDL_Keycode prev_pressed, prev_release;
	SDL_Keycode keypad_mapping[16];
	uint32_t gfx_scaling;
	// Debugging
	uint32_t cycles_until_update;
	int debug_enable;
	WINDOW* debug_win;
	chip8_system* c8;
	chip8_state prev_state;
	uint8_t change[8];
	// Control
	int debug_toggle;
	int pause_rel, debug_rel;
	int ff_hold, slow_hold;
	float speed_scaling, speed_factor;
	float orig_freq;
	uint32_t orig_cpc;
	
} chip8_emulator;

WINDOW* new_win(int h, int w, int sy, int sx);
void rm_win(WINDOW* win);
void update_debug(chip8_emulator* emu);
void update_prev_state(chip8_emulator* emu);
void init_debug(chip8_emulator* emu);
void handle_exception(chip8_emulator* emu);

/*
extern SDL_Keycode keypad_mapping[16];
extern size_t scaling;
extern uint32_t cycles_until_update;
extern WINDOW* debug_win;
extern int debug_enable;
extern int c8_pause;
*/

int interface_init(chip8_emulator* emu, char* filename);
void interface_deinit(chip8_emulator* emu);

void update_gfx(chip8_emulator* emu);
int keypad_input(chip8_emulator* emu);

void rescale_speed(chip8_emulator* emu, float factor);


#endif
