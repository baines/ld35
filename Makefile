srcs := $(wildcard *.c)
hdrs := $(wildcard *.h)
objs := $(patsubst %.c,%.o,$(srcs))

CFLAGS  := -std=c99 -g -Wall
LDFLAGS := -lSDL2 -lm

all: game

game: $(objs)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c $(hdrs)
	$(CC) -c $(CFLAGS) $< -o $@
