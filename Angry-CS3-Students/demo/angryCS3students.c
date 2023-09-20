#include "body.h"
#include "color.h"
#include "collision.h"
#include "forces.h"
#include "levels.h"
#include "list.h"
#include "polygon.h"
#include "utils.h"
#include "sdl_wrapper.h"
#include <emscripten.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

// Window constants
const size_t WINDOW_W = 1000;
const size_t WINDOW_H = 500;
const vector_t MIN_POINT = {0, 0};
const vector_t MAX_POINT = {WINDOW_W, WINDOW_H};

// Pig constants
const size_t PIG_ID = 0;
const size_t N_CIRCLE_PTS = 20;
const size_t PIG_RADIUS = 10;
const double PIG_MASS = 1;
const size_t N_PIGS = 2;
const size_t PIG_TOKENS = 3;
const rgb_color_t PIG_COLOR = {0.0, 1.0, 0.0};

// Bird constants
const size_t BIRD_ID = 1;
const size_t BIRD_RADIUS = 15;
const size_t BIRD_SPEEDY_SIDE = 50;
const double SPEEDUP_FACTOR = 2.0;
const double EXPLOSION_RADIUS = 100.0;
const double BIRD_MASS = 10;
const size_t SPLIT_RAD = 10;
const rgb_color_t BIRD_STANDARD_COLOR = {1.0, 0.0, 0.0};
const rgb_color_t BIRD_EGG_COLOR = {0.75, 1.0, 0.0};
const rgb_color_t BIRD_SPLIT_COLOR = {0.0, 1.0, 1.0};
const rgb_color_t BIRD_BOMB_COLOR = {0.0, 0.0, 0.0};
const rgb_color_t BIRD_SPEEDY_COLOR = {0.5, 1.0, 0.0};
const rgb_color_t BIRD_BIG_COLOR = {0.7, 0.0, 0.0};

// Platform constants
const size_t PLAT_ID = 2;
const int32_t PLAT_LENGTH = 40;
const int32_t PLAT_HEIGHT = 5;
const int32_t WALL_LENGTH = 5;
const int32_t WALL_HEIGHT = 25;
const size_t N_PLATFORMS = 2;
const rgb_color_t PLAT_COLOR = {0.0, 0.0, 1.0};

// Dynamic Platforms constants
const rgb_color_t MOVING_PLAT = {1.0, 1.0, 220.0};
const vector_t MOVING_PLAT_VEL = {0.1, 0.1};
const rgb_color_t SLIPPERY_PLAT = {186, 219, 1.0};
const double SLIP_ACCEL = 0.5;
const double SLIP_FRICTION = 0.5;
const double TERMINAL_SLIP_SPEED = 0.3;

// Obstacle constants
const size_t OBST_ID = 3;
const int32_t OBST_LENGTH = 45;
const int32_t OBST_HEIGHT = 20;
const size_t N_OBSTACLES = 2;
const rgb_color_t OBST_COLOR = {0.0, 1.0, 1.0};

// Slingshot constants
const size_t SLING_ID = 4;
const int32_t SLING_LENGTH = 10;
const int32_t SLING_HEIGHT = 50;
const size_t BAND_ID = 5;
const int32_t BAND_HEIGHT = 8;
const rgb_color_t SLINGSHOT_COLOR = {0.588, 0.294, 0};
const rgb_color_t BAND_COLOR = {0.5, 0.5, 0.5};

// Rubberband constants
const size_t RUBBER_ID = 5;
const rgb_color_t RUBBER_COLOR = {0, 0.5, 0};
const vector_t RUBBER_CENTER = {146.0, 127.0};

// Coin constants
const size_t COIN_ID = 5;
const size_t COIN_RADIUS = 5;
const double COIN_MASS = 1;
const rgb_color_t COIN_COLOR = {1.0, 0.84, 0.0};
const double COIN_SPAWN = 20;
const size_t COIN_TOKENS = 1;

// Clock constants
const size_t CLOCK_ID = 6;
const size_t CLOCK_RADIUS = 8;
const double CLOCK_MASS = 1;
const rgb_color_t CLOCK_COLOR = {0.0, 0.0, 0.0};
const double CLOCK_SPAWN = 100;
const size_t CLOCK_FACTOR = 2;
const double CLOCK_TIMER = 30;

// SDL constants
const SDL_Color color = {0, 0, 0};
const SDL_Color passed_color = {34, 139, 34};
const SDL_Color title_color = {181, 0, 0};
const SDL_Color star_color = {255, 165, 0};

// Force constants
const size_t GRAVITY_ID = 0;
const size_t GRAVITY_CONST = 1600;

