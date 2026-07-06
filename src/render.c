#include <string.h>

#include "../include/render.h"




static SDL_Texture *  load_img_texture(struct render_t * render, const char * path){
    SDL_Surface * img_surface = IMG_Load(path);
    if (img_surface == NULL) {
        fprintf(stderr, "Erreur IMG_Load %s : %s\n", path, IMG_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Texture * img_texture = SDL_CreateTextureFromSurface(render->renderer, img_surface);
    SDL_FreeSurface(img_surface);
    return img_texture;
}



struct render_t *render_create(const char *title, int width, int height){
    struct render_t * render = malloc(sizeof(struct render_t ));
    if(render == NULL){
        fprintf(stderr,"struct render_t allocation error\n");
        exit (EXIT_FAILURE);
    }

   if( SDL_Init(SDL_INIT_VIDEO)<0){
        fprintf(stderr,"SDL_Init error : %s\n",SDL_GetError());
        exit (EXIT_FAILURE);
   }

   if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG))) {
        fprintf(stderr, "Erreur IMG_Init : %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() != 0) {
        fprintf(stderr, "Erreur TTF_Init : %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_SetHint(SDL_HINT_RENDER_LOGICAL_SIZE_MODE, "stretch");

    render->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    render->renderer = SDL_CreateRenderer(
        render->window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    

    render->black = load_img_texture(render , "./ressource/blanc.png");
    render->blue = load_img_texture(render , "./ressource/blue.png");
    render->red = load_img_texture(render , "./ressource/red.png");
    render->yellow = load_img_texture(render , "./ressource/yellow.png");
    render->green = load_img_texture(render , "./ressource/green.png");
    render->hud_bg = load_img_texture(render, "./ressource/hud.png");
    render->grid_bg = load_img_texture(render, "./ressource/grid.png");
    
    
    render->font = TTF_OpenFont("./ressource/PixelOperator.ttf",24);
   

    return render;
    
}

/*Texte affiché en cas de victoire */
static void render_text_no_win(struct render_t * render ,struct game_t * game){
    char number[4];
    snprintf(number,4,"%d",game->score);
   
    char score[100] = "Score : ";
    strcat(score,number);

    // Couleur blanche pour le texte (R=255, G=255, B=255)
    SDL_Color color = {255, 255, 255, 255}; 
    SDL_Surface * ttf_surface = TTF_RenderText_Solid(render->font , score , color);
    SDL_Texture * ttf_texture = SDL_CreateTextureFromSurface(render->renderer , ttf_surface);

    SDL_Rect rect = {20, 20, ttf_surface->w, ttf_surface->h};
    
    SDL_FreeSurface(ttf_surface);
    SDL_RenderCopy(render->renderer,ttf_texture,NULL,&rect);
    SDL_DestroyTexture(ttf_texture);
}

static void render_win(struct render_t * render){
    SDL_Color color = {0,255,0,0};
    SDL_Surface * ttf_surface = TTF_RenderText_Solid(render->font , "YOU WIN !!!" , color);
    SDL_Texture * ttf_texture = SDL_CreateTextureFromSurface(render->renderer , ttf_surface);

    // En haut à gauche
    SDL_Rect rect = {20, 20, ttf_surface->w, ttf_surface->h};
    
    SDL_FreeSurface(ttf_surface);
    SDL_RenderCopy(render->renderer,ttf_texture,NULL,&rect);
    SDL_DestroyTexture(ttf_texture);
}




// 1. On crée une vraie fonction d'aide classique au-dessus
static void draw_mini_block(struct render_t *render, SDL_Texture *texture, int next_x, int start_y, int mini_tile, int x_offset, int y_offset) {
    SDL_Rect r = {
        next_x + (x_offset * mini_tile), 
        start_y + (y_offset * mini_tile), 
        mini_tile, 
        mini_tile
    };
    SDL_RenderCopy(render->renderer, texture, NULL, &r);
}

static void render_next_piece(struct render_t * render, struct game_t * game, int window_w) {
    // Couleur noire pour le texte "Next:"
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface * surface = TTF_RenderText_Solid(render->font, "Next:", color);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(render->renderer, surface);
    
    int next_x = window_w - 150;
    SDL_Rect text_rect = {next_x, 15, surface->w, surface->h}; // Remonté un peu (15 au lieu de 20)
    SDL_FreeSurface(surface);
    SDL_RenderCopy(render->renderer, texture, NULL, &text_rect);
    SDL_DestroyTexture(texture);

    SDL_Texture * block_texture = NULL;
    switch(game->next_color) {
        case GREEN: block_texture = render->green; break;
        case RED: block_texture = render->red; break;
        case YELLOW: block_texture = render->yellow; break;
        case BLUE: block_texture = render->blue; break;
        default: block_texture = render->black; break; // Le noir sera utilisé si pas de couleur
    }

    int mini_tile = 12; // RÉDUIT (au lieu de 15) pour éviter le débordement
    int start_y = 35;   // REMONTÉ (au lieu de 40) pour rester dans le HUD

    switch (game->next_form) {
        case L: 
            for(int i=0; i<3; i++) draw_mini_block(render, block_texture, next_x, start_y, mini_tile, i, 2);
            for(int i=0; i<3; i++) draw_mini_block(render, block_texture, next_x, start_y, mini_tile, 0, i);
            break;
        case C: 
            for(int i=0; i<3; i++) {
                for(int j=0; j<3; j++) draw_mini_block(render, block_texture, next_x, start_y, mini_tile, i, j);
            }
            break;
        case T: 
            for(int i=0; i<3; i++) draw_mini_block(render, block_texture, next_x, start_y, mini_tile, i, 0);
            for(int j=0; j<3; j++) draw_mini_block(render, block_texture, next_x, start_y, mini_tile, 1, j);
            break;
        case V: 
            for(int i=0; i<3; i++) draw_mini_block(render, block_texture, next_x, start_y, mini_tile, 0, i);
            break;
        case H: 
            for(int i=0; i<3; i++) draw_mini_block(render, block_texture, next_x, start_y, mini_tile, i, 0);
            break;
        default: break;
    }
}

/*
static void render_no_move_or_time(struct render_t * render, struct game_t * game){
    //AFFichage des scores
   
    int window_w;
    int window_h;

    SDL_GetWindowSize(render->window, &window_w, &window_h);

    int hud_height = 80;

    
    int tile_h = (window_h - hud_height) / game->map.height;        

    SDL_Color color = {255,255,255,0};
    SDL_Surface * ttf_surface = TTF_RenderText_Solid(render->font , "YOU FAIL !!!" , color);
    SDL_Texture * ttf_texture = SDL_CreateTextureFromSurface(render->renderer , ttf_surface);

    SDL_Rect rect = {0,game->map.height*tile_h+20,ttf_surface->w,ttf_surface->h};
    SDL_FreeSurface(ttf_surface);
    SDL_RenderCopy(render->renderer,ttf_texture,NULL,&rect);
    SDL_DestroyTexture(ttf_texture);
}*/void render_game(struct render_t *render, struct game_t *game){
    int window_w;
    int window_h;

    SDL_GetWindowSize(render->window, &window_w, &window_h);

    int hud_height = 80;

    int tile_w = window_w / game->width;
    int tile_h = (window_h - hud_height) / game->height;

    // 1. On nettoie l'écran (fond noir par défaut en cas de trou)
    SDL_SetRenderDrawColor(render->renderer, 0, 0, 0, 255);
    SDL_RenderClear(render->renderer);
    
    // --- NOUVEAU : DESSIN DES BACKGROUNDS ---
    // Background du HUD (Zone du haut)
    SDL_Rect hud_rect = {0, 0, window_w, hud_height};
    SDL_RenderCopy(render->renderer, render->hud_bg, NULL, &hud_rect);

    // Background de la zone de jeu
    SDL_Rect grid_rect = {0, hud_height, window_w, window_h - hud_height};
    SDL_RenderCopy(render->renderer, render->grid_bg, NULL, &grid_rect);
    // ----------------------------------------
    
    
    // 2. Dessin de la grille et des blocs
    for(int j=0; j<game->height; j++){
        for(int i=0; i<game->width; i++){
            
            SDL_Rect rect = {i * tile_w, hud_height + (j * tile_h), tile_w, tile_h};
            
            // On dessine le bloc coloré si la case n'est pas vide
            if (game->grid[j][i].tile_color != BLACK) {
                switch(game->grid[j][i].tile_color){
                    case GREEN:  SDL_RenderCopy(render->renderer, render->green, NULL, &rect); break;
                    case RED:    SDL_RenderCopy(render->renderer, render->red, NULL, &rect); break;
                    case YELLOW: SDL_RenderCopy(render->renderer, render->yellow, NULL, &rect); break;
                    case BLUE:   SDL_RenderCopy(render->renderer, render->blue, NULL, &rect); break;
                    default: break;
                }
            }
            
            // On dessine le CONTOUR de la case en gris clair pour voir la grille
            //SDL_SetRenderDrawColor(render->renderer, 200, 200, 200, 255); 
            //SDL_RenderDrawRect(render->renderer, &rect);
        }
    }
    
    // 3. Dessin de la ligne de séparation sous le HUD (en NOIR)
    SDL_SetRenderDrawColor(render->renderer, 0, 0, 0, 255);
    SDL_RenderDrawLine(render->renderer, 0, hud_height, window_w, hud_height);

    // 4. Affichage du HUD (Texte + Prochaine pièce)
    if(game->score < MAX_SCORE) {
        render_text_no_win(render, game);
    } else {
        render_win(render); // (Pense à changer la couleur de cette fonction en noir aussi si tu l'utilises)
    }

    render_next_piece(render, game, window_w);
    
    SDL_RenderPresent(render->renderer);
}

void render_destroy(struct render_t *render) {
    SDL_DestroyTexture(render->black);
    SDL_DestroyTexture(render->blue);
    SDL_DestroyTexture(render->red);
    SDL_DestroyTexture(render->yellow);
    SDL_DestroyTexture(render->green);
    SDL_DestroyTexture(render->hud_bg);
    SDL_DestroyTexture(render->grid_bg);

    TTF_CloseFont(render->font);

    SDL_DestroyRenderer(render->renderer);
    SDL_DestroyWindow(render->window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    free(render);
}