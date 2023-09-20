#include "utils.h"
#include "levels.h"

void make_birds(scene_t *scene) {
    vector_t bird_1 = (vector_t) {RUBBER_CENTER.x-30, 15};
    vector_t bird_2 = (vector_t) {RUBBER_CENTER.x-75, 15};
    vector_t bird_3 = (vector_t) {RUBBER_CENTER.x-120, 15};
    vector_t bird_4 = (vector_t) {RUBBER_CENTER.x-165, 15};

    make_bird(scene, BIRD_STANDARD_COLOR, RUBBER_CENTER, 0);
    make_bird(scene, BIRD_BOMB_COLOR, bird_1, 1);
    make_bird(scene, BIRD_SPLIT_COLOR, bird_2, 2);
    make_bird(scene, BIRD_EGG_COLOR, bird_3, 3);
    make_speedy(scene, BIRD_SPEEDY_COLOR, bird_4, 4);
}

void extra_birds(scene_t *scene) {
    vector_t bird_5 = (vector_t) {RUBBER_CENTER.x-210, 15};
    vector_t bird_6 = (vector_t) {RUBBER_CENTER.x-255, 15};
    make_bird(scene, BIRD_BOMB_COLOR, bird_5, 1);
    make_bird(scene, BIRD_STANDARD_COLOR, bird_6, 0);
}

