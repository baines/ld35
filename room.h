#ifndef ROOM_H_
#define ROOM_H_

// XXX: sensible order?
enum {
	ROOM_RIGHT,
	ROOM_LEFT,
	ROOM_DOWN,
	ROOM_UP,
};

void room_load(int number);
void room_update(int delta);
//void room_draw();

void room_switch(int which);

#endif
