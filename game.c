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

	bool is_bat;

	float x_vel;
	float y_vel;

	int bat_timer;

} Player;

static Player player = {
	.sprite = sprites,
	.speed = 2,
	.is_bat = false,
};

#define PLAYER_TERMINAL_VEL 12.0f
#define PLAYER_ACCEL 0.2f
#define PLAYER_MAX_IMPULSE 12.0f
#define PLAYER_MAX_IMPULSE_BAT 15.0f
#define BAT_BAR_WIDTH 100
#define BAT_BAR_HEIGHT 20

void game_init(void){

	// player
	sprite_push_tex(400, 200, 64, 64, "data/vamp.png");
	sprites[0].radius = 16;
	sprites[0].hit_box_scale_x = 0.5f;
	sprites[0].hit_box_scale_y = 0.5f;

	room_init();

	room_load(1);
}

static void game_do_player_death(void){

	SDL_Point pos = sprite_get_center(player.sprite);
	particles_spawn(pos, 0.f, 10.0f, 100); 

	player.sprite->x = (WIN_WIDTH / 2);
	player.sprite->y = (WIN_HEIGHT / 2);
}

void game_update(int delta){

//	printf("delta: %d\n", delta);

	particles_update(delta);

	const uint8_t* keys = SDL_GetKeyboardState(NULL);
	Sprite* player_s = player.sprite;

	if(player.is_bat){
		player_s->hit_box_scale_x = player_s->hit_box_scale_y = 0.5f;
	} else {
		player_s->hit_box_scale_x = 0.5f;
		player_s->hit_box_scale_y = 1.0f;
	}

	const float accel   = (PLAYER_ACCEL * (delta / 16.0f));
	const float max_imp = player.is_bat ? PLAYER_MAX_IMPULSE_BAT : PLAYER_MAX_IMPULSE;
	bool pressed = false;

	if(keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]){
		if(player.is_bat){
			player.y_vel = MAX(
				player.y_vel - accel,
				-(max_imp * accel)
			);
		}
		pressed = true;
	} else if(keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]){
		if(player.is_bat){
			player.y_vel = MIN(
				player.y_vel + accel,
				max_imp * accel
			);
		}
		pressed = true;
	} else if(player.is_bat){
		int sign = player.y_vel > 0 ? 1 : -1;

		if(abs(player.y_vel) < 0.01f){
			player.y_vel = 0;
		} else {
			player.y_vel -= MIN(abs(player.y_vel), accel / 2.0f) * sign;
		}
	}

	if(keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]){
		player.x_vel = MAX(
			player.x_vel - accel,
			-(max_imp * accel)
		);
		player_s->flip_mode = SDL_FLIP_HORIZONTAL;
		pressed = true;
	} else if(keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]){
		player.x_vel = MIN(
			player.x_vel + accel,
			max_imp * accel
		);
		player_s->flip_mode = SDL_FLIP_NONE;
		pressed = true;
	} else {
		int sign = player.x_vel > 0 ? 1 : -1;

		if(abs(player.x_vel) < 0.01f){
			player.x_vel = 0;
		} else {
			player.x_vel -= MIN(abs(player.x_vel), accel) * sign;
		}
	}

	if(player.bat_timer > 0){
		player.bat_timer -= delta;
		if(player.bat_timer <= 0){
			player.bat_timer = 0;
			player.is_bat = false;
			sprite_set_tex(player_s, "data/vamp.png", 0);
		}
	}

	if(!player.bat_timer && keys[SDL_SCANCODE_SPACE]){
		sprite_set_tex(player_s, "data/bat.png", 0);
		player.is_bat = true;
		player.bat_timer = 2000;
		player.y_vel = 0;
	}

	if(!player.is_bat){
		if(player.y_vel < PLAYER_TERMINAL_VEL){
			player.y_vel = MAX(PLAYER_TERMINAL_VEL, player.y_vel + (float)delta / 16.0f);
		} else {
			player.y_vel = PLAYER_TERMINAL_VEL;
		}
	}

	// animation
	
	if(player.is_bat || pressed){
		player.anim_timer += delta;

		if(player.anim_timer > 200){
			player_s->cur_frame = (player_s->cur_frame + 1) % player_s->num_frames;
			player.anim_timer = 0;
		}
	} else {
		player_s->cur_frame = 0;
	}

	// collision with tiles

	SDL_Rect x_rect = sprite_get_hit_box(player_s), y_rect = x_rect;

	x_rect.x += player.x_vel;
	y_rect.y += player.y_vel;

	bool collision_x = false;
	bool collision_y = false;
	bool should_kill = false;

	// meh just loop over everything...
	for(int i = 1; i < num_sprites; ++i){
		if(sprites[i].collision_type == COLLISION_BOX){

			SDL_Rect s_rect = sprite_get_hit_box(sprites + i);
			SDL_Rect intersect;

//			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
//			SDL_RenderDrawRect(renderer, &s_rect);

			if(!collision_x && SDL_IntersectRect(&x_rect, &s_rect, &intersect)){
				collision_x = true;

				if(sprites[i].collision_response == CRESP_KILL){
					should_kill = true;
					break;
				}

				player_s->x += player.x_vel;
				player.x_vel = 0;
				if(intersect.x <= x_rect.x){
					player_s->x += intersect.w;
				} else {
					player_s->x -= intersect.w;
				}

				//printf("X POS: %d\n", player_s->x);
			}

			if(!collision_y && SDL_IntersectRect(&y_rect, &s_rect, &intersect)){
				collision_y = true;

				if(sprites[i].collision_response == CRESP_KILL){
					should_kill = true;
					break;
				}

				player_s->y += player.y_vel;
				player.y_vel = 0;
				if(intersect.y <= y_rect.y){
					player_s->y += intersect.h;
				} else {
					player_s->y -= intersect.h;
				}

				//printf("YI: %d %d %d, POS: %d\n", intersect.y, player.sprite->y, intersect.h, player_s->y);
			}

		}
	}

	if(should_kill){
		game_do_player_death();
	} else {

		if(!collision_x){
			player_s->x += player.x_vel;
		}

		if(!collision_y){
			player_s->y += player.y_vel;
		}
	}
	
	// collision with edges of screen

	SDL_Point p = sprite_get_center(player_s);
	if(p.x > WIN_WIDTH){
		room_switch(ROOM_RIGHT);
		player_s->x = 1;
		printf("setting player x %d\n", player_s->x);
	} else if(p.x < 0){
		room_switch(ROOM_LEFT);
		player_s->x = WIN_WIDTH - (player_s->w + 1);
		printf("setting player x %d\n", player_s->x);
	}

	if(p.y > WIN_HEIGHT){
		room_switch(ROOM_DOWN);
		player_s->y = 1;
		printf("setting player y %d\n", player_s->y);
	} else if(p.y < 0){
		room_switch(ROOM_UP);
		player_s->y = WIN_HEIGHT - (player_s->h + 1);
		printf("setting player y %d\n", player_s->y);
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

	for(int i = 1; i < num_sprites; ++i){
		sprite_draw(sprites + i);
	}

	particles_draw();

	// draw player on top of stuff
	sprite_draw(sprites);

	if(player.bat_timer){
		SDL_Rect bar_rect = {
			.x = (WIN_WIDTH / 2) - (BAT_BAR_WIDTH / 2),
			.y = 400,
			.w = BAT_BAR_WIDTH * (player.bat_timer / 2000.0f),
			.h = BAT_BAR_HEIGHT,
		};

		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		SDL_RenderFillRect(renderer, &bar_rect);

		bar_rect.w = BAT_BAR_WIDTH;

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(renderer, &bar_rect); 
	}

	//debug
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_Rect r = sprite_get_hit_box(sprites);
	SDL_RenderDrawRect(renderer, &r);
}
