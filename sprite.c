#include "game.h"
#include "sprite.h"
#include <SDL2/SDL_image.h>
#include <float.h>

// do numbers greater than 4096 exist?
Sprite sprites[MAX_SPRITES];
int num_sprites;

typedef struct {
	char* name;
	SDL_Texture* tex;
} TexCache;

TexCache tex_cache[128];

void sprite_set_col(Sprite* s, unsigned int color){
	SDL_Color c = {
		.r = ((color >> 24) & 0xFF),
		.g = ((color >> 16) & 0xFF),
		.b = ((color >> 8) & 0xFF),
		.a = ((color >> 0) & 0xFF),
	};
	s->color = c;
}

void sprite_set_tex(Sprite* s, const char* name, int frames){

	SDL_Texture* tex = NULL;
	for(int i = 0; i < array_count(tex_cache); ++i){
		if(tex_cache[i].name && strcmp(name, tex_cache[i].name) == 0){
			tex = tex_cache[i].tex;
			break;
		}
	}

	if(!tex){
		SDL_Surface* surf = IMG_Load(name);
		SDL_assert(surf);

		tex = SDL_CreateTextureFromSurface(renderer, surf);
		if(!tex){
			fprintf(stderr, "wtf %s\n", SDL_GetError());
		}

		SDL_FreeSurface(surf);

		SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
	}

	if(!frames){
		int tw, th;
		SDL_QueryTexture(tex, NULL, NULL, &tw, &th);
		frames = tw / th;
		printf("auto frames: %s %d\n", name, frames);
	}

	s->cur_frame = 0;
	s->num_frames = frames;

	s->tex = tex;
}

Sprite* sprite_push(int x, int y, int w, int h){
	// black by default?
	return sprite_push_col(x, y, w, h, 0xff);
}

Sprite* sprite_push_col(int x, int y, int w, int h, unsigned int color){
	if(num_sprites >= array_count(sprites)){
		return NULL;
	}

	Sprite s = {
		.x = x,
		.y = y,
		.w = w,
		.h = h,
	};

	sprites[num_sprites] = s;
	sprite_set_col(sprites + num_sprites, color);

	num_sprites++;

	return sprites + (num_sprites - 1);
}

Sprite* sprite_push_tex_frames(int x, int y, int w, int h, const char* name, int frames){
	if(num_sprites >= array_count(sprites)){
		return NULL;
	}

	sprite_push(x, y, w, h);
	sprite_set_tex(sprites + num_sprites - 1, name, frames);

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

void sprite_draw(Sprite* s){
	SDL_SetRenderDrawColor(
		renderer,
		s->color.r,
		s->color.g,
		s->color.b,
		s->color.a
	);

	if(s->tex){
		SDL_Rect src_rect;

		SDL_Rect* src_rect_ptr = NULL;

		if(s->num_frames){
			int tw, th;
			SDL_QueryTexture(s->tex, NULL, NULL, &tw, &th);

			src_rect.w = (tw / s->num_frames);
			src_rect.x = src_rect.w * s->cur_frame;
			src_rect.h = th;
			src_rect.y = 0;

			src_rect_ptr = &src_rect;
		}

		SDL_SetRenderTarget(renderer, NULL);
		SDL_RenderCopyEx(renderer, s->tex, src_rect_ptr, &s->rect, 0, NULL, s->flip_mode);
	} else {
		SDL_RenderFillRect(renderer, &s->rect);
	}
}

void sprite_pop(Sprite* start, int num){
	// probably need a proper memove here...
	num_sprites -= num;
	if(num_sprites < 0){
		num_sprites = 0;
	}
}

SDL_Rect sprite_get_hit_box(Sprite* s){

	if(fabs(s->hit_box_scale) < 0.001f){
		return s->rect;
	}

//	SDL_Point p = sprite_get_center(s);
	float scale = s->hit_box_scale, half_scale = scale / 2.0f;

	SDL_Rect ret = {
		s->x + (half_scale * s->w),
		s->y + (half_scale * s->h),
		s->w * scale,
		s->h * scale,
	};

	return ret;
}
