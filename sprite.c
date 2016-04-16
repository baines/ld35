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

void sprite_push_tex_frames(int x, int y, int w, int h, const char* name, int frames){
	if(num_sprites >= array_count(sprites)){
		return;
	}

	int comp_per_pixel;
	int actual_w, actual_h;

	void* pixels = stbi_load(name, &actual_w, &actual_h, &comp_per_pixel, 4);
	SDL_assert(pixels);

	printf("COMP PER PIXEL: %d\n", comp_per_pixel);

	SDL_Texture* tex = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STATIC,
		actual_w,
		actual_h
	);

	SDL_UpdateTexture(
		tex,
		NULL,
		pixels,
		comp_per_pixel * actual_w
	);

	free(pixels);

	if(!tex){
		fprintf(stderr, "wtf %s\n", SDL_GetError());
	}

	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

	sprite_push(x, y, w, h);
	sprites[num_sprites-1].tex = tex;

	if(frames){
		sprites[num_sprites-1].num_frames = frames;
	}
}

void sprite_push_tex(int x, int y, int w, int h, const char* name){
	sprite_push_tex_frames(x, y, w, h, name, 0);
}
