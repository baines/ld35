#include <stdbool.h>
#include <float.h>
#include <SDL2/SDL.h>
#include "ld35.h"

typedef struct {
	Sprite* sprite;
	int speed;
	int anim_timer;

	bool is_bat;

	float x_vel;
	float y_vel;

	int bat_timer;
	int cooldown;

	float x_remainder;
	float y_remainder;

} Player;

static Player player = {
	.sprite = sprites,
	.speed = 2,
	.is_bat = false,
};

static int screen_shake_timer;
static int screen_shake_amount;

static bool has_landed = false;

#define PLAYER_TERMINAL_VEL 8.0f
#define BAT_TERMINAL_VEL 4.0f
#define PLAYER_ACCEL 0.2f
#define PLAYER_MAX_IMPULSE 12.0f
#define PLAYER_MAX_IMPULSE_BAT 15.0f
#define BAT_BAR_WIDTH 100
#define BAT_BAR_HEIGHT 20
#define BAT_TIMER_MAX 2000

void game_init(void){

	// player
	sprite_push_tex(400, 200, 64, 64, "data/vamp.png");
	sprites[0].radius = 16;
	sprites[0].hit_box_scale_x = 0.45f;
	sprites[0].hit_box_scale_y = 1.0f;

	room_init();

	room_load(4);

	SDL_Point p = room_get_spawn();
	player.sprite->x = p.x;
	player.sprite->y = p.y;
}

static void game_screen_shake(int duration, int amount){
	screen_shake_timer = duration;
	screen_shake_amount = amount;
}

static void game_do_player_death(void){

	SDL_Point pos = sprite_get_center(player.sprite);
	particles_spawn(pos, 0.f, 30.0f, 100); 

	if(player.is_bat){
		sound_play("data/bat.ogg", 0);
	}
	sound_play("data/die.ogg", 0);

	SDL_Point p = room_get_spawn();
	player.sprite->x = p.x;
	player.sprite->y = p.y;

	game_screen_shake(250, 10);
}