// Sprite constants
const char* SPRITE_FOLDER = "assets/";
const char* SPRITE_TYPE = ".png";
const char *STUDENT_NAMES[] = {"cloudly", "lea", "saraswati", "asav", "hopper"};
const char *TA_NAMES[] = {"alice", "sarah", "eshani", "devin", "eli", "enoch",
                          "jun", "jia", "leoJ", "leoZ", "markus", "maria",
                          "sahil", "rachael", "winter"};
const size_t N_STUDENTS = 5;
const size_t N_TAS = 15;
const char* COIN_PATH = "coin";
const char* CLOCK_PATH = "clock";
const char* BACK_PATH = "anb";

// Text constants
char *TEXT[] = {"ANGRY CS3 STUDENTS", "Start (S)", "Instructions (I)", "Choose Level",
                "1", "2", "3", "4", "5", "6", "Level Completed!", "Replay (R)", "Menu (M)", 
                "Next (N)", "Level Failed!", "Back (B)", "*", "**", "***", "Locked", "Unlocked"};

// General constants
const double ACCEL = 50;
const size_t VELOCITY = 500;
const size_t RECTANGLE_POINTS = 4;
const size_t MASS = 1;
const size_t EGG_ID = 7;
const double MAX_TIME = 240;
const size_t LEFT_SPAWN_LIMIT = 150;
const size_t WALL_ID = 8;

// Define the state and level struct
typedef struct level {
    size_t num_stars;
    size_t max_score;
    bool passed;
} level_t;

typedef struct state {
  scene_t *scene;
  
  vector_t *rubber_center;
  
  bool return_press;
  bool front_page;
  bool level_menu;
  bool level_play;
  bool instructions;
  bool passed;
  bool failed;
  bool cleaned;
  bool sequential;

  size_t prev_pigs;
  size_t prev_coins;
  size_t prev_clocks;
  size_t curr_level;

  double time;  
  double stop_clock;
  double coin_time;
  double clock_time;
  double bomb_time;
  
  size_t score;
  size_t remaining_birds;
  size_t x_scale;
  size_t y_scale;

  void* background;

  list_t *stats;

  list_t *text;
} state_t;

/* SCENE SET UP */

// Helper function to reset all constants before starting a level
void setup(state_t *state, size_t n_pigs, size_t remaining_birds) {
    state->rubber_center = malloc(sizeof(vector_t));
    state->rubber_center->x = RUBBER_CENTER.x;
    state->rubber_center->y = RUBBER_CENTER.y;
    state->prev_pigs = n_pigs;
    state->remaining_birds = remaining_birds;
    state->score = 0;
    state->stop_clock = 0.0;
    state->prev_coins = 0;
    state->prev_clocks = 0;
    state->clock_time = 0.0;
    state->coin_time = 0.0;
    state->bomb_time = 0.0;
    state->time = 0.0;

    state->level_play=true;
    state->passed=false;
    state->failed=false;
    state->level_menu = false;
    state->instructions = false;
    state->return_press = false;
    state->cleaned = false;
}

