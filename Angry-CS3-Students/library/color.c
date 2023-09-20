#include "color.h"
#include "test_util.h"
#include <stdio.h>
#include <stdlib.h>

rgb_color_t rand_color() {
  rgb_color_t color;

  color.r = (double)rand() / (RAND_MAX);
  color.g = (double)rand() / (RAND_MAX);
  color.b = (double)rand() / (RAND_MAX);

  return color;
}

rgb_color_t *color_init(double r, double g, double b) {
  rgb_color_t *color = malloc(sizeof(rgb_color_t));

  color->r = r;
  color->g = g;
  color->b = b;

  return color;
}
