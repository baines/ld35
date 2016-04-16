#include <SDL2/SDL.h>
#include "game.h"
#include "sprite.h"
#include "room.h"

typedef struct {
	Sprite* sprite;
	int speed;
	int anim_timer;
} Player;

static Player player = {
	.sprite = sprites,
	.speed = 2
};

void game_init(void){

	// player
//	sprite_push_tex_frames(400, 200, 128, 128, "data/vamp.png", 8);
	sprite_push_tex_frames(400, 200, 64, 64, "data/bat.png", 4);

	room_load(1);

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

	player.anim_timer += delta;

	if(player.anim_timer > 100){
		player_s->cur_frame = (player_s->cur_frame + 1) % player_s->num_frames;
		player.anim_timer = 0;
	}
}

void game_draw(void){

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	SDL_RenderDrawRect(renderer, NULL);

	for(int i = 0; i < 32; ++i){
		SDL_RenderDrawLine(renderer, i * 32, 0, i * 32, WIN_HEIGHT);
	}

	for(int i = 0; i < 32; ++i){
		SDL_RenderDrawLine(renderer, 0, i * 32, WIN_WIDTH, i * 32);
	}

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

			SDL_SetRenderTarget(renderer, NULL);
			SDL_RenderCopy(renderer, sprites[i].tex, src_rect_ptr, &sprites[i].rect);
		} else {
			SDL_RenderFillRect(renderer, &sprites[i].rect);
		}
	}
}
