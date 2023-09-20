#ifndef __UTILS_H__
#define __UTILS_H__

#include "list.h"
#include "scene.h"
#include "vector.h"
#include <stdlib.h>
#include <stdio.h>

extern const size_t N_CIRCLE_PTS;
extern const size_t PLAT_ID;
extern const size_t PIG_ID;
extern const size_t BIRD_ID;
extern const size_t WALL_ID;
extern const size_t PIG_RADIUS;
extern const size_t BIRD_RADIUS;
extern const size_t N_CIRCLE_PTS; 
extern const size_t BIRD_SPEEDY_SIDE;

extern const size_t GRAVITY_CONST;
extern const size_t GRAVITY_ID;

extern const int32_t PLAT_HEIGHT;
extern const int32_t PLAT_LENGTH;
extern const int32_t WALL_HEIGHT;
extern const int32_t WALL_LENGTH;

extern const double BIRD_MASS;
extern const double PIG_MASS;

extern const rgb_color_t PIG_COLOR;
extern const rgb_color_t PLAT_COLOR;

extern const char* SPRITE_FOLDER;
extern const char* SPRITE_TYPE;
extern const char* TA_NAMES[];
extern const char* STUDENT_NAMES[];
extern const size_t N_TAS;
extern const size_t N_STUDENTS;

/* 
    Helper function to get body in the scene based on id
 */
body_t *get_body(scene_t *scene, size_t body_id);

/* Helper function to get body in the scene based on id */
size_t get_idx(scene_t *scene, size_t body_id);

/* Helper function to make a circle given the radius and number of points
 */
list_t *make_circle(double radius);

// Helper function to construct an equilateral triangle with given side length
list_t *make_equilateral_triangle(double side_length); 

/* Function to make slingshot */
list_t *make_slingshot();

/* Function to make rubberband given scene, and center of rubberband */
list_t *make_rubberband(scene_t *scene, vector_t *center);

/* Helper function to make rectangle */
list_t *make_rectangle(int32_t length, int32_t height);

/* Helper function to set physics and destructive collision simultaneously */
void make_collisions(scene_t *scene, size_t id);

void make_speedy(scene_t *scene, rgb_color_t color, vector_t center, size_t student_idx);

void make_pigs(scene_t *scene, list_t *plat_centers);

void make_bird(scene_t *scene, rgb_color_t color, vector_t center, size_t student_idx);

void make_platforms(scene_t *scene, list_t *centers, size_t obj_id, rgb_color_t color);

char* make_path(char* name);


#endif // #ifndef __UTILS_H__