// Helper function that updates the scene when a bird leaves the screen
void wrap_scene(state_t *state) {
    body_t *bird = get_body(state->scene, BIRD_ID);
    size_t idx = get_idx(state->scene, BIRD_ID);
    vector_t vec = body_get_centroid(bird);
    if (vec.y <= 0 || vec.x >= WINDOW_W || vec.x <= 0) {
        list_t *rubberband = make_rubberband(state->scene, state->rubber_center);
        body_t *rubber = get_body(state->scene, RUBBER_ID);
        body_set_shape(rubber, rubberband);

        // remove the bird

        if (state->remaining_birds > 0) {
          scene_remove_body(state->scene, idx);
          state->remaining_birds--;

          state->return_press = false;

          if (state->remaining_birds > 0) {
              body_t *new_bird = scene_get_body(state->scene, 1);
              body_set_centroid(new_bird, RUBBER_CENTER);

              // if we are on the last bird, we cannot move any more birds up to the 
              if (state->remaining_birds > 2) {
                for (size_t i = 0; i < state->remaining_birds-1; i++) {
                  body_t *curr_bird = scene_get_body(state->scene, i+2);
                  body_set_centroid(curr_bird, vec_add(body_get_centroid(curr_bird), (vector_t){45, 0}));
                }
              }
          }
        }
    }

    // Go through all bodies in the scene and remove if their centroids are out of bounds
    for (size_t i = 0; i < scene_bodies(state->scene); i++) {
        body_t *curr_body = scene_get_body(state->scene, i);
        vector_t centroid = body_get_centroid(curr_body);
        size_t *info = body_get_info(curr_body);

        if (*info == EGG_ID) {
            if (centroid.x < 0 || centroid.x > WINDOW_W || centroid.y < 0 || centroid.y > WINDOW_H) {
            scene_remove_body(state->scene, i);
            } 
        }

        // If we collide, since we don't have angular physics implemented, we will
        // bounce off the wall, remove the wall, and then make a tiny explosion around the wall
        collision_info_t collision = find_collision(body_get_shape(bird), body_get_shape(curr_body));
        if (collision.collided == true && *info==WALL_ID) {
            scene_remove_body(state->scene, i);
            scene_tick(state->scene, 0);

            vector_t explosion_center = body_get_centroid(curr_body);

            for (size_t i = 0; i < scene_bodies(state->scene); i++) {
                body_t *body = scene_get_body(state->scene, i);
                vector_t body_position = body_get_centroid(body);
                size_t* info = body_get_info(body);

                if (vec_distance(explosion_center, body_position) <= EXPLOSION_RADIUS - 50
                    && *info != BIRD_ID && *info != PLAT_ID && *info != SLING_ID && *info != RUBBER_ID) {
                    // add explosion effect?
                    scene_remove_body(state->scene, i);
                }
            }
        } 
    }

    // Reset the center of the rubberband after shooting 
    if (body_get_centroid(bird).x >= RUBBER_CENTER.x && state->return_press == true) {
        state->rubber_center->x = RUBBER_CENTER.x;
        state->rubber_center->y = RUBBER_CENTER.y;

        list_t *rubberband = make_rubberband(state->scene, state->rubber_center);
        body_t *rubber = get_body(state->scene, RUBBER_ID);
        body_set_shape(rubber, rubberband);
    }
}

// Helper function remove all objects from the scene
void clean_scene(state_t *state) {
    for (size_t i = 0; i < scene_bodies(state->scene); i++) {
        scene_remove_body(state->scene, i);
    }
    // Scene tick so we remove the body right away, for score calculation
    scene_tick(state->scene, 0);
    printf("Total bodies: %zu \n", scene_bodies(state->scene));
}

void check_level(state_t *state) {
    printf("Checking Level: \n");
    clean_scene(state);
    printf("Scene cleaned \n");

    if (state->curr_level==1) {
        setup(state, 2, 5);
        level_one(state->scene);
    } else if (state->curr_level==2) {
        setup(state, 3, 5);
        level_two(state->scene);
    } else if (state->curr_level==3) {
        setup(state, 4, 4);
        level_three(state->scene);
    } else if (state->curr_level==4) {
        setup(state, 5, 5);
        level_four(state->scene);
    } else if (state->curr_level==5) {
        setup(state, 5, 6);
        level_five(state->scene);
    } else if (state->curr_level==6) {
        setup(state, 9, 7);
        level_six(state->scene);
    }
    size_t* rubber_id = malloc(sizeof(size_t));
    *rubber_id = RUBBER_ID;

    list_t *rubberband = make_rubberband(state->scene, state->rubber_center);
    body_t *rubberband_b = body_init_with_info(rubberband, INFINITY, RUBBER_COLOR, rubber_id, free);

    size_t* sling_id = malloc(sizeof(size_t));
    *sling_id =SLING_ID;

    list_t *slingshot = make_slingshot();
    body_t *slingshot_b = body_init_with_info(slingshot, INFINITY, SLINGSHOT_COLOR, sling_id, free);

    scene_add_body(state->scene, rubberband_b);
    scene_add_body(state->scene, slingshot_b);
}   

/* MUSIC/SOUND EFFECTS */

void play_sound(const char *filename) {
    // Load sound
    Mix_Chunk *sound = Mix_LoadWAV(filename);
    if(!sound) {
        printf("Error loading sound: %s\n", Mix_GetError());
        Mix_Quit();
        exit(EXIT_FAILURE);
    }

    // Play sound
    if(Mix_PlayChannel(-1, sound, 0) == -1) {
        printf("Error playing sound: %s\n", Mix_GetError());
        Mix_FreeChunk(sound);
        Mix_Quit();
        exit(EXIT_FAILURE);
    }
    Mix_FreeChunk(sound);
}

void play_music(const char *filename) {
    // Load music
    Mix_Music *music = Mix_LoadMUS(filename);
    if(!music) {
        printf("Error loading music: %s\n", Mix_GetError());
        Mix_Quit();
        exit(EXIT_FAILURE);
    }

    // Play music on loop
    if(Mix_PlayMusic(music, -1) == -1) {
        printf("Error playing music: %s\n", Mix_GetError());
        Mix_FreeMusic(music);
        Mix_Quit();
        exit(EXIT_FAILURE);
    }
}

