
#include "sdl_wrapper.h"
#include "utils.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Window constants
const size_t WINDOW_W = 1000;
const size_t WINDOW_H = 500;
const vector_t MIN_POINT = {0, 0};
const vector_t MAX_POINT = {WINDOW_W, WINDOW_H};


const char* SPRITE_FOLDER = "assets/";
const char* SPRITE_TYPE = ".png";

// Define the state struct
typedef struct state {
  scene_t *scene;
} state_t;
// Function that initializes the state to hold the star and returns the state

state_t *emscripten_init() {
    sdl_init(MIN_POINT, MAX_POINT);

    state_t *state = malloc(sizeof(state_t));
    state->scene = scene_init();
    
    return state;
}

/*
 * Runs each tick of the loop and displays result on the canvas
 * Input to the function is the updated state
 */
void emscripten_main(state_t *state) {
  sdl_clear();
  //char* name = "angrybird";
  void* texture = sdl_get_texture("assets/alice.png");
  vector_t center = {0, 0};
  vector_t dim = {50, 50};
  sdl_render_image(state->scene, texture, center, dim);
  sdl_show();
}

// Function to free memory associated with everything
void emscripten_free(state_t *state)
{
  scene_free(state->scene);
  free(state);
}

