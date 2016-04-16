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

struct Tile {
	unsigned int color; //TODO: need textures too? / combine with sprite
} tile_desc[] = {
	[TILE_AIR]   = { .color = 0xff },
	[TILE_WALL]  = { .color = 0xffffffff },
	[TILE_SPIKE] = { .color = 0xff0000ff }
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

			int coll_type = COLLISION_NONE;

			switch(*c){
				case '#': {
					tiles[i]  = TILE_WALL;
					coll_type = COLLISION_BOX;
				} break;

				case 'x': {
					tiles[i] = TILE_SPIKE;
					coll_type = COLLISION_BOX;
				} break;

				case '.':
				default: {
					tiles[i] = TILE_AIR;
				} break;
			}

			Sprite* s = sprite_push_col(x, y, 32, 32, tile_desc[tiles[i]].color);
			SDL_assert(s);

			s->collision_type = coll_type;

			++i;
			x += 32;
		}

		x = 0;
		y += 32;
	}

	fclose(f);
}
