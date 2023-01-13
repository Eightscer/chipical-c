# chipical
CHIP8 emulator with ncurses (for debugging information on terminal) and SDL2 (for graphics). Developed for Linux.

Passes corax89's and BestCoder's test ROMs.

## Running

Simply run `make` in the root directory of the repo. You will need ncurses and SDL2 runtime libraries installed.

The compiled binary should appear in the `/bin` folder. The program takes one argument: the filepath of the CHIP8 ROM you want to run.

E.g. `./bin/chipical /path/to/rom.ch8`

Standard keypad mappings. {X, 1, 2, 3, Q, W, E, A, S, D, Z, C, 4, R, F, V} maps to {0, 1, 2, ... D, E, F}, respectively.

Pressing P will toggle between pausing and continuing execution. Pressing ESC will quit the program (when the SDL window is focused).

Holding L will speed up emulation, while holding K will slow down emulation. This also scales the rate at which the delay and sound timers count down appropriately.

The emulator will halt when an 'exception' occurs, with the only option being to press ESC to quit. An exception occurs when the program is invoking strange or unintended behavior (e.g. stack overflow, unknown opcode, index out of bounds, program counter out of bounds).

## Configuration

Currently, there is no way to configure the emulator's options at runtime. If you insist however, it shouldn't be too difficult to modify the source code to make some changes to your liking:

`interface.c`:
- `emu->gfx_scaling`: Scaling factor of the graphics and SDL window
- `emu->cycles_until_update`: Amount of cycles to be run before updating the debugging window
- `default_keypad_mapping`: Mapping of CHIP8 keypad to computer keycodes

`chip.c`:
- `c8->cycles_per_sec`: Rate at which the emulator runs. Most older implementations run at or below 1000 MHz, but this number isn't necessarily standard.
- `c8->color0`: RGBA value for background color.
- `c8->color1`: RGBA value for foreground color (sprites).

## Todo

### Likely

- Save states
- Configuration file support (JSON?)
- Configuration menu using ncurses

### Unlikely

- Disassembler / assembler
- SuperChip48 support
- Pedantic compatibility options (e.g. different opcode behavior for older machines)
- Sound
- Rewinding

## Current Issues

- The debugger window consumes a lot of CPU usage while active (about 4-5 times more than the interpreter itself according to `htop`), even after modifying the code to only print when changes are made.
- Speed scaling seems slightly off, it appears empirically that the scaling factor is about 1 off than intended (i.e. when fast forwarding with a scale of 4.0, it will appear to run only 3 times faster instead of 4).
- Random memory leaks (?) and segfaults everywhere