// 5 birds
void level_one(scene_t *scene) {
    list_t *centers = list_init(2, free);

    vector_t *center = malloc(sizeof(vector_t));
    *center = (vector_t) {500, 100};
    list_add(centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {600, 400};
    list_add(centers, center);
    
    // Make the bodies
    make_birds(scene);
    make_platforms(scene, centers, PLAT_ID, PLAT_COLOR);
    make_pigs(scene, centers);

    // Go through all birds and platforms and make collisions with them
    make_collisions(scene, BIRD_ID);
    list_free(centers);
}

void level_two(scene_t *scene) {
    list_t *centers = list_init(3, free);

    vector_t *center = malloc(sizeof(vector_t));
    *center = (vector_t) {500, 100};
    list_add(centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {600, 400};
    list_add(centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {850, 250};
    list_add(centers, center); 
    
    // Make the bodies
    make_birds(scene);
    make_platforms(scene, centers, PLAT_ID, PLAT_COLOR);
    make_pigs(scene, centers);

    // Go through all birds and platforms and make collisions with them
    make_collisions(scene, BIRD_ID);
    list_free(centers);
}

void level_three(scene_t *scene) {
    list_t *plat_centers = list_init(4, free);
    list_t *wall_centers = list_init(2, free);

    // Platform coordinates
    vector_t *center = malloc(sizeof(vector_t));
    *center = (vector_t) {500, 100};
    list_add(plat_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {500, 100+2*WALL_HEIGHT+2*PLAT_HEIGHT};
    list_add(plat_centers, center); 

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {600, 400};
    list_add(plat_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {850, 250};
    list_add(plat_centers, center); 

    // Wall coordinates
    center = malloc(sizeof(vector_t));
    *center = (vector_t) {465, 100+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {535, 100+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);
    
    // Make the bodies
    make_birds(scene);
    make_platforms(scene, plat_centers, PLAT_ID, PLAT_COLOR);
    make_platforms(scene, wall_centers, WALL_ID, SLINGSHOT_COLOR);
    make_pigs(scene, plat_centers);

    // Go through all birds and platforms and make collisions with them
    make_collisions(scene, BIRD_ID);
    list_free(plat_centers);
    list_free(wall_centers);
}

void level_four(scene_t *scene) {
    list_t *plat_centers = list_init(5, free);
    list_t *wall_centers = list_init(2, free);

    // Platform coordinates
    vector_t *center = malloc(sizeof(vector_t));
    *center = (vector_t) {500, 100};
    list_add(plat_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {500, 100+2*WALL_HEIGHT+2*PLAT_HEIGHT};
    list_add(plat_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {600, 400};
    list_add(plat_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {850, 250};
    list_add(plat_centers, center); 

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {850, 250+2*WALL_HEIGHT+2*PLAT_HEIGHT};
    list_add(plat_centers, center);

    // Wall coordinates
    center = malloc(sizeof(vector_t));
    *center = (vector_t) {500-PLAT_LENGTH+WALL_LENGTH, 100+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {500+PLAT_LENGTH-WALL_LENGTH, 100+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {850-PLAT_LENGTH+WALL_LENGTH, 250+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {850+PLAT_LENGTH-WALL_LENGTH, 250+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);
    
    // Make the bodies
    make_birds(scene);
    make_platforms(scene, plat_centers, PLAT_ID, PLAT_COLOR);
    make_platforms(scene, wall_centers, WALL_ID, SLINGSHOT_COLOR);
    make_pigs(scene, plat_centers);

    // Go through all birds and platforms and make collisions with them
    make_collisions(scene, BIRD_ID);
    list_free(plat_centers);
    list_free(wall_centers);
}

void level_five(scene_t *scene) {
    list_t *plat_centers = list_init(5, free);
    list_t *wall_centers = list_init(2, free);

    // Platform coordinates
    vector_t *center = malloc(sizeof(vector_t));
    *center = (vector_t) {500, 100};
    list_add(plat_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {500, 100+2*WALL_HEIGHT+2*PLAT_HEIGHT};
    list_add(plat_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {600, 400};
    list_add(plat_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {850, 250};
    list_add(plat_centers, center); 

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {850, 250+2*WALL_HEIGHT+2*PLAT_HEIGHT};
    list_add(plat_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {700, 175};
    list_add(plat_centers, center); 

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {700, 175+2*WALL_HEIGHT+2*PLAT_HEIGHT};
    list_add(plat_centers, center);

    // Wall coordinates
    center = malloc(sizeof(vector_t));
    *center = (vector_t) {500-PLAT_LENGTH+WALL_LENGTH, 100+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {500+PLAT_LENGTH-WALL_LENGTH, 100+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {850-PLAT_LENGTH+WALL_LENGTH, 250+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {850+PLAT_LENGTH-WALL_LENGTH, 250+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);
    
    center = malloc(sizeof(vector_t));
    *center = (vector_t) {700-PLAT_LENGTH+WALL_LENGTH, 175+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {700+PLAT_LENGTH-WALL_LENGTH, 175+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);

    // Make the bodies
    make_birds(scene);
    vector_t bird_5 = (vector_t) {RUBBER_CENTER.x-210, 15};
    make_bird(scene, BIRD_BOMB_COLOR, bird_5, 1);
    make_platforms(scene, plat_centers, PLAT_ID, PLAT_COLOR);
    make_platforms(scene, wall_centers, WALL_ID, SLINGSHOT_COLOR);
    make_pigs(scene, plat_centers);

    // Go through all birds and platforms and make collisions with them
    make_collisions(scene, BIRD_ID);
    list_free(plat_centers);
    list_free(wall_centers);
}

void level_six(scene_t *scene) {
    list_t *plat_centers = list_init(8, free);
    list_t *wall_centers = list_init(2, free);

    // Platform coordinates
    vector_t *center = malloc(sizeof(vector_t));
    *center = (vector_t) {500, 100};
    list_add(plat_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {500, 100+2*WALL_HEIGHT+2*PLAT_HEIGHT};
    list_add(plat_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {500, 100+4*WALL_HEIGHT+4*PLAT_HEIGHT};
    list_add(plat_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {600, 350};
    list_add(plat_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {600, 350+2*WALL_HEIGHT+2*PLAT_HEIGHT};
    list_add(plat_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {850, 250};
    list_add(plat_centers, center); 

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {850, 250+2*WALL_HEIGHT+2*PLAT_HEIGHT};
    list_add(plat_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {700, 175};
    list_add(plat_centers, center); 

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {700, 175+2*WALL_HEIGHT+2*PLAT_HEIGHT};
    list_add(plat_centers, center);

    // Wall coordinates
    center = malloc(sizeof(vector_t));
    *center = (vector_t) {500-PLAT_LENGTH+WALL_LENGTH, 100+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {500+PLAT_LENGTH-WALL_LENGTH, 100+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {500-PLAT_LENGTH+WALL_LENGTH, 100+3*WALL_HEIGHT+3*PLAT_HEIGHT};
    list_add(wall_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {500+PLAT_LENGTH-WALL_LENGTH, 100+3*WALL_HEIGHT+3*PLAT_HEIGHT};
    list_add(wall_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {850-PLAT_LENGTH+WALL_LENGTH, 250+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {850+PLAT_LENGTH-WALL_LENGTH, 250+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);
    
    center = malloc(sizeof(vector_t));
    *center = (vector_t) {700-PLAT_LENGTH+WALL_LENGTH, 175+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {700+PLAT_LENGTH-WALL_LENGTH, 175+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {600-PLAT_LENGTH+WALL_LENGTH, 350+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);

    center = malloc(sizeof(vector_t));
    *center = (vector_t) {600+PLAT_LENGTH-WALL_LENGTH, 350+WALL_HEIGHT+PLAT_HEIGHT};
    list_add(wall_centers, center);

    // Make the bodies
    make_birds(scene);
    extra_birds(scene);
    make_platforms(scene, plat_centers, PLAT_ID, PLAT_COLOR);
    make_platforms(scene, wall_centers, WALL_ID, SLINGSHOT_COLOR);
    make_pigs(scene, plat_centers);

    // Go through all birds and platforms and make collisions with them
    make_collisions(scene, BIRD_ID);
    list_free(plat_centers);
    list_free(wall_centers);
}