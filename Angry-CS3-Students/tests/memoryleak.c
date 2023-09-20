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

// Pig constants
const size_t PIG_ID = 0;
const size_t N_CIRCLE_PTS = 20;
const size_t PIG_RADIUS = 10;
const size_t PIG_MASS = 1;
const size_t N_PIGS = 2;
const size_t PIG_TOKENS = 3;
const rgb_color_t PIG_COLOR = {0.0, 1.0, 0.0};

// Bird constants
const size_t BIRD_ID = 1;
const size_t BIRD_RADIUS = 15;
const size_t BIRD_MASS = 10;
const vector_t BIRD_SPAWN = {125, 100};
const rgb_color_t BIRD_COLOR = {1.0, 0.0, 0.0};

// Platform constants
const size_t PLAT_ID = 2;
const int32_t PLAT_LENGTH = 20;
const int32_t PLAT_HEIGHT = 5;
const size_t N_PLATFORMS = 2;
const rgb_color_t PLAT_COLOR = {0.0, 0.0, 1.0};

// Obstacle constants
const size_t OBST_ID = 3;
const int32_t OBST_LENGTH = 45;
const int32_t OBST_HEIGHT = 20;
const size_t N_OBSTACLES = 2;
const rgb_color_t OBST_COLOR = {0.0, 1.0, 1.0};

// Slingshot constants
const size_t SLING_ID = 4;
const int32_t SLING_LENGTH = 10;
const int32_t SLING_HEIGHT = 50;
const size_t BAND_ID = 5;
const int32_t BAND_HEIGHT = 8;
const rgb_color_t SLINGSHOT_COLOR = {0.588, 0.294, 0};
const rgb_color_t BAND_COLOR = {0.5, 0.5, 0.5};

// Rubberband constants
const size_t RUBBER_ID = 5;
const rgb_color_t RUBBER_COLOR = {0, 0.5, 0};
const vector_t RUBBER_CENTER = {146.0, 127.0};

// Force constants
const size_t GRAVITY_ID = 0;

// General constants
const double TIME_SPAN = 5;
const double ACCEL = 50;
const size_t VELOCITY = 500;
const size_t RECTANGLE_POINTS = 4;
const size_t MASS = 1;


// Define the state struct
typedef struct state {
  scene_t *scene;
  double time;
  vector_t *rubber_center;
  bool return_press;
  size_t prev_pigs;
  size_t score;
} state_t;

/* SCENE SET UP */

// Helper function to construct circle with given radius centered at (0, 0)
list_t *make_circle(double radius) {
  list_t *circle = list_init(N_CIRCLE_PTS, free);
  double arc_angle = 2 * M_PI / N_CIRCLE_PTS;
  vector_t point = {.x = radius, .y = 0.0};
  for (size_t i = 0; i < N_CIRCLE_PTS; i++)
  {
    vector_t *v = malloc(sizeof(*v));
    *v = point;
    list_add(circle, v);
    point = vec_rotate(point, arc_angle);
  }
  return circle;
}

void make_pigs(scene_t *scene, list_t *plat_centers) {
  // bird is always the third body in the scene 
  body_t *bird = scene_get_body(scene, 2);
  size_t* info = body_get_info(bird);
  assert (*info == BIRD_ID);

  vector_t *center;
  for(size_t i = 0; i < list_size(plat_centers); i++) {

    size_t* id = malloc(sizeof(size_t));
    *id = PIG_ID;

    body_t *pig = body_init_with_info(make_circle(PIG_RADIUS), PIG_MASS, PIG_COLOR, id, free);

    vector_t *plat_center = (vector_t*) list_get(plat_centers, i);
    center = malloc(sizeof(vector_t));
    center->x = plat_center->x;
    center->y = plat_center->y + PLAT_HEIGHT + PIG_RADIUS;
    body_set_centroid(pig, *center);

    create_destructive_collision2(scene, pig, bird);
    scene_add_body(scene, pig);

    free(center);
  }
}

void make_bird(scene_t *scene) {
  size_t* id = malloc(sizeof(size_t));
  *id = BIRD_ID;

  body_t *bird = body_init_with_info(make_circle(BIRD_RADIUS), BIRD_MASS, BIRD_COLOR, id, free);
  body_set_centroid(bird, RUBBER_CENTER);
  scene_add_body(scene, bird);
}

list_t *make_rectangle(int32_t length, int32_t height) {
  list_t *rectangle = list_init(4, free);
  vector_t *v = malloc(sizeof(*v));
  *v = (vector_t){-length, -height};
  list_add(rectangle, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){+length, -height};
  list_add(rectangle, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){+length, +height};
  list_add(rectangle, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-length, +height};
  list_add(rectangle, v);
  return rectangle;
}

void make_platforms(scene_t *scene, list_t *centers) {
  // bird is always the third body in scene 
  body_t *bird = scene_get_body(scene, 2);
  size_t* info = body_get_info(bird);
  assert (*info == BIRD_ID);

  for(size_t i = 0; i < list_size(centers); i++) {
    size_t* id = malloc(sizeof(size_t));
     *id = PLAT_ID;

    list_t *shape = make_rectangle(PLAT_LENGTH, PLAT_HEIGHT);
    body_t *platform = body_init_with_info(shape, INFINITY, PLAT_COLOR, id, free);

    vector_t *center = (vector_t*) list_get(centers, i);
    body_set_centroid(platform, *center);

    create_physics_collision(scene, 1, platform, bird);
    scene_add_body(scene, platform);

  }
}

