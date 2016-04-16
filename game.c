#include <stdbool.h>
#include <SDL2/SDL.h>
#include "game.h"
#include "sprite.h"
#include "room.h"
#include "particles.h"

typedef struct {
	Sprite* sprite;
	int speed;
	int anim_timer;
} Player;

static Player player = {
	.sprite = sprites,
	.speed = 2,
};

void game_init(void){

	// player
//	sprite_push_tex_frames(400, 200, 128, 128, "data/vamp.png", 8);
	sprite_push_tex_frames(400, 200, 64, 64, "data/bat.png", 4);
	sprites[0].radius = 16;
	sprites[0].hit_box_scale = 0.5f;

	room_load(1);
}

static void game_do_player_death(void){

	SDL_Point pos = sprite_get_center(player.sprite);
	particles_spawn(pos, 0.f, 10.0f, 100); 

	player.sprite->x = (WIN_WIDTH / 2);
	player.sprite->y = (WIN_HEIGHT / 2);
}

void game_update(int delta){

	particles_update(delta);

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

	// collision with edges of screen

	SDL_Point p = sprite_get_center(player_s);
	if(p.x > WIN_WIDTH){
		room_switch(ROOM_RIGHT);
		player_s->x = 16;
	} else if(p.x < 0){
		room_switch(ROOM_LEFT);
		player_s->x = WIN_WIDTH - 16;
	}

	if(p.y > WIN_HEIGHT){
		room_switch(ROOM_DOWN);
		player_s->y = 16;
	} else if(p.y < 0){
		room_switch(ROOM_UP);
		player_s->y = WIN_HEIGHT - 16;
	}

	// collision with tiles

	SDL_Rect x_rect = sprite_get_hit_box(player_s), y_rect = x_rect;

	x_rect.x += (move.x - player_s->x);
	y_rect.y += (move.y - player_s->y);

	bool collision_x = false;
	bool collision_y = false;

	int collision_response = 0;

	// meh just loop over everything...
	for(int i = 1; i < num_sprites; ++i){
		switch(sprites[i].collision_type){
			case COLLISION_BOX: {

				SDL_Rect s_rect = sprite_get_hit_box(sprites + i);

				if(SDL_HasIntersection(&x_rect, &s_rect)){
					collision_x = true;
					collision_response = sprites[i].collision_response;
					break;
				}
				if(SDL_HasIntersection(&y_rect, &s_rect)){
					collision_y = true;
					collision_response = sprites[i].collision_response;
					break;
				}
			} break;
		}
	}

	if((collision_y || collision_x) && collision_response == CRESP_KILL){
		game_do_player_death();
	} else {

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
		sprite_draw(sprites + i);
	}

	particles_draw();

	// draw player on top of stuff
	sprite_draw(sprites);

	//debug
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_Rect r = sprite_get_hit_box(sprites);
	SDL_RenderDrawRect(renderer, &r);
}
