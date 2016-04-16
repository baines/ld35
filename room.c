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

static int current_room;

static int sprite_offset;

typedef struct {
	int neighbours[4];
} RoomMap;

// hope you don't want more than 256 rooms
static RoomMap room_map[256];

void room_init(void){

	FILE* f = fopen("data/map.txt", "rb");

	// XXX: first line is ignored...
	char line[256];
	fgets(line, sizeof(line), f);

	int id;
	int n[4];
	while(fscanf(f, "%d: %d %d %d %d\n", &id, n, n + 1, n + 2, n + 3) == 5){
		puts("loaded some stuff");
		SDL_assert(id < array_count(room_map));
		memcpy(room_map[id].neighbours, n, sizeof(n));
	}

}

void room_load(int number){

	printf("Loading room %d\n", number);

	// already a room loaded
	if(sprite_offset){
		sprite_pop(sprites + sprite_offset, ROOM_WIDTH * ROOM_HEIGHT);
	}

	char* filename;
	asprintf(&filename, "data/room%d.txt", number);
	FILE* f = fopen(filename, "rb");

	SDL_assert(f);

	current_room = number;

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
	int id = room_map[current_room].neighbours[which];

	room_load(id);
}
