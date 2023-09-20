
#include "body.h"
#include "color.h"
#include "forces.h"
#include "list.h"
#include "polygon.h"
#include "sdl_wrapper.h"
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

// Define the state struct
typedef struct state {
  scene_t *scene;
} state_t;
// Function that initializes the state to hold the star and returns the state

state_t *emscripten_init() {
    sdl_init(MIN_POINT, MAX_POINT);

    state_t *state = malloc(sizeof(state_t));
    state->scene = scene_init();
    
    sdl_render_text(state->scene);
    return state;
}

/*
 * Runs each tick of the loop and displays result on the canvas
 * Input to the function is the updated state
 */
void emscripten_main(state_t *state) {
  sdl_clear();
  sdl_render_text(state->scene);
  sdl_show();
  //sdl_render_scene(state->scene);
}

// Function to free memory associated with everything
void emscripten_free(state_t *state)
{
  scene_free(state->scene);
  free(state);
}