list_t *make_slingshot(scene_t *scene) {
  list_t *slingshot_list = list_init(13, free);
  
  vector_t *v = malloc(sizeof(*v));
  *v = (vector_t){143, 0};
  list_add(slingshot_list, v);
  
  v = malloc(sizeof(*v));
  *v = (vector_t){157, 0};
  list_add(slingshot_list, v);

  v = malloc(sizeof(*v));
  *v = (vector_t){157, 50};
  list_add(slingshot_list, v);

  v = malloc(sizeof(*v));
  *v = (vector_t){182, 100};
  list_add(slingshot_list, v);

  v = malloc(sizeof(*v));
  *v = (vector_t){182, 150};
  list_add(slingshot_list, v);

  v = malloc(sizeof(*v));
  *v = (vector_t){173, 150};
  list_add(slingshot_list, v);

  v = malloc(sizeof(*v));
  *v = (vector_t){173, 100};
  list_add(slingshot_list, v);

  v = malloc(sizeof(*v));
  *v = (vector_t){146, 52};
  list_add(slingshot_list, v);

  v = malloc(sizeof(*v));
  *v = (vector_t){127, 93};
  list_add(slingshot_list, v);

  v = malloc(sizeof(*v));
  *v = (vector_t){127, 138};
  list_add(slingshot_list, v);

  v = malloc(sizeof(*v));
  *v = (vector_t){120, 138};
  list_add(slingshot_list, v);

  v = malloc(sizeof(*v));
  *v = (vector_t){120, 93};
  list_add(slingshot_list, v);

  v = malloc(sizeof(*v));
  *v = (vector_t){143, 47};
  list_add(slingshot_list, v);

  return slingshot_list;
}

list_t *make_rubberband(scene_t *scene, vector_t *center){
  // remove existing rubber bands if they exit
  for (int i = 0; i < scene_bodies(scene); i++) {
    body_t *rubber = scene_get_body(scene, i);
    if ((size_t) body_get_info(rubber) == RUBBER_ID){
      scene_remove_body(scene, i);
    }
  }

  list_t *rubber_list = list_init(13, free);
  
  vector_t *v = malloc(sizeof(*v));
  *v = (vector_t){127, 117};
  list_add(rubber_list, v);
  
  v = malloc(sizeof(*v));
  *v = (vector_t){127, 117};
  list_add(rubber_list, v);

  v = malloc(sizeof(*v));
  *v = (vector_t){157, 50};
  list_add(rubber_list, v);

  v = malloc(sizeof(*v));
  *v = (vector_t){center->x, center->y};
  list_add(rubber_list, v);

  v = malloc(sizeof(*v));
  *v = (vector_t){173, 125};
  list_add(rubber_list, v);

  v = malloc(sizeof(*v));
  *v = (vector_t){173, 140};
  list_add(rubber_list, v);

  v = malloc(sizeof(*v));
  *v = (vector_t){center->x, center->y};
  list_add(rubber_list, v);

  return rubber_list;
}

void make_obstacles(scene_t *scene, list_t *centers) {
  // bird is always the third body in scene 
  body_t *bird = scene_get_body(scene, 2);
  size_t* info = body_get_info(bird);
  assert (*info == BIRD_ID);


  for(size_t i = 0; i < list_size(centers); i++) {
    size_t* id = malloc(sizeof(size_t));
    *id = OBST_ID;
    list_t *shape = make_rectangle(OBST_LENGTH, OBST_HEIGHT);
    body_t *obstacle = body_init_with_info(shape, INFINITY, OBST_COLOR, id, free);

    vector_t *center = (vector_t*) list_get(centers, i);
    body_set_centroid(obstacle, *center);

    create_physics_collision(scene, 1, obstacle, bird);
    scene_add_body(scene, obstacle);
  }
}

void level_one(scene_t *scene) {
  list_t *centers = list_init(N_OBSTACLES, free);

  vector_t *center = malloc(sizeof(vector_t));
  *center = (vector_t) {500, 100};
  list_add(centers, center);

  center = malloc(sizeof(vector_t));
  *center = (vector_t) {600, 400};
  list_add(centers, center);

  make_platforms(scene, centers);
  make_pigs(scene, centers);

  list_free(centers);
  // free(center);
}

void wrap_scene(state_t *state) {
  scene_t *scene = state->scene;
  // bird is always the third body in scene 
  body_t *bird = scene_get_body(scene, 2);
  vector_t vec = body_get_centroid(bird);
  if (vec.y <= 0 || vec.y >= WINDOW_H || vec.x >= WINDOW_W || vec.x <= 0) {
    make_rubberband(scene, state->rubber_center);
    body_set_centroid(bird, RUBBER_CENTER);
    body_set_velocity(bird, VEC_ZERO);
    scene_remove_force_creator(scene, GRAVITY_ID);

    state->return_press = false;
  }
}


