#ifndef SPRITE_H_
#define SPRITE_H_
#include <SDL2/SDL.h>

typedef struct {
	union {
		SDL_Rect rect;
		struct {
			int x, y, w, h;
		};
	};

	SDL_Color color;
	SDL_Texture* tex;

	int num_frames;
	int cur_frame;

} Sprite;

// 256 should be enough for anyone...
extern Sprite sprites[256];
extern int num_sprites;

void sprite_push(int x, int y, int w, int h);
void sprite_push_col(int x, int y, int w, int h, unsigned int color);
void sprite_push_tex(int x, int y, int w, int h, const char* name);
void sprite_push_tex_frames(int x, int y, int w, int h, const char* name, int frames);

#endif
