srcs := $(wildcard *.c)
hdrs := $(wildcard *.h)
objs := $(patsubst %.c,%.o,$(srcs))

CFLAGS  := -D_GNU_SOURCE -std=c99 -g -Wall
LDFLAGS := -lSDL2 -lm -lSDL2_image -lSDL2_mixer

all: game

game: $(objs)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c $(hdrs)
	$(CC) -c $(CFLAGS) $< -o $@