/* KEY HANDLING */
void on_key_press(char key, key_event_type_t type, double held_time,
                  void *curr_state) {
    state_t *state = (state_t *)curr_state;

    if (type == KEY_PRESSED) {
        if (state->front_page) {
            if (key==S_KEY) {
                state->level_menu = true;
                state->front_page=false;
            }
            if (key==I_KEY) {
                state->front_page=false;
                state->instructions=true;
            }
        } else if (state->level_menu) {
            if (key==KEY_1) {
                printf("level 1 \n");
                state->curr_level = 1;
            } else if (key==KEY_2) {
                state->curr_level = 2;
                printf("level 2 \n");
            } else if (key==KEY_3) {
                state->curr_level = 3;
                printf("level 3 \n");
            } else if (key==KEY_4) {
                state->curr_level = 4;
                printf("level 4 \n");
            } else if (key==KEY_5) {
                state->curr_level = 5;
                printf("level 5 \n");
            } else if (key==KEY_6) {
                state->curr_level = 6;
                printf("level 6 \n");
            } else if (key==B_KEY) {
                state->level_menu = false;
                state->front_page=true;
            }

            if (state->sequential) {
                // Player must play the levels sequentially, cannot play out of order
                bool can_play = true;
                if (state->curr_level != 1) {
                    for (size_t i = 0; i < state->curr_level-1; i++) {
                        level_t *level = list_get(state->stats, i);
                        if (!level->passed) {
                            can_play=false;
                            break;
                        }
                    }
                }
                if (can_play) {
                    check_level(state);
                }
            } else {
                check_level(state);
            }
            
        } else if (state->instructions) {
            if (key==B_KEY) {
                printf("B Key \n");
                state->instructions=false;
                state->level_menu = false;
                state->front_page=true;
            }
        } else if (state->failed || (state->level_play && state->passed)) {
            if (key==M_KEY) {
                state->level_menu = true;
                state->passed     = false;
                state->level_play = false;
                state->failed=false;
            } else if (key==R_KEY) {
                check_level(state);
            } else if (key==N_KEY && state->passed && state->curr_level!=6) {
                printf("n_key_pressed \n");
                state->curr_level++;
                check_level(state);
            }
        } else if (state->level_play && !state->passed) {
            body_t *bird;

            for (size_t i = 0; i < scene_bodies(state->scene); i++) {
                size_t* info = body_get_info(scene_get_body(state->scene, i));

                if (*info == BIRD_ID) {
                    bird = scene_get_body(state->scene, i);
                    break;
                }
            }

            if (key == SPACEBAR && body_get_velocity(bird).y < 0) {
                vector_t new_vel = vec_add(body_get_velocity(bird), vec_multiply(held_time, (vector_t) {ACCEL, -ACCEL}));
                body_set_velocity(bird, new_vel);
            } 
            // Key control when we are in slingshot mode
            if (state->level_play && !state->return_press) {
                vector_t *rubber_center = state->rubber_center;
                if (key==LEFT_ARROW) {
                    rubber_center->x = rubber_center->x - 0.1*rubber_center->x*held_time;
                    rubber_center->y = rubber_center->y;
                }
                if (key==RIGHT_ARROW && rubber_center->x < RUBBER_CENTER.x) {
                    rubber_center->x = rubber_center->x + 0.1*rubber_center->x*held_time;
                    rubber_center->y = rubber_center->y;
                }
                if (key==DOWN_ARROW) {
                    rubber_center->x = rubber_center->x;
                    rubber_center->y = rubber_center->y - 0.1*rubber_center->y*held_time;
                }
                if (key==UP_ARROW) {
                    if (rubber_center->y < 250) {
                        rubber_center->x = rubber_center->x;
                        rubber_center->y = rubber_center->y + 0.1*rubber_center->y*held_time;
                    }
                }
                if (key==RETURN) {
                    state->x_scale = 0.1*(RUBBER_CENTER.x - rubber_center->x);
                    state->y_scale = 0.1*(RUBBER_CENTER.y - rubber_center->y);
                    body_set_velocity(bird, (vector_t) {state->x_scale*ACCEL, state->y_scale*ACCEL});

                    create_downward_gravity(state->scene, GRAVITY_CONST, bird, GRAVITY_ID);
                    state->return_press = true;
                }
                list_t *rubberband = make_rubberband(state->scene, rubber_center);
                body_t *rubber = get_body(state->scene, RUBBER_ID);
                body_set_shape(rubber, rubberband);

                if (state->remaining_birds > 0) {
                    body_set_centroid(bird, *(rubber_center));
                }
            }

            // If we click the S key during flight, then activate powerups
            if (key==S_KEY && state->return_press == true) {
                rgb_color_t bird_color = body_get_color(bird);

                // Speedy bird
                if (BIRD_SPEEDY_COLOR.r == bird_color.r &&
                    BIRD_SPEEDY_COLOR.g == bird_color.g &&
                    BIRD_SPEEDY_COLOR.b == bird_color.b) {
                    vector_t curr_vel = body_get_velocity(bird);
                    body_set_velocity(bird, vec_multiply(SPEEDUP_FACTOR, curr_vel));

                    create_downward_gravity(state->scene, GRAVITY_CONST, bird, GRAVITY_ID);
                    make_collisions(state->scene, BIRD_ID);
                }

                // Split bird
                if (BIRD_SPLIT_COLOR.r == bird_color.r &&
                    BIRD_SPLIT_COLOR.g == bird_color.g &&
                    BIRD_SPLIT_COLOR.b == bird_color.b) {
                    vector_t centroid = body_get_centroid(bird);
                    size_t* bird_id = malloc(sizeof(size_t));
                    *bird_id = BIRD_ID;

                    rgb_color_t *white = malloc(sizeof(rgb_color_t));
                    white->r = 1.0;
                    white->g = 1.0;
                    white->b = 1.0;

                    // Remove the original big bird
                    body_set_color(bird, white);
                    
                    // Define each of the split bodies
                    list_t *split1 = make_circle(SPLIT_RAD);
                    list_t *split2 = make_circle(SPLIT_RAD);
                    list_t *split3 = make_circle(SPLIT_RAD);

                    body_t *split1_b = body_init_with_info(split1, 1, BIRD_SPLIT_COLOR, bird_id, free);
                    body_t *split2_b = body_init_with_info(split2, 1, BIRD_SPLIT_COLOR, bird_id, free);
                    body_t *split3_b = body_init_with_info(split3, 1, BIRD_SPLIT_COLOR, bird_id, free);
                    
                    body_set_centroid(split1_b, centroid);
                    body_set_centroid(split2_b, centroid);
                    body_set_centroid(split3_b, centroid);

                    // Set a velocity for each split body            
                    body_set_velocity(split1_b, (vector_t) {1.5*state->x_scale*ACCEL, 1.5*state->y_scale*ACCEL});
                    body_set_velocity(split2_b, (vector_t) {state->x_scale*ACCEL, state->y_scale*ACCEL});
                    body_set_velocity(split3_b, (vector_t) {0.5*state->x_scale*ACCEL, 0.5*state->y_scale*ACCEL});

                    // Add gravity to each split piece
                    create_downward_gravity(state->scene, GRAVITY_CONST, split1_b, GRAVITY_ID);
                    create_downward_gravity(state->scene, GRAVITY_CONST, split2_b, GRAVITY_ID);
                    create_downward_gravity(state->scene, GRAVITY_CONST, split3_b, GRAVITY_ID);

                    // Add each split body to the scene
                    scene_add_body(state->scene, split1_b);
                    scene_add_body(state->scene, split2_b);
                    scene_add_body(state->scene, split3_b);

                    make_collisions(state->scene, BIRD_ID);
                    free(white);
                }

                // Bomb bird
                if (BIRD_BOMB_COLOR.r == bird_color.r &&
                    BIRD_BOMB_COLOR.g == bird_color.g &&
                    BIRD_BOMB_COLOR.b == bird_color.b) {
                    vector_t explosion_center = body_get_centroid(bird);
                    rgb_color_t *white = malloc(sizeof(rgb_color_t));
                    white->r = 1.0;
                    white->g = 1.0;
                    white->b = 1.0;

                    // Remove the original bird and speed it up by a lot so it gets out 
                    // of the scene faster, i.e. not much delay until next bird pops up
                    body_set_color(bird, white);
                    body_set_velocity(bird, (vector_t) {25*ACCEL, 25*ACCEL}); 

                    for (size_t i = 0; i < scene_bodies(state->scene); i++) {
                        body_t *body = scene_get_body(state->scene, i);
                        vector_t body_position = body_get_centroid(body);
                        size_t* info = body_get_info(body);

                        if (vec_distance(explosion_center, body_position) <= EXPLOSION_RADIUS 
                            && *info != BIRD_ID && *info != PLAT_ID && *info != SLING_ID && *info != RUBBER_ID) {
                            // add explosion effect?
                            scene_remove_body(state->scene, i);
                        }
                    }
                    make_collisions(state->scene, BIRD_ID);
                    free(white);
                }

                // Egg bird
                if (BIRD_EGG_COLOR.r == bird_color.r &&
                    BIRD_EGG_COLOR.g == bird_color.g &&
                    BIRD_EGG_COLOR.b == bird_color.b) {
                    vector_t centroid = body_get_centroid(bird);
                    size_t* egg_id = malloc(sizeof(size_t));
                    *egg_id = EGG_ID;

                    list_t *egg = make_circle(7);
                    body_t *egg_b = body_init_with_info(egg, 1, BIRD_EGG_COLOR, egg_id, free);
                    body_set_centroid(egg_b, centroid);
                    scene_add_body(state->scene, egg_b);

                    create_downward_gravity(state->scene, GRAVITY_CONST, egg_b, GRAVITY_ID);

                    make_collisions(state->scene, EGG_ID);
                }
            }

            // If the 'P' key is pressed, play/pause the music
            if (key == P_KEY && type == KEY_PRESSED) {
            if (Mix_PlayingMusic() == 1) {
                if (Mix_PausedMusic() == 1) {
                Mix_ResumeMusic();       
                } else {
                Mix_PauseMusic();         
                }
            }
            }
        } 
    } 
}

