#include "game.h"
#include "sprite.h"
#define STB_IMAGE_IMPLEMENTATION 1
#include "stb_image.h"

Sprite sprites[256];
int num_sprites;

void sprite_push(int x, int y, int w, int h){
	// black by default?
	sprite_push_col(x, y, w, h, 0xff);
}

void sprite_push_col(int x, int y, int w, int h, unsigned int color){
	if(num_sprites >= array_count(sprites)){
		return;
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
}

void sprite_push_tex(int x, int y, int w, int h, const char* name){
	if(num_sprites >= array_count(sprites)){
		return;
	}

	int comp_per_pixel;
	int actual_w, actual_h;

	void* pixels = stbi_load(name, &actual_w, &actual_h, &comp_per_pixel, 4);
	SDL_assert(pixels);


	printf("COMP PER PIXEL: %d\n", comp_per_pixel);

	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);

	if(!tex){
		fprintf(stderr, "wtf %s\n", SDL_GetError());
	}
	SDL_assert(tex);

	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

	SDL_FreeSurface(surface);

	sprite_push(x, y, w, h);
	sprites[num_sprites-1].tex = tex;
}
