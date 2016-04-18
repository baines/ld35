#ifndef LD35_H_
#define LD35_H_
#include <SDL2/SDL.h>

// main

#define array_count(x) (sizeof(x) / sizeof(*x))

#define MAX(a,b) ((a) >  (b) ? (a) : (b))
#define MIN(a,b) ((a) <= (b) ? (a) : (b))

#define WIN_WIDTH  832
#define WIN_HEIGHT 480

extern SDL_Window* win;
extern SDL_Renderer* renderer;
extern SDL_Rect viewport;

void game_init(void);
void game_update(double);
void game_draw(void);

// sprites

enum {
	COLLISION_NONE,
	COLLISION_BOX,
	COLLISION_CIRCLE,
	//TODO: triangles
};

enum {
	CRESP_BLOCK,
	CRESP_KILL,
	CRESP_POWERUP,
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

	float hit_box_scale_x;
	float hit_box_scale_y;
	float radius;

	int flip_mode;
	int rotation;

	int respawn_timer;

} Sprite;

#define MAX_SPRITES 400

extern Sprite sprites[MAX_SPRITES];
extern int num_sprites;

void sprite_set_col(Sprite* s, unsigned int color);
void sprite_set_tex(Sprite* s, const char* name, int frames);

Sprite* sprite_push(int x, int y, int w, int h);
Sprite* sprite_push_col(int x, int y, int w, int h, unsigned int color);
Sprite* sprite_push_tex(int x, int y, int w, int h, const char* name);
Sprite* sprite_push_tex_frames(int x, int y, int w, int h, const char* name, int frames);

SDL_Point sprite_get_center(Sprite* s);

void sprite_draw(Sprite* s);

void sprite_pop(Sprite* start, int num);

SDL_Rect sprite_get_hit_box(Sprite* s);

// particles

void particles_spawn(SDL_Point pos, float xv, float yv, int amount);
void particles_update(double delta);
void particles_draw(void);

// room

enum {
	ROOM_UP,
	ROOM_DOWN,
	ROOM_LEFT,
	ROOM_RIGHT,
};

void room_init(void);
void room_load(int number);
void room_update(double delta);
//void room_draw();

void room_switch(int which);
void room_get_powerup(int index);
SDL_Point room_get_spawn(void);
// sound

void sound_init(void);
void sound_play(const char* name, int loops);

#endif
