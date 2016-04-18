#include "ld35.h"
#include <SDL2/SDL_mixer.h>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

typedef struct {
	const char* name;
	int volume;
	Mix_Chunk* sfx;
} Sound;

Sound sounds[] = {
	{ "data/bat.ogg", 80 },
	{ "data/die.ogg", 64 },
	{ "data/shapeshift.ogg", 40 },
	{ "data/unshapeshift.ogg", 40 },
	{ "data/nope.ogg", 20 },
	{ "data/powerup.ogg", 64 },
};

#define NUM_CHANNELS 3
static int channel;
static Mix_Chunk* sfx_chans[128];
static Mix_Music* music_loop;

static void music_loop_callback(void){
	if(music_loop){
		Mix_PlayMusic(music_loop, -1);
	}
	Mix_HookMusicFinished(NULL);
}

void sound_init(void){

	SDL_Init(SDL_INIT_AUDIO);

	int flags = Mix_Init(MIX_INIT_OGG);
	if(!(flags & MIX_INIT_OGG)){
		fprintf(stderr, "Couldn't load ogg vorbis codec: %d\n", flags);
		Mix_Init(0);
	}

#ifdef __EMSCRIPTEN__
    int const frequency = EM_ASM_INT_V({
        var context;
        try {
            context = new AudioContext();
        } catch (e) {
            context = new webkitAudioContext();
        }
        return context.sampleRate;
    });
#else
    int const frequency = 48000;
#endif
   
	if(Mix_OpenAudio(frequency, AUDIO_S16, 2, 4096) == -1){
		fprintf(stderr, "Couldn't open audio: %s\n", Mix_GetError());
		return;
	}

	for(int i = 0; i < array_count(sounds); ++i){
		sounds[i].sfx = Mix_LoadWAV(sounds[i].name);
		if(!sounds[i].sfx){
			fprintf(stderr, "Couldn't load %s: %s\n", sounds[i].name, Mix_GetError());
		}
		Mix_VolumeChunk(sounds[i].sfx, sounds[i].volume ? sounds[i].volume : 64);
	}

	Mix_ReserveChannels(NUM_CHANNELS);

	Mix_Music* intro_music = Mix_LoadMUS("data/ear-bleed-intro.ogg");
	if(intro_music){
		Mix_PlayMusic(intro_music, 0);
		Mix_HookMusicFinished(&music_loop_callback);
	}

	music_loop = Mix_LoadMUS("data/ear-bleed-loop.ogg");
}


void sound_play(const char* name, int loops){

	Mix_Chunk* chunk = NULL;

	for(int i = 0; i < array_count(sounds); ++i){
		if(strcmp(name, sounds[i].name) == 0){
			chunk = sounds[i].sfx;
			break;
		}
	}

	if(!chunk) return;

	// don't double up sound effects
	for(int i = 0; i < NUM_CHANNELS; ++i){
		if(sfx_chans[i] == chunk){
			Mix_HaltChannel(i);
		}
	}

	int i = Mix_PlayChannel(channel, chunk, loops);
	if(i == -1){
		fprintf(stderr, "SFX error: %s\n", Mix_GetError());
	} else {
		sfx_chans[i] = chunk;
	}

	channel = (channel + 1) % NUM_CHANNELS;
}
