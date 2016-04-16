#include <SDL2/SDL.h>
#include "stb_image.h"
#include "game.h"
#include "sprite.h"

typedef struct {
	Sprite* sprite;
	int speed;
} Player;

static Player player = {
	.sprite = sprites,
	.speed = 2
};

static int anim_timer;

void game_init(void){

	// player
	sprite_push_tex_frames(400, 200, 128, 128, "data/bat.png", 4);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void game_update(int delta){

	const uint8_t* keys = SDL_GetKeyboardState(NULL);
	Sprite* player_s = player.sprite;

	if(keys[SDL_SCANCODE_W]){
		player_s->y -= player.speed;
	}

	if(keys[SDL_SCANCODE_S]){
		player_s->y += player.speed;
	}

	if(keys[SDL_SCANCODE_A]){
		player_s->x -= player.speed;
	}

	if(keys[SDL_SCANCODE_D]){
		player_s->x += player.speed;
	}

	anim_timer += delta;

	if(anim_timer > 100){
		player_s->cur_frame = (player_s->cur_frame + 1) % player_s->num_frames;
		anim_timer = 0;
	}
}

void game_draw(void){

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	for(int i = 0; i < num_sprites; ++i){

		SDL_SetRenderDrawColor(
			renderer,
			sprites[i].color.r,
			sprites[i].color.g,
			sprites[i].color.b,
			sprites[i].color.a
		);
		
		if(sprites[i].tex){
			SDL_Rect src_rect;

			SDL_Rect* src_rect_ptr = NULL;

			if(sprites[i].num_frames){
				int tw, th;
				SDL_QueryTexture(sprites[i].tex, NULL, NULL, &tw, &th);

				src_rect.w = (tw / sprites[i].num_frames);
				src_rect.x = src_rect.w * sprites[i].cur_frame;
				src_rect.h = th;
				src_rect.y = 0;

				src_rect_ptr = &src_rect;
			}

			SDL_RenderCopy(renderer, sprites[i].tex, src_rect_ptr, &sprites[i].rect);
		} else {
			SDL_RenderFillRect(renderer, &sprites[i].rect);
		}
	}
}
