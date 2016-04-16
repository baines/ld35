#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "room.h"
#include "sprite.h"
#include "game.h"

#define ROOM_WIDTH  26
#define ROOM_HEIGHT 15

enum {
	TILE_AIR,
	TILE_WALL,
	TILE_SPIKE,
};

typedef struct {
	unsigned int color; //TODO: need textures too? / combine with sprite
	int collision_type;
	int collision_response;
} Tile;

Tile tile_desc[] = {
	[TILE_AIR]   = {
		.color          = 0xff,
		.collision_type = COLLISION_NONE
	},

	[TILE_WALL]  = {
		.color              = 0xffffffff,
		.collision_type     = COLLISION_BOX,
		.collision_response = CRESP_BLOCK,
	},

	[TILE_SPIKE] = {
		.color              = 0xff0000ff,
		.collision_type     = COLLISION_BOX,
		.collision_response = CRESP_KILL,
	}
};

static int sprite_offset;

void room_load(int number){

	printf("Loading room %d\n", number);

	// already a room loaded
	if(sprite_offset){
		sprite_pop(sprites + sprite_offset, ROOM_WIDTH * ROOM_HEIGHT);
	}

	char* filename;
	asprintf(&filename, "data/room%d.txt", number);
	FILE* f = fopen(filename, "rb");

	free(filename);

	char line[256];

	int x = 0, y = 0;
	int i = 0;

	int tile_type = 0;

	sprite_offset = num_sprites;

	while(fgets(line, sizeof(line), f)){

		for(const char* c = line; *c; ++c){
			if(*c == '\n') continue;

			switch(*c){
				case '#': {
					tile_type = TILE_WALL;
				} break;

				case 'x': {
					tile_type = TILE_SPIKE;
				} break;

				case '.':
				default: {
					tile_type = TILE_AIR;
				} break;
			}

			Tile* t = tile_desc + tile_type;

			Sprite* s = sprite_push_col(x, y, 32, 32, t->color);

			s->collision_type     = t->collision_type;
			s->collision_response = t->collision_response;

			++i;
			x += 32;
		}

		x = 0;
		y += 32;
	}

	fclose(f);
}

void room_switch(int which){
	//TODO
	printf("switch %d\n", which);

	int id = 0;

	switch(which){
		case ROOM_RIGHT: {

		} break;
		case ROOM_LEFT: {

		} break;
		case ROOM_UP: {

		} break;
		case ROOM_DOWN: {

		} break;
	}

	room_load(id);
}