/* KEY HANDLING */

void on_key_press(char key, key_event_type_t type, double held_time,
                  void *curr_state) {
  state_t *state = (state_t *)curr_state;
  scene_t *scene = state->scene;
  body_t *rubber_band = scene_get_body(scene, 1);
  body_t *bird = scene_get_body(scene, 2);
  // vector_t *RUBBER_CENTER

  if (type == KEY_PRESSED) {
    if (key == SPACEBAR && body_get_velocity(bird).y < 0) {
      vector_t new_vel = vec_add(body_get_velocity(bird), vec_multiply(held_time, (vector_t) {ACCEL, -ACCEL}));
      body_set_velocity(bird, new_vel);
    } 
    if (state->return_press == false) {
      if (key==LEFT_ARROW) {
        state->rubber_center->x = state->rubber_center->x - 0.5*state->rubber_center->x*held_time;
        state->rubber_center->y = state->rubber_center->y;

        body_set_centroid(bird, *(state->rubber_center));
        list_t *rubberband = make_rubberband(state->scene, state->rubber_center);
        body_t *rubberband_b = body_init_with_info(rubberband, INFINITY, RUBBER_COLOR, (void *)RUBBER_ID, NULL);
        scene_add_body(state->scene, rubberband_b);
      }
      if (key==RIGHT_ARROW) {
        state->rubber_center->x = state->rubber_center->x + 0.5*state->rubber_center->x*held_time;
        state->rubber_center->y = state->rubber_center->y;
        
        body_set_centroid(bird, *(state->rubber_center));

        list_t *rubberband = make_rubberband(state->scene, state->rubber_center);
        body_t *rubberband_b = body_init_with_info(rubberband, INFINITY, RUBBER_COLOR, (void *)RUBBER_ID, NULL);
        scene_add_body(state->scene, rubberband_b);
      }
      if (key==DOWN_ARROW) {
        //vector_t *new_center = malloc(sizeof(vector_t));
        state->rubber_center->x = state->rubber_center->x;
        state->rubber_center->y = state->rubber_center->y - 0.5*state->rubber_center->y*held_time;

        body_set_centroid(bird, *(state->rubber_center));
        list_t *rubberband = make_rubberband(state->scene, state->rubber_center);
        body_t *rubberband_b = body_init_with_info(rubberband, INFINITY, RUBBER_COLOR, (void *)RUBBER_ID, NULL);
        scene_add_body(state->scene, rubberband_b);
      }
      if (key==UP_ARROW) {
       // vector_t *new_center = malloc(sizeof(vector_t));
        state->rubber_center->x = state->rubber_center->x;
        state->rubber_center->y = state->rubber_center->y + 0.5*state->rubber_center->y*held_time;

        body_set_centroid(bird, *(state->rubber_center));
        list_t *rubberband = make_rubberband(state->scene, state->rubber_center);
        body_t *rubberband_b = body_init_with_info(rubberband, INFINITY, RUBBER_COLOR, (void *)RUBBER_ID, NULL);
        scene_add_body(state->scene, rubberband_b);
      }
      if (key==RETURN) {
        
        body_set_velocity(bird, (vector_t) {50, 50});

        create_downward_gravity(scene, 100, bird, GRAVITY_ID);
        state->return_press = true;
      }
    }
    
  } 

}

/* SCORE */

size_t calculate_score(state_t *state) {
  size_t curr_pigs = 0;
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    size_t* info = body_get_info(scene_get_body(state->scene, i));

    if (*info == PIG_ID) {
      curr_pigs++;
    }
  }

  if (curr_pigs < state->prev_pigs) {
      state->score = (state->prev_pigs - curr_pigs) * PIG_TOKENS;
  }

  return curr_pigs;
}

int main() {
  // vector_t *original_center = vec_init(RUBBER_CENTER.x, RUBBER_CENTER.y);

  state_t *state = malloc(sizeof(state_t));
  state->scene = scene_init();
  state->rubber_center = malloc(sizeof(vector_t));
  state->rubber_center->x = RUBBER_CENTER.x;
  state->rubber_center->y = RUBBER_CENTER.y;
  state->return_press = false;

  list_t *slingshot = make_slingshot(state->scene);
  body_t *slingshot_b = body_init_with_info(slingshot, INFINITY, SLINGSHOT_COLOR, (void *)SLING_ID, NULL);
  scene_add_body(state->scene, slingshot_b);

  list_t *rubberband = make_rubberband(state->scene, state->rubber_center);
  body_t *rubberband_b = body_init_with_info(rubberband, INFINITY, RUBBER_COLOR, (void *)RUBBER_ID, NULL);
  scene_add_body(state->scene, rubberband_b);

  make_bird(state->scene);

  level_one(state->scene);
  double num_seconds = 60.0;
  double dt = 0.1;
  state->time = 0.0;
  while(state->time < num_seconds) {
    state->time += dt;
    scene_tick(state->scene, dt);
    wrap_scene(state);
  }

  scene_free(state->scene);
  free(state->rubber_center);
  free(state);

} 

