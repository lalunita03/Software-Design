#include "collision.h"
#include "list.h"
#include "vector.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

const size_t BIG_NUMBER = 100000;

// Helper to take the minimum and maximum values
double min_max(vector_t unit, list_t *shape, int min) {
  bool var = true;
  double min_max = -1;

  for (size_t i = 0; i < list_size(shape); i++) {
    double magnitude = vec_dot(*(vector_t *)list_get(shape, i), unit);

    if (var == true || min * magnitude < min * min_max) {
      min_max = magnitude;
      var = false;
    }
  }

  return min_max;
}

// Helper function to calculate unit vector
void unit_vectors(list_t *units, list_t *shape) {
  for (size_t i = 0; i < list_size(shape); i++) {
    vector_t point1 = *(vector_t *)list_get(shape, i);
    vector_t point2 = *(vector_t *)list_get(shape, (i + 1) % list_size(shape));

    vector_t edge = vec_subtract(point1, point2);

    double mag = 1.0 / sqrt(vec_dot(edge, edge));

    vector_t unit_edge = vec_multiply(mag, edge);
    vector_t *unit = malloc(sizeof(vector_t));
    unit->x = -unit_edge.y;
    unit->y = unit_edge.x;
    list_add(units, unit);
  }
}

double overlap(double min1, double max1, double min2, double max2) {
  double start = MAX(min1, min2);
  double end = MIN(max1, max2);
  double distance = end - start;
  return distance;
}

collision_info_t find_collision(list_t *shape1, list_t *shape2) {
  collision_info_t collision_info;

  list_t *units = list_init(
      sizeof(vector_t) * (list_size(shape1) + list_size(shape2)), free);
  unit_vectors(units, shape1);
  unit_vectors(units, shape2);

  vector_t min_unit;
  double curr_dist = BIG_NUMBER;

  for (size_t i = 0; i < list_size(units); i++) {
    vector_t unit = *(vector_t *)list_get(units, i);

    double min1 = min_max(unit, shape1, 1);
    double max1 = min_max(unit, shape1, -1);
    double min2 = min_max(unit, shape2, 1);
    double max2 = min_max(unit, shape2, -1);

    double min_dist = overlap(min1, max1, min2, max2);

    if (min_dist < curr_dist) {
      min_unit = unit;
      curr_dist = min_dist;

      if (max1 < min2 || max2 < min1) {
        collision_info.collided = false;
      } else {
        collision_info.collided = true;
        collision_info.axis = min_unit;
      }
    }
  }

  list_free(units);
  return collision_info;
}