/* SCORE */

size_t count_same_bodies(scene_t *scene, size_t id) {
    size_t curr = 0;

    for (size_t i = 0; i < scene_bodies(scene); i++) {
      body_t *body = scene_get_body(scene, i);
      size_t* info = body_get_info(body);

      if (*info == id) {
        curr++;
      }
    }
    return curr;
}

void calculate_score(state_t *state) {
  size_t factor = 1;
  double time_left = state->stop_clock - state->time;
  if(time_left > 0.0) {
    factor = CLOCK_FACTOR;
    printf("time left: %f\n", time_left);
  }

  size_t curr_pigs = count_same_bodies(state->scene, PIG_ID);
  if (curr_pigs < state->prev_pigs) {
      state->score += (state->prev_pigs - curr_pigs) * PIG_TOKENS * factor;
      printf("hit pig\n");
      printf("Score: %zu\n", state->score);

  }
  state->prev_pigs = curr_pigs;

  size_t curr_coins = count_same_bodies(state->scene, COIN_ID);
  if (curr_coins < state->prev_coins) {
    state->score += (state->prev_coins - curr_coins) * COIN_TOKENS * factor;
    printf("hit coin\n");
    printf("Score: %zu\n", state->score);

  }
  state->prev_coins = curr_coins;

  size_t curr_clocks = count_same_bodies(state->scene, CLOCK_ID);
  if (curr_clocks < state->prev_clocks) {
    state->stop_clock = state->time + CLOCK_TIMER;
    printf("hit clock\n");
    printf("%zu\n", state->score);

  }
  state->prev_clocks = curr_clocks;
}

