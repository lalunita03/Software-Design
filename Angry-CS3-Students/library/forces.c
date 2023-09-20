#include "forces.h"
#include "collision.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct force {
  double constant;
  body_t *body1;
  body_t *body2;
  void *aux;
  collision_handler_t handler;
  free_func_t aux_free;
  int has_collided;
} force_t;

typedef struct drag {
  double constant;
  body_t *body;
} drag_t;

typedef struct impulse {
  double elasticity;
  body_t *body1;
  body_t *body2;
} impulse_t;

const size_t MIN_DISTANCE = 5;


void force_free(void *aux) {
  force_t *force_aux = (force_t *)aux;
  if (force_aux->aux_free != NULL) {
    force_aux->aux_free(force_aux->aux);
  }
  free(force_aux);
}

void newtonian_gravity(void *aux) {
  force_t *force_aux = (force_t *)aux;

  body_t *body1 = force_aux->body1;
  body_t *body2 = force_aux->body2;

  vector_t centroid1 = body_get_centroid(body1);
  vector_t centroid2 = body_get_centroid(body2);
  vector_t distance = vec_subtract(centroid1, centroid2);

  double G = force_aux->constant;
  double mass1 = body_get_mass(body1);
  double mass2 = body_get_mass(body2);
  double dist = sqrt(vec_dot(distance, distance));

  if (dist > MIN_DISTANCE) {
    vector_t unit_vec = vec_multiply(1 / dist, distance);
    vector_t gravity = vec_multiply(G * mass1 * mass2 / pow(dist, 2), unit_vec);
    body_add_force(body1, vec_negate(gravity));
    body_add_force(body2, gravity);
  }
}

void create_newtonian_gravity(scene_t *scene, double G, body_t *body1,
                              body_t *body2) {
  force_t *force = malloc(sizeof(force_t));
  assert(force != NULL);

  force->constant = G;
  force->body1 = body1;
  force->body2 = body2;

  list_t *bodies = list_init(2, (free_func_t)body_free);
  list_add(bodies, body1);
  list_add(bodies, body2);

  scene_add_bodies_force_creator(scene, (force_creator_t)newtonian_gravity,
                                 (void *)force, bodies, (free_func_t)free, 0);
}

void downward_gravity(void *aux) {
  force_t *force_aux = (force_t*) aux;
  
  body_t *body1 = force_aux->body1;
  double g = force_aux->constant;

  vector_t gravity = {0, g};
  body_add_force(body1, vec_negate(gravity));
}

void create_downward_gravity(scene_t *scene, double g, body_t *body1, size_t id) {
  force_t *force = malloc(sizeof(force_t));
  assert(force != NULL);

  force->constant = g;
  force->body1 = body1;

  list_t *bodies = list_init(1, (free_func_t) body_free);
  list_add(bodies, body1);

  scene_add_bodies_force_creator(scene, (force_creator_t)downward_gravity,
                                 (void *)force, bodies, (free_func_t)free, id);
}

void horizontal_friction(void *aux) {
  force_t *force_aux = (force_t *)aux;
  
  body_t *body1 = force_aux->body1;
  double friction_constant = force_aux->constant;

  vector_t velocity = body_get_velocity(body1);
  vector_t friction_force = {friction_constant * velocity.x, 0};  // assuming x is the horizontal direction

  body_add_force(body1, vec_negate(friction_force));
}

void create_horizontal_friction(scene_t *scene, double friction, body_t *body1, size_t id) {
  force_t *force = malloc(sizeof(force_t));
  assert(force != NULL);

  force->constant = friction;
  force->body1 = body1;

  list_t *bodies = list_init(1, (free_func_t) body_free);
  list_add(bodies, body1);

  scene_add_bodies_force_creator(scene, (force_creator_t)horizontal_friction,
                                 (void *)force, bodies, (free_func_t)free, id);
}


void spring(void *aux) {
  force_t *force_aux = (force_t *)aux;
  body_t *body1 = force_aux->body1;
  body_t *body2 = force_aux->body2;
  double k = force_aux->constant;

  vector_t center_b1 = body_get_centroid(body1);
  vector_t center_b2 = body_get_centroid(body2);
  vector_t displacement = vec_subtract(center_b1, center_b2);
  vector_t spring_force = vec_multiply(k, displacement);

  body_add_force(body1, vec_negate(spring_force));
  body_add_force(body2, spring_force);
}

void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2) {
  force_t *aux = malloc(sizeof(force_t));
  assert(aux != NULL);
  aux->constant = k;
  aux->body1 = body1;
  aux->body2 = body2;

  list_t *bodies = list_init(2, (free_func_t)body_free);
  list_add(bodies, body1);
  list_add(bodies, body2);

  scene_add_bodies_force_creator(scene, (force_creator_t)spring, (void *)aux,
                                 bodies, (free_func_t)free, 0);
}

