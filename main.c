#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "ld35.h"

bool running = true;

SDL_Window* win;
SDL_Renderer* renderer;
SDL_Rect viewport;

static int titlescreen_timer = 4000;

void handle_event(SDL_Event* e){
	switch(e->type){
		case SDL_QUIT: {
			running = false;
		} break;
	}
}

int main(int argc, char** argv){

	srand(time(0));

	SDL_Init(SDL_INIT_EVERYTHING);

	if(SDL_CreateWindowAndRenderer(WIN_WIDTH, WIN_HEIGHT, 0, &win, &renderer) == -1){
		fprintf(stderr, "SDL error creating window/renderer: %s\n", SDL_GetError());
		return 1;
	}

	SDL_SetWindowTitle(win, "VampShift: The Bloodening");

	SDL_RenderGetViewport(renderer, &viewport);

	uint64_t timer_freq = SDL_GetPerformanceFrequency() / (1000 * 1000);
	
	printf("%" PRIu64 "\n", timer_freq);
	uint64_t timer_start;

	sound_init();
	game_init();

	Sprite titlescreen = {
		.x = (WIN_WIDTH / 2) - (512/2),
		.y = (WIN_HEIGHT / 2) - (256/2),
		.w = 512,
		.h = 256,
	};
	sprite_set_tex(&titlescreen, "data/title.png", 1);

	timer_start = SDL_GetPerformanceCounter() / timer_freq;
	
	while(running){
		SDL_Event e;
		while(SDL_PollEvent(&e)){
			handle_event(&e);
		}

		uint64_t timer_diff = (SDL_GetPerformanceCounter() / timer_freq) - timer_start;
		timer_start = SDL_GetPerformanceCounter() / timer_freq;

		//XXX: test if this actually works...
		if(timer_diff < 16667){
			SDL_Delay(17 - (timer_diff / 1000));
			timer_diff = 16667;
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
	
		if(titlescreen_timer > 0){
			titlescreen_timer -= (timer_diff / 1000);
			sprite_draw(&titlescreen);
		} else {
			game_update(timer_diff / 1000);
			game_draw();
		}
		SDL_RenderPresent(renderer);
	}

}