/* POP UPS */

void center_and_forces(scene_t *scene, body_t *pop_up) {

  vector_t rand_center = {(rand() % (WINDOW_W - LEFT_SPAWN_LIMIT)) + LEFT_SPAWN_LIMIT, (rand() % WINDOW_H)};
  body_set_centroid(pop_up, rand_center);

    for (size_t i = 0; i < scene_bodies(scene); i++) {
        body_t *body = scene_get_body(scene, i);
        size_t *info = body_get_info(body);

        if (*info == BIRD_ID) {
          create_destructive_collision2(scene, pop_up, body);
        }
    }
}

void make_coin(scene_t *scene) {
  size_t* id = malloc(sizeof(size_t));
  *id = COIN_ID;

  list_t *shape = make_circle(COIN_RADIUS);
  body_t *coin = body_init_with_info(shape, COIN_MASS, COIN_COLOR, id, free);
  center_and_forces(scene, coin);
  scene_add_body(scene, coin);
}

// void make_bomb(scene_t *scene) {
//     size_t* id = malloc(sizeof(size_t));
//     *id = BOMB_ID;

//     list_t *shape = make_circle(BOMB_RADIUS);
//     body_t *bomb = body_init_with_info(shape, BOMB_MASS, BOMB_COLOR, id, free);
//     center_and_forces(scene, bomb);
//     scene_add_body(scene, bomb);
// }


void make_clock(scene_t *scene) {
  size_t* id = malloc(sizeof(size_t));
  *id = CLOCK_ID;

  list_t *shape = make_circle(CLOCK_RADIUS);
  body_t *clock = body_init_with_info(shape, CLOCK_MASS, CLOCK_COLOR, id, free);
  center_and_forces(scene, clock);
  scene_add_body(scene, clock);
}

