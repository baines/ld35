#include "game.h"
#include "sprite.h"

#define PARTICLE_SIZE 6

typedef struct {
	Sprite sprite;
	float x_vel;
	float y_vel;
	int ttl;
} Particle;

Particle particles[1024];
int particle_cursor;

void particles_spawn(SDL_Point pos, float xv, float yv, int amount){

	//TODO: better spread
	xv += ((rand() % 800) - 400) / 100.0f;
	yv += ((rand() % 800) - 400) / 100.0f;

	Particle p = {
		.sprite = {
			.x = pos.x,
			.y = pos.y,
			.w = PARTICLE_SIZE,
			.h = PARTICLE_SIZE,
		},
		.x_vel = xv,
		.y_vel = yv,
		.ttl = 50 + (rand() % 100)
	};

	particles[particle_cursor] = p;
	particle_cursor = (particle_cursor + 1) % array_count(particles);
}

void particles_update(int delta){

	for(int i = 0; i < array_count(particles); ++i){
		Particle* p = particles + i;
		if(p->ttl <= 0) continue;

		if(p->ttl < 100){

			// crappy gravity
			p->y_vel -= 2;

			p->sprite.x += p->x_vel;
			p->sprite.y -= p->y_vel;
		}
		p->ttl--;
	}

}

void particles_draw(void){

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	for(int i = 0; i < array_count(particles); ++i){
		Particle* p = particles + i;
		if(p->ttl <= 0 || p->ttl >= 100) continue;

		SDL_RenderFillRect(renderer, &p->sprite.rect);
	}

}
