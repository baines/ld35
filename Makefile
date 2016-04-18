srcs := $(wildcard *.c)
hdrs := $(wildcard *.h)
objs := $(patsubst %.c,%.o,$(srcs))

CFLAGS  := -D_GNU_SOURCE -Wall -std=c99 -g
LDFLAGS := -lm -lSDL2 -lSDL2_mixer 

all: game

webgame.html: CFLAGS  := -D_GNU_SOURCE -Wall -I. -I/home/alex/Desktop/emsc/include -Oz
webgame.html: LDFLAGS := --preload-file data -s USE_SDL=2 -s STB_IMAGE=1 -s USE_OGG=1 -s USE_VORBIS=1 -s TOTAL_MEMORY=18000000
webgame.html: $(objs)
	$(CC) $(CFLAGS) $^ ./emscripten/libSDL2_mixer.a -o $@ $(LDFLAGS)

game: $(objs)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c $(hdrs)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	$(RM) $(objs) game
