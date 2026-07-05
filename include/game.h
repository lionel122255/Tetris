#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define FORM_SIZE 3
#define MAX_SCORE 1000
#define WINDOW_EXTRA_HEIGHT 80

enum direction_t {
    NORTH,
    EAST,
    SOUTH,
    WEST
};

enum color_t {
    GREEN,
    RED,
    BLUE,
    YELLOW,
    BLACK
};


struct location_t {
    unsigned int abs;
    unsigned int ord;

};

enum form_t {
    L,
    C,
    T,
    V,//ligne verticale
    H, //ligne horizontale
    NONE,
};




struct tile_t  {
    enum color_t tile_color;
    bool filled ;
};

struct game_t {
    struct tile_t ** grid ;
    int height;
    int width;
    enum form_t current_form;
    struct location_t form_location;
    int current_height;
    int score;
    enum form_t next_form;
    enum color_t next_color;

};


struct game_t * init_game(int height, int width);

enum form_t get_random_form();

enum color_t get_random_color();

struct location_t  move_form(struct game_t * game  , enum direction_t dir );

bool line_is_filled(struct game_t * game , int line);

bool game_is_lost(struct game_t * game);

void place_new_form(struct game_t * game , enum form_t form,enum color_t color);

void  update_height(struct game_t * game);

void clear_line(struct game_t * game, int line);

void decrease_height(struct game_t * game,int line);

bool can_move_form(struct game_t *game, enum direction_t dir);

void free_game(struct game_t * game);






#endif