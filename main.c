#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "game.h"

bool running = true;

void handle_event(SDL_Event* e){
	switch(e->type){
		case SDL_QUIT: {
			running = false;
		} break;
	}
}

int main(int argc, char** argv){

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* win;
	SDL_Renderer* r;

	if(SDL_CreateWindowAndRenderer(854, 480, 0, &win, &r) == -1){
		fprintf(stderr, "SDL error creating window/renderer: %s\n", SDL_GetError());
		return 1;
	}

	SDL_SetWindowTitle(win, "LD35");

	uint64_t timer_freq = SDL_GetPerformanceFrequency() / (1000 * 1000);
	
	printf("%" PRIu64 "\n", timer_freq);
	uint64_t timer_start;

	game_init();

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
		}

		game_update(timer_diff);
		
		SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
		SDL_RenderClear(r);
		game_draw(r);

		SDL_RenderPresent(r);
	}

}