void drag(void *aux) {
  drag_t *force_aux = (drag_t *)aux;
  double gamma = force_aux->constant;
  body_t *body = force_aux->body;
  vector_t velocity = body_get_velocity(body);
  vector_t drag_force = vec_multiply(-gamma, velocity);

  body_add_force(body, drag_force);
}

void create_drag(scene_t *scene, double gamma, body_t *body) {
  drag_t *aux = malloc(sizeof(drag_t));
  assert(aux != NULL);
  aux->constant = gamma;
  aux->body = body;

  list_t *bodies = list_init(1, NULL);
  list_add(bodies, body);
  scene_add_bodies_force_creator(scene, (force_creator_t)drag, (void *)aux,
                                 (list_t *)bodies, free, 0);
}

void handler_destructive_collision(body_t *body1, body_t *body2, vector_t axis,
                                   void *aux) {
  body_remove(body1);
  body_remove(body2);
}

void create_destructive_collision(scene_t *scene, body_t *body1,
                                  body_t *body2) {
  create_collision(scene, body1, body2,
                   (collision_handler_t)handler_destructive_collision, NULL,
                   NULL);
}

void handler_destructive_collision2(body_t *body1, body_t *body2, vector_t axis,
                                    void *aux) {
  body_remove(body1);
}

void create_destructive_collision2(scene_t *scene, body_t *body1,
                                   body_t *body2) {
  create_collision(scene, body1, body2,
                   (collision_handler_t)handler_destructive_collision2, NULL,
                   NULL);
}

// general force creator
void collision(void *aux) {
  force_t *force_aux = (force_t *)aux;
  body_t *body1 = force_aux->body1;
  body_t *body2 = force_aux->body2;

  list_t *shape1 = body_get_shape(body1);
  list_t *shape2 = body_get_shape(body2);

  collision_info_t collision = find_collision(shape1, shape2);
  if (collision.collided == true && force_aux->has_collided == false) {
    force_aux->handler(body1, body2, collision.axis, force_aux->aux);
    force_aux->has_collided = true;
  } else if (collision.collided == false && force_aux->has_collided == true) {
    force_aux->has_collided = false;
  }

  list_free(shape1);
  list_free(shape2);
}

void create_collision(scene_t *scene, body_t *body1, body_t *body2,
                      collision_handler_t handler, void *aux,
                      free_func_t freer) {
  force_t *collision_aux = malloc(sizeof(force_t));
  assert(collision_aux != NULL);
  collision_aux->body1 = body1;
  collision_aux->body2 = body2;
  collision_aux->aux = aux;
  collision_aux->handler = handler;
  collision_aux->aux_free = freer;
  collision_aux->has_collided = false;

  list_t *bodies = list_init(2, NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);

  scene_add_bodies_force_creator(scene, (force_creator_t)collision,
                                 (void *)collision_aux, bodies, force_free, 0);
}

void handler_physics_collision(body_t *body1, body_t *body2, vector_t axis,
                               void *aux) {
  impulse_t *impulse_aux = (impulse_t *)aux;
  double elasticity = impulse_aux->elasticity;

  double mass1 = body_get_mass(body1);
  double mass2 = body_get_mass(body2);

  vector_t vel1 = body_get_velocity(body1);
  vector_t vel2 = body_get_velocity(body2);

  double vel_comp1 = vec_dot(vel1, axis);
  double vel_comp2 = vec_dot(vel2, axis);

  double coefficient = 0;

  if (mass1 == INFINITY) {
    coefficient = mass2;
  } else if (mass2 == INFINITY) {
    coefficient = mass1;
  } else {
    coefficient = (mass1 * mass2) / (mass1 + mass2);
  }

  vector_t impulse1 = vec_multiply(
      coefficient * (1 + elasticity) * (vel_comp2 - vel_comp1), axis);

  vector_t impulse2 = vec_negate(impulse1);

  body_add_impulse(body1, impulse1);
  body_add_impulse(body2, impulse2);
}

// regsitering force creator
void create_physics_collision(scene_t *scene, double elasticity, body_t *body1,
                              body_t *body2) {
  impulse_t *impulse_aux = malloc(sizeof(impulse_t));
  assert(impulse_aux != NULL);
  impulse_aux->elasticity = elasticity;
  create_collision(scene, body1, body2,
                   (collision_handler_t)handler_physics_collision, impulse_aux,
                   free);
}

void handler_enough_collision(body_t *body1, body_t *body2, void *aux) {
  
}

void create_enough_collision(scene_t *scene, body_t *body1, body_t *body2) {

}