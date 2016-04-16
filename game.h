#ifndef GAME_H_
#define GAME_H_

#define array_count(x) (sizeof(x) / sizeof(*x))

struct SDL_Renderer;

void game_init(void);
void game_update(float);
void game_draw(SDL_Renderer*);

#endif
