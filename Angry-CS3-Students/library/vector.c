#include "vector.h"
#include <math.h>
#include <stdlib.h>

// Define the functions specified in vector.h
const vector_t VEC_ZERO = {0.0, 0.0};

vector_t *vec_init(double x, double y){
  vector_t *vec = malloc(sizeof(vector_t));
  vec->x = x;
  vec->y = y;
  return vec;
}

vector_t vec_add(vector_t v1, vector_t v2) {
  v1.x = v1.x + v2.x;
  v1.y = v1.y + v2.y;

  return v1;
}

vector_t vec_subtract(vector_t v1, vector_t v2) {
  v1 = vec_add(vec_negate(v2), v1);

  return v1;
}

double vec_distance(vector_t v1, vector_t v2) {
  return sqrt(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2));
}

vector_t vec_negate(vector_t v) {
  v.x = -v.x;
  v.y = -v.y;

  return v;
}

vector_t vec_multiply(double scalar, vector_t v) {
  v.x = scalar * v.x;
  v.y = scalar * v.y;

  return v;
}

double vec_dot(vector_t v1, vector_t v2) { return v1.x * v2.x + v1.y * v2.y; }

double vec_cross(vector_t v1, vector_t v2) { return v1.x * v2.y - v1.y * v2.x; }

vector_t vec_rotate(vector_t v, double angle) {
  double newx = v.x * cos(angle) - v.y * sin(angle);
  double newy = v.x * sin(angle) + v.y * cos(angle);

  v.x = newx;
  v.y = newy;
  return v;
}