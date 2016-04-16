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
	sprite_push_col(400, 200, 20, 20, 0x00ff00ff);
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

void game_draw(SDL_Renderer* r){

	for(int i = 0; i < num_sprites; ++i){

		SDL_SetRenderDrawColor(
			r,
			sprites[i].color.r,
			sprites[i].color.g,
			sprites[i].color.b,
			255 // TODO: work out alpha handling
		);

		SDL_RenderFillRect(r, &sprites[i].rect);
	}
}
