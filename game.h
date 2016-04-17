#ifndef GAME_H_
#define GAME_H_
#include <SDL2/SDL.h>

#define array_count(x) (sizeof(x) / sizeof(*x))

#define MAX(a,b) ((a) >  (b) ? (a) : (b))
#define MIN(a,b) ((a) <= (b) ? (a) : (b))

#define WIN_WIDTH  832
#define WIN_HEIGHT 480

extern SDL_Window* win;
extern SDL_Renderer* renderer;

void game_init(void);
void game_update(int);
void game_draw(void);

#endif