void update_pop_ups(state_t *state) {
    if (state->coin_time > COIN_SPAWN) {
        make_coin(state->scene);
        state->coin_time = 0.0;
    }

    if (state->clock_time > CLOCK_SPAWN) {
        make_clock(state->scene);
        state->clock_time = 0.0;
    }

    // if(state->bomb_time > BOMB_SPAWN) {
    //     make_bomb(state->scene);
    //     state->bomb_time = 0.0;
    // }
}



/* SPRITES */

void draw_sprites(scene_t *scene) {
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_t* body = scene_get_body(scene, i);
    size_t* info = body_get_info(body);

    if (*info == PIG_ID) {
      vector_t center = body_get_centroid(body);
      vector_t dim = {2 * PIG_RADIUS, 2 * PIG_RADIUS};
      vector_t corner = center;
      corner.x -= PIG_RADIUS;
      corner.y += PIG_RADIUS;
      
      sdl_render_image(scene, body_get_image(body), corner, dim);
    }
    else if (*info == BIRD_ID) {
      vector_t center = body_get_centroid(body);
      vector_t dim = {2 * BIRD_RADIUS, 2 * BIRD_RADIUS};
      vector_t corner = center;
      corner.x -= BIRD_RADIUS;
      corner.y += BIRD_RADIUS;
      
      sdl_render_image(scene, body_get_image(body), corner, dim);
    }
    else if (*info == COIN_ID) {
      vector_t center = body_get_centroid(body);
      vector_t dim = {2 * COIN_RADIUS, 2 * COIN_RADIUS};
      vector_t corner = center;
      corner.x -= COIN_RADIUS;
      corner.y += COIN_RADIUS;
      
      sdl_render_image(scene, body_get_image(body), corner, dim);
    }
    else if (*info == CLOCK_ID) {
      vector_t center = body_get_centroid(body);
      vector_t dim = {2 * CLOCK_RADIUS, 2 * CLOCK_RADIUS};
      vector_t corner = center;
      corner.x -= CLOCK_RADIUS;
      corner.y += CLOCK_RADIUS;
      
      sdl_render_image(scene, body_get_image(body), corner, dim);
    }
  }
}

// Function that initializes the state to hold the star and returns the state
state_t *emscripten_init() {
    srand(time(0));
    sdl_init(MIN_POINT, MAX_POINT);
    
    state_t *state = malloc(sizeof(state_t));
    state->scene = scene_init();
    state->front_page = true;
    state->sequential = true;
    state->background = sdl_get_texture(make_path((char*)BACK_PATH));
    
    // Initialize a list containing stats for each of the 6 levels to zero score
    // and zero stars
    state->stats = list_init(6, free);
    
    for (size_t i = 0; i < 6; i++) {
        level_t *l = malloc(sizeof(level_t));
        l->num_stars = 0;
        l->max_score = 0;
        l->passed = false;
        list_add(state->stats, l);
    }

    sdl_on_key((key_handler_t)on_key_press);
    state->time = 0.0;

    state->text = list_init(21, sdl_destroy_message);

    for(size_t i = 0; i < 21; i++) {
        list_add(state->text, (void*) build_text_object(TEXT[i]));
    }

    return state;
}

/*
 * Runs each tick of the loop and displays result on the canvas
 * Input to the function is the updated state
 */
