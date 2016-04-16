#ifndef SPRITE_H_
#define SPRITE_H_
#include <SDL2/SDL.h>

enum {
	COLLISION_NONE,
	COLLISION_BOX,
	COLLISION_CIRCLE,
	//TODO: triangles
};

enum {
	CRESP_BLOCK,
	CRESP_KILL,
};

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

	int collision_type;
	int collision_response;

	float hit_box_scale;
	float radius;

} Sprite;

#define MAX_SPRITES 4096

extern Sprite sprites[MAX_SPRITES];
extern int num_sprites;

void sprite_set_col(Sprite* s, unsigned int color);

Sprite* sprite_push(int x, int y, int w, int h);
Sprite* sprite_push_col(int x, int y, int w, int h, unsigned int color);
Sprite* sprite_push_tex(int x, int y, int w, int h, const char* name);
Sprite* sprite_push_tex_frames(int x, int y, int w, int h, const char* name, int frames);

SDL_Point sprite_get_center(Sprite* s);

void sprite_draw(Sprite* s);

void sprite_pop(Sprite* start, int num);

SDL_Rect sprite_get_hit_box(Sprite* s);

#endif
