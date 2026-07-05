#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <time.h>

#include "../include/game.h"
#include "../include/render.h"
 


int main(){
    srand(time(NULL));
    struct game_t * game = init_game(25,25);
   

    struct render_t * render = render_create("Tetris" , game->width * TILE_SIZE , 
                                game->height*TILE_SIZE +WINDOW_EXTRA_HEIGHT);

    int running =1;
    SDL_Event event ;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Erreur SDL_mixer : %s\n", Mix_GetError());
        exit(EXIT_FAILURE);
    }

        // 1. Charger la musique
   Mix_Music *musique = Mix_LoadMUS("./ressource/hardwiredgroove.ogg");

    if (musique == NULL) {
        fprintf(stderr, "Erreur chargement musique : %s\n", Mix_GetError());
        exit(EXIT_FAILURE);
    }

    /* boucle infinie */
    Mix_PlayMusic(musique, -1);
    bool can_place_new_form = true;

    Uint32 last_fall = SDL_GetTicks();
    Uint32 fall_delay = 500;

while (running && !game_is_lost(game))
{
    if (can_place_new_form) {
        place_new_form(game,
                       game->next_form,
                       game->next_color);
        game->next_form = get_random_form();
        game->next_color = get_random_color();

        can_place_new_form = false;
    }

    while (SDL_PollEvent(&event)) {

        switch (event.type) {

            case SDL_QUIT:
                running = 0;
                break;

            case SDL_KEYDOWN:

                if (event.key.keysym.sym == SDLK_m) {

                    if (Mix_PausedMusic())
                        Mix_ResumeMusic();
                    else
                        Mix_PauseMusic();
                }

                else if (event.key.keysym.sym == SDLK_LEFT) {

                    if (can_move_form(game, WEST))
                        move_form(game, WEST);
                }

                else if (event.key.keysym.sym == SDLK_RIGHT) {

                    if (can_move_form(game, EAST))
                        move_form(game, EAST);
                }

                else if (event.key.keysym.sym == SDLK_DOWN) {

                    if (can_move_form(game, SOUTH)) {

                        move_form(game, SOUTH);

                    } else {

                        game->current_form = NONE;
                        can_place_new_form = true;
                    }
                }

                /* plus tard :
                 * SDLK_UP -> rotation
                 */

                break;

            default:
                break;
        }
    }

    /* descente automatique */

    Uint32 now = SDL_GetTicks();

    if (now - last_fall >= fall_delay) {

        if (can_move_form(game, SOUTH)) {

            move_form(game, SOUTH);

        } else {

            game->current_form = NONE;
            can_place_new_form = true;
        }

        last_fall = now;
    }

    /* suppression des lignes */

    bool cont = true;

    while (cont) {

        cont = false;

        for (int i = 0; i < game->height; i++) {

            if (line_is_filled(game, i)) {

                decrease_height(game, i);

                game->score += 10;

                cont = true;
            }
        }
    }

    update_height(game);

    render_game(render, game);

    SDL_Delay(16);
}

    Mix_FreeMusic(musique);
    Mix_CloseAudio();
    render_destroy(render);
    free_game(game);
    

    


    return 0;
}