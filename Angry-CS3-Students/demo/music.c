#include "sdl_wrapper.h"
#include "utils.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Window constants
const size_t SCREEN_WIDTH = 1000;
const size_t SCREEN_HEIGHT = 500;
const vector_t BOTTOM_LEFT = {0, 0};
const vector_t TOP_RIGHT = {SCREEN_WIDTH, SCREEN_HEIGHT};

const char* AUDIO_FOLDER = "sounds/";
const char* EXTENSION = ".wav";

// Define the state struct
typedef struct game_state {
  scene_t *scene;
} game_state_t;

// Dynamic arrays to store the sounds and music
Mix_Chunk **effect_sounds = NULL;
int sound_count = 0;

Mix_Music **music_tracks = NULL;
int music_count = 0;

int sound_volume;

int loadMusicTrack(const char* filename) {
    Mix_Chunk *track = Mix_LoadWAV(filename);
    if(track == NULL) {
        printf("Failed to load music. SDL_Mixer error: %s\n", Mix_GetError());
        return -1;
    }
    music_tracks = realloc(music_tracks, sizeof(*music_tracks) * (music_count + 1));
    music_tracks[music_count] = track;
    return music_count++;
}

int loadSoundEffect(const char* filename) {
    Mix_Chunk *effect = Mix_LoadWAV(filename);
    if(effect == NULL) {
        printf("Failed to load sound. SDL_Mixer error: %s\n", Mix_GetError());
        return -1;
    }
    effect_sounds = realloc(effect_sounds, sizeof(*effect_sounds) * (sound_count + 1));
    effect_sounds[sound_count] = effect;
    return sound_count++;
}

void adjustVolume(int v) {
    sound_volume = (MIX_MAX_VOLUME * v) / 100;
}

int playMusicTrack(int m) {
    if(Mix_PlayingMusic() == 0) {
        Mix_Volume(1, sound_volume);
        Mix_PlayMusic(music_tracks[m], -1);
    }
    return 0;
}

int playSoundEffect(int s) {
    Mix_Volume(-1, sound_volume);
    Mix_PlayChannel(-1, effect_sounds[s], 0);
    return 0;
}

int setupMixer() {
    Mix_Init(MIX_INIT_MP3);
    SDL_Init(SDL_INIT_AUDIO);
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_Mixer couldn't initialize. Err: %s\n", Mix_GetError());
        return -1;
    }
    adjustVolume(80);
    return 0;
}

void closeMixer() {
    for(int s = 0; s < sound_count; s++) {
        Mix_FreeChunk(effect_sounds[s]);
        effect_sounds[s] = NULL;
    }
    free(effect_sounds);
    for(int m = 0; m < music_count; m++) {
        Mix_FreeMusic(music_tracks[m]);
        music_tracks[m] = NULL;
    }
    free(music_tracks);
    Mix_Quit();
}

void switchPlay() {
    if(Mix_PausedMusic() == 1) {
        Mix_ResumeMusic();
    } else {
        Mix_PauseMusic();
    }
}

game_state_t *game_start() {
    sdl_init(BOTTOM_LEFT, TOP_RIGHT);

    game_state_t *game = malloc(sizeof(game_state_t));
    game->scene = scene_init();
    
    return game;
}

void game_run(game_state_t *game) {
  sdl_clear();
  setupMixer();
    
  int sound_fx = loadSoundEffect("sounds/Birds_Flying.wav");
  int bg_music = loadMusicTrack("sounds/Angry_Birds_Theme.wav");
    
  playMusicTrack(bg_music);
  playSoundEffect(sound_fx);
}

void game_end(game_state_t *game)
{
  closeMixer();
  scene_free(game->scene);
  free(game);
}
