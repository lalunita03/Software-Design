#include "body.h"
#include "polygon.h"
#include "sdl_wrapper.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

const double TRANSLATION_CONSTANT = 0.5;

typedef struct body {
  list_t *shape;
  vector_t centroid;
  vector_t velocity;
  vector_t force;
  vector_t impulse;
  rgb_color_t color;
  void *info;
  free_func_t info_freer;
  double mass;
  bool removed;
  void* image;
} body_t;

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
  body_t *new_shape = body_init_with_info(shape, mass, color, NULL, NULL);
  return new_shape;
}

body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color,
                            void *info, free_func_t info_freer) {
  body_t *new_shape = malloc(sizeof(body_t));
  assert(new_shape != NULL);

  new_shape->shape = shape;
  assert(new_shape->shape != NULL);

  assert(mass > 0); // does not make sense to have negative mass
  new_shape->mass = mass;

  vector_t force = {0.0, 0.0};
  vector_t impulse = {0.0, 0.0};
  vector_t velocity = {0.0, 0.0};

  new_shape->centroid = polygon_centroid(shape);
  new_shape->color = color;
  new_shape->velocity = velocity;
  new_shape->force = force;
  new_shape->impulse = impulse;
  new_shape->removed = false;
  new_shape->info = info;
  new_shape->info_freer = info_freer;
  new_shape->image = NULL;

  return new_shape;
}

void body_set_shape(body_t *body, list_t* shape) {
  list_free(body->shape);
  body->shape = shape;
}

void body_set_mass(body_t *body, double mass) { body->mass = mass; }
double body_get_mass(body_t *body) { return body->mass; }

list_t *body_get_shape(body_t *body) {
  list_t *copy = list_init(list_size(body->shape), free);
  for (int i = 0; i < list_size(body->shape); i++) {
    vector_t *element = list_get(body->shape, i);
    vector_t *copy_elem = malloc(sizeof(vector_t));

    copy_elem->x = element->x;
    copy_elem->y = element->y;
    list_add(copy, copy_elem);
  }

  return copy;
}

vector_t body_get_centroid(body_t *body) { return body->centroid; }

vector_t body_get_velocity(body_t *body) { return body->velocity; }

void* body_get_image(body_t *body) { return body->image; }

void body_free(body_t *body) {
  list_free(body->shape);
  if (body->info_freer != NULL) {
    body->info_freer(body->info);
  }
  if (body->image != NULL) {
    sdl_destroy_texture(body->image);
  }

  free(body);
}

rgb_color_t body_get_color(body_t *figure) { return figure->color; }

void *body_get_info(body_t *body) { return body->info; }

bool body_is_removed(body_t *body) { return body->removed; }

void body_remove(body_t *body) { body->removed = true; }

void body_set_centroid(body_t *body, vector_t x) {
  vector_t translate = vec_subtract(x, body->centroid);
  polygon_translate(body->shape, translate);
  body->centroid = x;
}

void body_set_velocity(body_t *body, vector_t v) { body->velocity = v; }

void body_set_rotation(body_t *body, double angle) {
  polygon_rotate(body->shape, angle, body->centroid);
}

void body_set_color(body_t *body, rgb_color_t *color) {
  body->color = *color;
}

void body_add_image(body_t *body, char* path) {
  body->image = sdl_get_texture(path);
}

void body_add_force(body_t *body, vector_t force) {
  body->force = vec_add(body->force, force);
}

void body_add_impulse(body_t *body, vector_t impulse) {
  body->impulse = vec_add(body->impulse, impulse);
}

void body_tick(body_t *body, double dt) {
  vector_t acceleration = vec_multiply(1 / body->mass, body->force);
  vector_t mass_impulse = vec_multiply(1 / body->mass, body->impulse);
  vector_t added_vel = vec_add(vec_multiply(dt, acceleration), mass_impulse);
  vector_t new_velocity = vec_add(body->velocity, added_vel);
  vector_t translate = vec_multiply(TRANSLATION_CONSTANT * dt,
                                    vec_add(body->velocity, new_velocity));
  vector_t centroid = vec_add(body_get_centroid(body), translate);

  body_set_centroid(body, centroid);
  body->velocity = new_velocity;

  // reset the forces and impulse
  vector_t force = {0.0, 0.0};
  body->force = force;
  vector_t impulse = {0.0, 0.0};
  body->impulse = impulse;
}
