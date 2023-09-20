#include "scene.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// Set some arbitrary number of bodies so that we can initialize our list
const size_t orig_bodies = 100;

typedef struct aux {
  force_creator_t force;
  void *aux;
  free_func_t freer;
  list_t *bodies;
  size_t id;
} aux_t;

typedef struct scene {
  list_t *data;
  list_t *force_creators;
} scene_t;

void aux_freer(aux_t *aux) {
  if (aux->bodies != NULL) {
    list_free2(aux->bodies);
  }
  if (aux->freer != NULL) {
    aux->freer(aux->aux);
  }
  free(aux);
}

scene_t *scene_init(void) {
  scene_t *new_scene = malloc(sizeof(scene_t));
  assert(new_scene != NULL);

  new_scene->data = list_init(orig_bodies, (free_func_t)body_free);
  new_scene->force_creators = list_init(orig_bodies, (free_func_t)aux_freer);

  return new_scene;
}

void scene_free(scene_t *scene) {
  list_free(scene->data);
  list_free(scene->force_creators);
  free(scene);
}

size_t scene_bodies(scene_t *scene) { return list_size(scene->data); }

body_t *scene_get_body(scene_t *scene, size_t index) {
  return list_get(scene->data, index);
}

void scene_add_body(scene_t *scene, body_t *body) {
  list_add(scene->data, body);
}

void scene_remove_body(scene_t *scene, size_t index) {
  body_t *body_to_remove = scene_get_body(scene, index);
  body_remove(body_to_remove);
}

void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer,
                                    void *aux, list_t *bodies,
                                    free_func_t freer, size_t id) {
  aux_t *newForce = malloc(sizeof(aux_t));
  newForce->force = forcer;
  newForce->aux = aux;
  newForce->freer = freer;
  newForce->bodies = bodies;
  newForce->id = id;

  list_add(scene->force_creators, newForce);
}

void scene_add_force_creator(scene_t *scene, force_creator_t forcer, void *aux,
                             free_func_t freer, size_t id) {
  scene_add_bodies_force_creator(scene, forcer, aux, NULL, freer, id);
}

void scene_remove_force_creator(scene_t *scene, size_t id) {
  for (size_t i = 0; i < list_size(scene->force_creators); i++){
    aux_t *force_creator = list_get(scene->force_creators, i);

    if (force_creator->id == id) {
      aux_freer(list_remove(scene->force_creators, i));
    }
  }
}

void scene_tick(scene_t *scene, double dt) {
  // execute all the force creators
  for (size_t i = 0; i < list_size(scene->force_creators); i++) {
    aux_t *force_creator = list_get(scene->force_creators, i);
    force_creator_t force = force_creator->force;
    void *force_aux = force_creator->aux;
    force(force_aux);
  }

  // Tick each body using body_tick. If any of the bodies are marked for
  // removal, then they should be removed from the scene and freed
  for (size_t i = list_size(scene->data); i > 0; i--) {
    body_t *body = list_get(scene->data, i - 1);
    if (body_is_removed(body)) {
      for (size_t j = list_size(scene->force_creators); j > 0; j--) {
        aux_t *aux = list_get(scene->force_creators, j - 1);
        list_t *bodies = aux->bodies;
        if (bodies != NULL) {
          for (size_t k = 0; k < list_size(bodies); k++) {
            if (list_get(bodies, k) == body) {
              aux_freer(list_remove(scene->force_creators, j - 1));
              break;
            }
          }
        }
      }
      body_free(list_remove(scene->data, i - 1));
    } else if (dt != 0) {
      body_tick(body, dt);
    }
  }
}
