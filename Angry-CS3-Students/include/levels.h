#ifndef __LEVELS_H__
#define __LEVELS_H__

#include "utils.h"
#include "scene.h"

extern const vector_t RUBBER_CENTER;
extern const size_t N_OBSTACLES;
extern const size_t BIRD_ID;
extern const rgb_color_t BIRD_STANDARD_COLOR;
extern const rgb_color_t BIRD_SPLIT_COLOR;
extern const rgb_color_t BIRD_EGG_COLOR;
extern const rgb_color_t BIRD_BOMB_COLOR;
extern const rgb_color_t BIRD_SPEEDY_COLOR;
extern const rgb_color_t SLINGSHOT_COLOR;

// We have 6 levels corresponding to the 6 assignments

void level_one(scene_t *scene);

void level_two(scene_t *scene);

void level_three(scene_t *scene);

void level_four(scene_t *scene);

void level_five(scene_t *scene);

void level_six(scene_t *scene);

#endif