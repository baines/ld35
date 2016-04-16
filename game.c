#include <stdbool.h>
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

	SDL_Point move = { .x = player_s->x, .y = player_s->y };

	if(keys[SDL_SCANCODE_W]){
		move.y -= player.speed;
	}
	if(keys[SDL_SCANCODE_S]){
		move.y += player.speed;
	}
	if(keys[SDL_SCANCODE_A]){
		move.x -= player.speed;
	}
	if(keys[SDL_SCANCODE_D]){
		move.x += player.speed;
	}

	/*
	SDL_Rect move_rect = player_s->rect;
	move_rect.x = move.x;
	move_rect.y = move.y;
*/

	SDL_Rect x_rect = player_s->rect;
	SDL_Rect y_rect = player_s->rect;

	x_rect.x = move.x;
	y_rect.y = move.y;

	bool collision_x = false;
	bool collision_y = false;

	// meh just loop over everything...
	for(int i = 1; i < num_sprites; ++i){
		switch(sprites[i].collision_type){
			case COLLISION_BOX: {
				if(SDL_HasIntersection(&x_rect, &sprites[i].rect)){
					collision_x = true;
					break;
				}
				if(SDL_HasIntersection(&y_rect, &sprites[i].rect)){
					collision_y = true;
					break;
				}
			} break;
		}
	}

	if(!collision_x){
		player_s->x = move.x;
	}

	if(!collision_y){
		player_s->y = move.y;
	}

	player.anim_timer += delta;

	if(player.anim_timer > 100){
		player_s->cur_frame = (player_s->cur_frame + 1) % player_s->num_frames;
		player.anim_timer = 0;
	}
}

static void game_draw_sprite(int i){
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
	for(int i = 1; i < num_sprites; ++i){
		game_draw_sprite(i);
	}

	// draw player on top of stuff
	game_draw_sprite(0);
}
