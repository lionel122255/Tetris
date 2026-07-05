#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <stdio.h>

#include "game.h"

#define TILE_SIZE 24

struct render_t {
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Texture *green;
    SDL_Texture *red;
    SDL_Texture *blue;
    SDL_Texture *yellow;
    SDL_Texture *black;
   

    TTF_Font *font;
};

struct render_t *render_create(const char *title, int width, int height);

void render_game(struct render_t *render, struct game_t *game);

void render_destroy(struct render_t *render);

#endif