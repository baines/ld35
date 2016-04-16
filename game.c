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

void game_init(void){

	// player
	sprite_push_tex(400, 200, 32*4, 32, "data/bat.png");
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void game_update(float delta){

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

//		SDL_RenderFillRect(renderer, &sprites[i].rect);
		if(sprites[i].tex){
			SDL_RenderCopy(renderer, sprites[i].tex, NULL, &sprites[i].rect);
		}
	}
}
