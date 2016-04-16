#ifndef GAME_H_
#define GAME_H_
#include <SDL2/SDL.h>

#define array_count(x) (sizeof(x) / sizeof(*x))

extern SDL_Window* win;
extern SDL_Renderer* renderer;

void game_init(void);
void game_update(int);
void game_draw(void);

#endif
