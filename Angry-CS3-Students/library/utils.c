#include "body.h"
#include "color.h"
#include "forces.h"
#include "polygon.h"
#include "utils.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Helper function to get body in the scene based on id
body_t *get_body(scene_t *scene, size_t body_id) {
    // initialize to something random to surpress warnings, will be replaced by 
    // actual body later
    body_t *ret_body = scene_get_body(scene, 0); 
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        size_t* info = body_get_info(scene_get_body(scene, i));

        if (*info == body_id) {
            ret_body = scene_get_body(scene, i);
            break;
        }
    }
    return ret_body;
}

// Helper function to get body in the scene based on id
size_t get_idx(scene_t *scene, size_t body_id) {
    // initialize to something random to surpress warnings, will be replaced by 
    // actual idx later
    size_t ret_idx = 0; 
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        size_t* info = body_get_info(scene_get_body(scene, i));

        if (*info == body_id) {
            ret_idx = i;
            break;
        }
    }
    return ret_idx;
}

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

// Helper function to construct an equilateral triangle with given side length
list_t *make_equilateral_triangle(double side_length) {
  list_t *triangle = list_init(3, free);
  vector_t points[3] = {
    {.x = 0, .y = 0},
    {.x = side_length, .y = 0},
    {.x = side_length / 2, .y = sqrt(3) * side_length / 2}
  };

  for (size_t i = 0; i < 3; i++)
  {
    vector_t *v = malloc(sizeof(*v));
    *v = points[i];
    list_add(triangle, v);
  }
  return triangle;
}

// makes the sprite path
char* make_path(char* name) {
  size_t len = strlen(SPRITE_FOLDER) + strlen(name) + strlen(SPRITE_TYPE) + 1;
  char* path = malloc(sizeof(char) * len);
  strcpy(path, SPRITE_FOLDER);
  strcat(path, name);
  strcat(path, SPRITE_TYPE);
  return path;
}


list_t *make_slingshot() {
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
  list_t *rubber_list = list_init(6, free);
  
  vector_t *v = malloc(sizeof(*v));
  *v = (vector_t){127, 117};
  list_add(rubber_list, v);
  
  v = malloc(sizeof(*v));
  *v = (vector_t){127, 130};
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

// Helper function to set physics and destructive collision simultaneously
void make_collisions(scene_t *scene, size_t id) {
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        body_t *main_obj = scene_get_body(scene, i);
        size_t* info = body_get_info(main_obj);
        if (*info == id || *info == WALL_ID) {
            for (size_t j = 0; j < scene_bodies(scene); j++) {
                body_t *obj = scene_get_body(scene, j);
                size_t* info_obj = body_get_info(scene_get_body(scene, j));
                if (*info_obj==PLAT_ID || *info_obj==WALL_ID) {
                    create_physics_collision(scene, 1, obj, main_obj);
                } else if (*info_obj == PIG_ID) {
                    create_destructive_collision2(scene, obj, main_obj);
                } 
            }
        }
    }
}

void make_pigs(scene_t *scene, list_t *plat_centers) {
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

        size_t ta_idx = rand() % N_TAS;
        body_add_image(pig, make_path((char*) TA_NAMES[ta_idx]));

        scene_add_body(scene, pig);

        free(center);
    }
}

void make_bird(scene_t *scene, rgb_color_t color, vector_t center, size_t student_idx) {
  size_t* id = malloc(sizeof(size_t));
  *id = BIRD_ID;

  body_t *bird = body_init_with_info(make_circle(BIRD_RADIUS), BIRD_MASS, color, id, free);
  body_set_centroid(bird, center);
  body_add_image(bird, make_path((char*) STUDENT_NAMES[student_idx]));
  scene_add_body(scene, bird);
}

void make_speedy(scene_t *scene, rgb_color_t color, vector_t center, size_t student_idx) {
  size_t* id = malloc(sizeof(size_t));
  *id = BIRD_ID;

  body_t *bird = body_init_with_info(make_equilateral_triangle(BIRD_SPEEDY_SIDE), BIRD_MASS, color, id, free);
  body_set_centroid(bird, center);
  body_add_image(bird, make_path((char*) STUDENT_NAMES[student_idx]));
  scene_add_body(scene, bird);
}

void make_platforms(scene_t *scene, list_t *centers, size_t obj_id, rgb_color_t color) {
  for(size_t i = 0; i < list_size(centers); i++) {
    size_t* id = malloc(sizeof(size_t));
    *id = obj_id;

    int32_t length = 0;
    int32_t height = 0;
    double mass = INFINITY;

    if (obj_id == PLAT_ID) {
      length = PLAT_LENGTH;
      height = PLAT_HEIGHT;
    } else if (obj_id == WALL_ID) {
      length = WALL_LENGTH;
      height = WALL_HEIGHT;
      mass=100;
    }
    
    list_t *shape = make_rectangle(length, height);
    body_t *platform = body_init_with_info(shape, mass, color, id, free);

    vector_t *center = (vector_t*) list_get(centers, i);
    body_set_centroid(platform, *center);

    scene_add_body(scene, platform);
  }
}