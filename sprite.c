#include "stb_image.h"

Sprite sprites[256];
int num_sprites;

void sprite_push(int x, int y, int w, int h){
	// black by default?
	sprite_push_col(x, y, w, h, 0);
}

void sprite_push_col(int x, int y, int w, int h, unsigned int color){
	if(num_sprites >= array_count(sprites)){
		return;
	}



}

void sprite_push_tex(int x, int y, int w, int h, const char* name){

}
