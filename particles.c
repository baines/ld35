#include "ld35.h"

#define PARTICLE_SIZE 6
#define PARTICLE_TTL_LIMIT 1500

typedef struct {
	Sprite sprite;
	float x_pos, y_pos, x_vel, y_vel;
	int ttl;
} Particle;

Particle particles[4096];
int particle_cursor;

void particles_spawn(SDL_Point pos, float xv, float yv, int amount){

	for(int i = 0; i < amount; ++i){
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
			.x_pos = pos.x,
			.y_pos = pos.y,
			.x_vel = xv,
			.y_vel = yv,
			.ttl = PARTICLE_TTL_LIMIT + (rand() % 400)
		};

		particles[particle_cursor] = p;
		particle_cursor = (particle_cursor + 1) % array_count(particles);
	}
}

void particles_update(int delta){

	for(int i = 0; i < array_count(particles); ++i){
		Particle* p = particles + i;
		if(p->ttl <= 0) continue;

		if(p->ttl < PARTICLE_TTL_LIMIT){

			// crappy gravity
			p->y_vel -= (delta / 8.0f);

			p->x_pos += (p->x_vel / (float)delta);
			p->y_pos -= (p->y_vel / (float)delta);

			p->sprite.x = p->x_pos;
			p->sprite.y = p->y_pos;
		}
		p->ttl -= delta;
	}
}

void particles_draw(void){

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	for(int i = 0; i < array_count(particles); ++i){
		Particle* p = particles + i;
		if(p->ttl <= 0 || p->ttl >= PARTICLE_TTL_LIMIT) continue;

		SDL_RenderFillRect(renderer, &p->sprite.rect);
	}

}
