#include "game.h"
#include "sprite.h"
#include <SDL2/SDL_image.h>

// do numbers greater than 4096 exist?
Sprite sprites[MAX_SPRITES];
int num_sprites;

Sprite* sprite_push(int x, int y, int w, int h){
	// black by default?
	return sprite_push_col(x, y, w, h, 0xff);
}

Sprite* sprite_push_col(int x, int y, int w, int h, unsigned int color){
	if(num_sprites >= array_count(sprites)){
		return NULL;
	}

	SDL_Color c = {
		.r = ((color >> 24) & 0xFF),
		.g = ((color >> 16) & 0xFF),
		.b = ((color >> 8) & 0xFF),
		.a = ((color >> 0) & 0xFF),
	};

	Sprite s = {
		.x = x,
		.y = y,
		.w = w,
		.h = h,
		.color = c
	};

	sprites[num_sprites++] = s;

	return sprites + (num_sprites - 1);
}

Sprite* sprite_push_tex_frames(int x, int y, int w, int h, const char* name, int frames){
	if(num_sprites >= array_count(sprites)){
		return NULL;
	}

	SDL_Surface* surf = IMG_Load(name);
	SDL_assert(surf);

	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
	if(!tex){
		fprintf(stderr, "wtf %s\n", SDL_GetError());
	}

	SDL_FreeSurface(surf);

	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

	sprite_push(x, y, w, h);
	sprites[num_sprites-1].tex = tex;

	if(frames){
		sprites[num_sprites-1].num_frames = frames;
	}

	return sprites + (num_sprites - 1);
}

Sprite* sprite_push_tex(int x, int y, int w, int h, const char* name){
	return sprite_push_tex_frames(x, y, w, h, name, 0);
}

SDL_Point sprite_get_center(Sprite* s){
	SDL_Point p = {
		s->x + (s->w / 2),
		s->y + (s->h / 2)
	};

	return p;
}