void emscripten_main(state_t *state) {
    if (state->front_page == true) {
        sdl_clear();
        sdl_render_text(state->scene,list_get(state->text, 0), 0, 0, 1000, 200);
        sdl_render_text(state->scene,list_get(state->text, 1), 200, 300, 200, 50);
        sdl_render_text(state->scene,list_get(state->text, 2), 550, 300, 275, 50);
        sdl_show();
    } else if (state->instructions) {
        sdl_clear(); 
        void* texture = sdl_get_texture("assets/instructions-1.png");
        vector_t center = {0, 0};
        vector_t dim = {1000, 500};
        sdl_render_image(state->scene, texture, center, dim);
        sdl_show();
    } else if (state->level_menu) {
        sdl_clear();
        sdl_render_text(state->scene, list_get(state->text, 3), 250, 0, 500, 200);
        size_t orig = 100;
        
        for (size_t i = 0; i < 6; i++) {
            level_t *level = list_get(state->stats, i);
            level_t *prev_level = list_get(state->stats, i);
            if (i != 0) {
                prev_level = list_get(state->stats, i-1);
            }
            sdl_render_text(state->scene, list_get(state->text, i+4), orig+i*150, 250, 50, 50);

            if (level->num_stars == 1) {
                sdl_render_text(state->scene, list_get(state->text, 16), orig+i*150+5, 325, 25, 25);
            } else if (level->num_stars == 2) {
                sdl_render_text(state->scene, list_get(state->text, 17), orig+i*150-5, 325, 60, 30);
            } else if (level->num_stars == 3) {
                sdl_render_text(state->scene, list_get(state->text, 18), orig+i*150-10, 325, 80, 35);
            }

            if (state->sequential) {
                if (prev_level->passed || i==0) {
                    sdl_render_text(state->scene, list_get(state->text, 20), orig+i*150-35, 350, 115, 25);
                } else {
                    sdl_render_text(state->scene, list_get(state->text, 19), orig+i*150-35, 350, 115, 25);
                }
            } else {
                sdl_render_text(state->scene, list_get(state->text, 20), orig+i*150-35, 350, 115, 25);
            }
        }

        sdl_render_text(state->scene, list_get(state->text, 15), 800, 440, 150, 50);
        sdl_show();
    } 
    
    if (state->level_play && !state->failed) {
        // If we are in the middle of a game
        if (!state->passed) {
            if (state->remaining_birds == 0 && count_same_bodies(state->scene, EGG_ID) == 0) {
                // Checks if game is over
                if (state->prev_pigs > 0) {
                    state->failed = true;
                    state->return_press = true;
                    state->level_play = false;
                }
            }

            sdl_clear();
            calculate_score(state);

            if (state->time > MAX_TIME) {
                state->failed = true;
                state->return_press = true;
                state->level_play = false;
            }
            update_pop_ups(state);
            wrap_scene(state);

            double dt = time_since_last_tick();

            scene_tick(state->scene, dt);

            sdl_render_image(state->scene, state->background, (vector_t ){0, WINDOW_H}, MAX_POINT);
            sdl_render_scene(state->scene);
            draw_sprites(state->scene);


            sdl_show();

            state->time += dt;
            state->coin_time += dt;
            state->clock_time += dt;
        }

        // Check if player has passed the level
        if(state->prev_pigs == 0 && scene_bodies(state->scene)!=0) {
            printf("Passed! \n");
            state->passed = true;
            // Update the highest score
            level_t *level_stats = list_get(state->stats, state->curr_level - 1);
            level_stats->passed = true;
            if (state->score > level_stats->max_score) {
                level_stats->max_score = state->score;
            }

            size_t n_pigs = 0;
            if (state->curr_level==1) {
                n_pigs = 2;
            } else if (state->curr_level==2) {
                n_pigs = 3;
            } else if (state->curr_level==3) {
                n_pigs = 4;
            } else if (state->curr_level==4) {
                n_pigs = 5;
            } else if (state->curr_level==5) {
                n_pigs = 5;
            } else if (state->curr_level==6) {
                n_pigs = 9;
            }

            if (state->score>=PIG_TOKENS*n_pigs && state->score<=PIG_TOKENS*n_pigs+3) {
                level_stats->num_stars = 1;
            } else if (state->score>PIG_TOKENS*n_pigs+3 && state->score>PIG_TOKENS*n_pigs+6) {
                level_stats->num_stars = 2;
            } else if (state->score>=PIG_TOKENS*n_pigs+6) {
                level_stats->num_stars = 3;
            }

            if (state->curr_level==6) {
                sdl_clear();              
                sdl_render_text(state->scene, list_get(state->text, 10), 250, 0, 500, 200);
                sdl_render_text(state->scene, list_get(state->text, 11), 100, 300, 200, 50);
                sdl_render_text(state->scene, list_get(state->text, 12), 675, 300, 225, 50);
                sdl_show();
            } else {
                sdl_clear();
                sdl_render_text(state->scene, list_get(state->text, 10), 250, 0, 500, 200);
                sdl_render_text(state->scene, list_get(state->text, 11), 100, 300, 200, 50);
                sdl_render_text(state->scene, list_get(state->text, 12), 375, 300, 225, 50);
                sdl_render_text(state->scene, list_get(state->text, 13), 675, 300, 225, 50);
                sdl_show();
            }
        }
    }
    
    if (state->failed) {
        sdl_clear();
        sdl_render_text(state->scene, list_get(state->text, 14), 250, 0, 500, 200);
        sdl_render_text(state->scene, list_get(state->text, 11), 100, 300, 200, 50);
        sdl_render_text(state->scene, list_get(state->text, 12), 675, 300, 225, 50);
        sdl_show();
    }
}

// Function to free memory associated with everything
void emscripten_free(state_t *state) { 
    list_free(state->text);
    scene_free(state->scene);
    free(state->rubber_center);
    sdl_destroy_texture(state->background);
    free(state);
}
