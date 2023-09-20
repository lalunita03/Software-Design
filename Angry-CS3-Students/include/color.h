#ifndef __COLOR_H__
#define __COLOR_H__

/**
 * A color to display on the screen.
 * The color is represented by its red, green, and blue components.
 * Each component must be between 0 (black) and 1 (white).
 */
typedef struct {
  float r;
  float g;
  float b;
} rgb_color_t;

/*
 * Helper function to generate a random color
 */
rgb_color_t rand_color();

/*
 * Helper function to initialize an rgb_color_t variable given specific r, g, b
 * values. We choose to extract this out as a function since we will need to
 * initialize colors many times and it is easier if we have it as a call to a
 * function in our code instead of having a lot of code duplication.
 */
rgb_color_t *color_init(double r, double g, double b);

#endif // #ifndef __COLOR_H__
