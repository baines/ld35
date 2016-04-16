#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sprite.h"

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

typedef struct {
	int id;
	int tiles[ROOM_WIDTH * ROOM_HEIGHT];
} Room;

Room current_room;

void room_load(int number){

	char* filename;
	asprintf(&filename, "data/room%d.txt", number);
	FILE* f = fopen(filename, "rb");

	free(filename);

	char line[256];

	int x = 0, y = 0;
	int i = 0;

	int* tiles = current_room.tiles;

	while(fgets(line, sizeof(line), f)){

		for(const char* c = line; *c; ++c){
			if(*c == '\n') continue;

			switch(*c){
				case '#': {
					tiles[i]  = TILE_WALL;
				} break;

				case 'x': {
					tiles[i] = TILE_SPIKE;
				} break;

				case '.':
				default: {
					tiles[i] = TILE_AIR;
				} break;
			}

			Tile* t = tile_desc + tiles[i];

			Sprite* s = sprite_push_col(x, y, 32, 32, t->color);
			SDL_assert(s);

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