void game_update(int delta){

//	printf("delta: %d\n", delta);

	particles_update(delta);
	room_update(delta);

	const uint8_t* keys = SDL_GetKeyboardState(NULL);
	Sprite* player_s = player.sprite;

	if(player.is_bat){
		player_s->hit_box_scale_x = 0.5f;
		player_s->hit_box_scale_y = 0.5f;
	} else {
		player_s->hit_box_scale_x = 0.5f;
		player_s->hit_box_scale_y = 1.0f;
	}

	const float accel   = (PLAYER_ACCEL * (delta / 16.0f));
	const float max_imp = player.is_bat ? PLAYER_MAX_IMPULSE_BAT : PLAYER_MAX_IMPULSE;

	bool moving_on_up = false;

	if(keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]){
		if(player.is_bat){
			player.y_vel = MAX(
				player.y_vel - (accel * 1.2f),
				-(max_imp * accel * 1.2f)
			);
			printf("YV: %.2f\n", player.y_vel);
			moving_on_up = true;
		}
	}
	
	if(keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]){
		if(player.is_bat){
			player.y_vel = MIN(
				player.y_vel + accel,
				max_imp * accel
			);
		}
	} else if(player.is_bat && !moving_on_up){
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
	} else if(keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]){
		player.x_vel = MIN(
			player.x_vel + accel,
			max_imp * accel
		);
		player_s->flip_mode = SDL_FLIP_NONE;
	} else {
		int sign = player.x_vel > 0 ? 1 : -1;

		if(abs(player.x_vel) < 0.01f){
			player.x_vel = 0;
		} else {
			player.x_vel -= MIN(abs(player.x_vel), accel) * sign;
		}
	}

	if(player.cooldown > 0){
		player.cooldown -= delta;
	}

	if(player.bat_timer){
		player.bat_timer -= delta;
		if(player.bat_timer <= 0){
			game_screen_shake(100, 4);
			
			player.bat_timer = 0;
			player.is_bat = false;

			player.cooldown = 200;
			
			sound_play("data/unshapeshift.ogg", 0);
			sprite_set_tex(player_s, "data/vamp.png", 0);

			// XXX: hack to stop clipping through walls :/
			int x_hack = player_s->x < 32 ? 14 : player_s->x > (WIN_WIDTH-32) ? -14 : 0; 
			int y_hack = player_s->y < 32 ? 14 : player_s->y > (WIN_HEIGHT-32) ? -14 : 0; 
			player_s->x += x_hack;
			player_s->y += y_hack;
			player.x_vel = player.y_vel = 0;
			player.x_remainder = player.y_remainder = 0;
		}
	}

	if(has_landed && !player.bat_timer && keys[SDL_SCANCODE_SPACE]){
		if(player.cooldown > 0){
			sound_play("data/nope.ogg", 0);
		} else {
			sound_play("data/bat.ogg", 0);
			sprite_set_tex(player_s, "data/bat.png", 0);
			player.is_bat = true;
			player.bat_timer = BAT_TIMER_MAX;
			player.y_vel = 0;
			has_landed = false;
		}
	}

	float grav = player.is_bat ? ((float)delta / 18.0f) : ((float)delta / 16.0f);
	float tv   = player.is_bat ? BAT_TERMINAL_VEL : PLAYER_TERMINAL_VEL;

	if(!player.is_bat || !moving_on_up){
		if(player.is_bat){
			printf("grav: %.2f, %.2f, %.2f\n", tv, player.y_vel, grav);
		}

		player.y_vel = MIN(tv, player.y_vel + grav);
	}

	// animation
	
	if(player.is_bat || fabs(player.x_vel) > FLT_EPSILON){
		player.anim_timer += delta;

		if(player.anim_timer > 200){
			player_s->cur_frame = (player_s->cur_frame + 1) % player_s->num_frames;
			player.anim_timer = 0;
		}
	} else {
		player_s->cur_frame = 0;
	}

	// collision with tiles

	SDL_Rect hit_box = sprite_get_hit_box(player_s);
	SDL_Rect x_rect  = hit_box;
	SDL_Rect y_rect  = hit_box;

	x_rect.x += (player.x_vel + player.x_remainder);
	y_rect.y += (player.y_vel + player.y_remainder);

	bool collision_x = false;
	bool collision_y = false;
	bool should_kill = false;

	// meh just loop over everything...
	for(int i = 1; i < num_sprites; ++i){
		if(sprites[i].collision_type == COLLISION_BOX){

			SDL_Rect s_rect = sprite_get_hit_box(sprites + i);
			SDL_Rect intersect;

			/*
			SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
			SDL_RenderFillRect(renderer, &hit_box);
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderDrawRect(renderer, &x_rect);
			SDL_RenderDrawRect(renderer, &y_rect);
			*/

			if(SDL_IntersectRect(&x_rect, &s_rect, &intersect)){
				collision_x = true;

				if(sprites[i].collision_response == CRESP_KILL){
					should_kill = true;
					break;
				}

				if(sprites[i].collision_response == CRESP_POWERUP){
					room_get_powerup(i);

					if(player.is_bat){
						player.bat_timer = BAT_TIMER_MAX;
					}
				} else {
					player_s->x += (player.x_vel + player.x_remainder);
					player.x_vel = 0;
					player.x_remainder = 0;
					if(intersect.x <= x_rect.x){
						player_s->x += intersect.w;
						x_rect.x += intersect.w;
					} else {
						player_s->x -= intersect.w;
						x_rect.x -= intersect.w;
					}
				}
			}

			if(SDL_IntersectRect(&y_rect, &s_rect, &intersect)){
				collision_y = true;

				if(sprites[i].collision_response == CRESP_KILL){
					should_kill = true;
					break;
				}

				if(sprites[i].collision_response == CRESP_POWERUP){
					
					room_get_powerup(i);
					
					if(player.is_bat){
						player.bat_timer = BAT_TIMER_MAX;
					}
				} else {

					if(!player.is_bat && player.cooldown <= 0 && player.y_vel > 0){
						has_landed = true;
					}

					player_s->y += player.y_vel; 
					player.y_vel = 0;
					player.y_remainder = 0;
					if(intersect.y <= y_rect.y){
						player_s->y += intersect.h;
						y_rect.y += intersect.h;
						x_rect.y += intersect.h + 1;
					} else {
						player_s->y -= intersect.h;
						y_rect.y -= intersect.h;
						x_rect.y -= intersect.h + 1;
					}
				}
				//printf("YI: %d %d %d, POS: %d\n", intersect.y, player.sprite->y, intersect.h, player_s->y);
			}

		}
	}

	if(should_kill){
		game_do_player_death();
	} else {

		if(!collision_x){
			float x_vel_i;
			float x_vel_f = modff(player.x_vel, &x_vel_i);
			player_s->x += (player.x_vel + player.x_remainder);
			player.x_remainder = x_vel_f;
		}

		if(!collision_y){
			float y_vel_i;
			float y_vel_f = modff(player.y_vel, &y_vel_i);
			player_s->y += (player.y_vel + player.y_remainder);
			player.y_remainder = y_vel_f;
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
		puts("going up");
		room_switch(ROOM_UP);
		player_s->y = WIN_HEIGHT - (player_s->h + 1);
		printf("setting player y %d\n", player_s->y);
	}

	if(screen_shake_timer > 0){
		screen_shake_timer -= delta;

		if(screen_shake_timer > 0){
			SDL_Rect vp = viewport;
			vp.x += (rand() % screen_shake_amount) - (screen_shake_amount/2);
			vp.y += (rand() % screen_shake_amount) - (screen_shake_amount/2);
			SDL_RenderSetViewport(renderer, &vp);
		} else {
			screen_shake_timer = 0;
			SDL_RenderSetViewport(renderer, &viewport);
		}
	}
}

void game_draw(void){

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	SDL_RenderDrawRect(renderer, NULL);

	/*
	for(int i = 0; i < 32; ++i){
		SDL_RenderDrawLine(renderer, i * 32, 0, i * 32, WIN_HEIGHT);
	}

	for(int i = 0; i < 32; ++i){
		SDL_RenderDrawLine(renderer, 0, i * 32, WIN_WIDTH, i * 32);
	}
*/
	
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
			.w = BAT_BAR_WIDTH * (player.bat_timer / (float)BAT_TIMER_MAX),
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
