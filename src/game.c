#include "../include/game.h"
#include <stdlib.h> // Ajouté par sécurité pour malloc/rand
#include <stdbool.h>

static struct tile_t empty_tile(void)
{
    return (struct tile_t){BLACK, false};
}

struct game_t *init_game(int height, int width)
{
    struct game_t *game = malloc(sizeof(struct game_t));
    if (!game) return NULL; // Sécurité

    game->height = height;
    game->width = width;
    game->score = 0;
    game->current_form = NONE;
    game->form_location = (struct location_t){0, 0};
    game->current_height = 0;

    // Initialisation de la prochaine forme
    game->next_form = get_random_form();
    game->next_color = get_random_color();

    game->grid = malloc(height * sizeof(struct tile_t *));

    for (int i = 0; i < height; i++) {
        game->grid[i] = malloc(width * sizeof(struct tile_t));
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            game->grid[i][j] = empty_tile();
        }
    }

    return game;
}

enum form_t get_random_form()
{
    return rand() % 5;
}



bool is_inside(struct game_t *game, int y, int x)
{
    return y >= 0 && y < game->height &&
           x >= 0 && x < game->width;
}
// 1. Définit la forme géométrique exacte de la pièce pour ne pas la confondre avec le décor
bool is_cell_of_current_form(struct game_t *game, int y, int x)
{
    int old_x = game->form_location.abs;
    int old_y = game->form_location.ord;

    // Si ce n'est pas dans la boîte, ce n'est pas notre forme
    if (y < old_y || y >= old_y + FORM_SIZE ||
        x < old_x || x >= old_x + FORM_SIZE)
        return false;

    // Coordonnées relatives à l'intérieur de la boîte (de 0 à FORM_SIZE-1)
    int rel_y = y - old_y;
    int rel_x = x - old_x;

    // On vérifie le "dessin" selon la façon dont tu les as créés dans place_new_form
    switch (game->current_form) {
        case L: return (rel_y == FORM_SIZE - 1 || rel_x == 0);
        case C: return true; // Le carré remplit toute la boîte
        case T: return (rel_y == 0 || rel_x == FORM_SIZE / 2);
        case V: return (rel_x == 0);
        case H: return (rel_y == 0);
        default: return false;
    }
}

// 2. Teste les collisions UNIQUEMENT pour la forme géométrique, en ignorant le décor autour
bool can_move_form(struct game_t *game, enum direction_t dir)
{
    int dx = 0;
    int dy = 0;

    switch (dir) {
        case NORTH: dy = -1; break;
        case SOUTH: dy = 1;  break;
        case EAST:  dx = 1;  break;
        case WEST:  dx = -1; break;
    }

    int old_x = game->form_location.abs;
    int old_y = game->form_location.ord;

    for (int y = old_y; y < old_y + FORM_SIZE; y++) {
        for (int x = old_x; x < old_x + FORM_SIZE; x++) {

            // On ne check la collision QUE si cette case appartient vraiment au "dessin" de la forme
            if (is_cell_of_current_form(game, y, x) && game->grid[y][x].filled) {
                int new_y = y + dy;
                int new_x = x + dx;

                if (!is_inside(game, new_y, new_x))
                    return false;

                // S'il y a une case remplie ET qu'elle ne fait pas partie de notre propre forme
                if (game->grid[new_y][new_x].filled && !is_cell_of_current_form(game, new_y, new_x))
                    return false;
            }
        }
    }

    return true;
}

// 3. Déplace UNIQUEMENT la forme géométrique sans effacer le décor qui est dans la boîte
struct location_t move_form(struct game_t *game, enum direction_t dir)
{
    if (!can_move_form(game, dir))
        return game->form_location;

    int dx = 0;
    int dy = 0;

    switch (dir) {
        case NORTH: dy = -1; break;
        case SOUTH: dy = 1;  break;
        case EAST:  dx = 1;  break;
        case WEST:  dx = -1; break;
    }

    int old_x = game->form_location.abs;
    int old_y = game->form_location.ord;

    struct tile_t tmp[FORM_SIZE][FORM_SIZE];

    // ÉTAPE 1 : Sauvegarder uniquement la forme qui tombe, et l'effacer de la grille
    for (int y = 0; y < FORM_SIZE; y++) {
        for (int x = 0; x < FORM_SIZE; x++) {
            // Si c'est un vrai bout de notre forme...
            if (is_cell_of_current_form(game, old_y + y, old_x + x) && game->grid[old_y + y][old_x + x].filled) {
                tmp[y][x] = game->grid[old_y + y][old_x + x];
                game->grid[old_y + y][old_x + x] = empty_tile(); // ...on l'efface
            } else {
                tmp[y][x] = empty_tile(); // Sinon on ignore le décor
            }
        }
    }

    int new_x = old_x + dx;
    int new_y = old_y + dy;

    // ÉTAPE 2 : Redessiner la forme à sa nouvelle place
    for (int y = 0; y < FORM_SIZE; y++) {
        for (int x = 0; x < FORM_SIZE; x++) {
            if (tmp[y][x].filled) {
                game->grid[new_y + y][new_x + x] = tmp[y][x];
            }
        }
    }

    game->form_location = (struct location_t){new_x, new_y};

    return game->form_location;
}


// Renommé "ligne_is_filled" pour garder l'anglais homogène
bool line_is_filled(struct game_t * game, int line)
{
    for(int i = 0; i < game->width; i++){
        if(game->grid[line][i].filled == false)
            return false;
    }
    return true;
}

bool game_is_lost(struct game_t * game)
{
   return game->current_height >= game->height - FORM_SIZE &&
          game->current_form == NONE;
}

enum color_t get_random_color()
{
    return rand() % 4;
}

static void add_form(struct game_t * game, enum form_t form)
{
    game->current_form = form;
}



static void set_line(struct game_t *game, int start_y, int start_x,
                     int line, enum color_t color)
{
    for (int x = 0; x < FORM_SIZE; x++) {
        game->grid[start_y + line][start_x + x].filled = true;
        game->grid[start_y + line][start_x + x].tile_color = color;
    }
}

static void set_column(struct game_t *game, int start_y, int start_x,
                       int column, enum color_t color)
{
    for (int y = 0; y < FORM_SIZE; y++) {
        game->grid[start_y + y][start_x + column].filled = true;
        game->grid[start_y + y][start_x + column].tile_color = color;
    }
}

void place_new_form(struct game_t *game, enum form_t form, enum color_t color)
{
    int start_x = game->width / 2 - FORM_SIZE / 2;
    int start_y = 0;

    add_form(game, form);

    switch (form) {
        case L:
            set_line(game, start_y, start_x, FORM_SIZE - 1, color);
            set_column(game, start_y, start_x, 0, color);
            break;

        case C:
            for (int i = 0; i < FORM_SIZE; i++) {
                set_line(game, start_y, start_x, i, color);
            }
            break;

        case T:
            set_line(game, start_y, start_x, 0, color);
            set_column(game, start_y, start_x, FORM_SIZE / 2, color);
            break;

        case V:
            set_column(game, start_y, start_x, 0, color);
            break;

        case H:
            set_line(game, start_y, start_x, 0, color);
            break;

        default:
            break;
    }

    game->form_location = (struct location_t){start_x, start_y};
}

void update_height(struct game_t *game)
{
    int highest_filled = game->height;

    for (int i = 0; i < game->height; i++) {
        for (int j = 0; j < game->width; j++) {
            if (game->grid[i][j].filled) {
                highest_filled = i;
                game->current_height = game->height - highest_filled;
                return;
            }
        }
    }

    game->current_height = 0;
}

void clear_line(struct game_t * game, int line)
{
    for(int i = 0; i < game->width; i++){
        game->grid[line][i].filled = false;
        game->grid[line][i].tile_color = BLACK;
    }
}

void decrease_height(struct game_t *game, int line)
{
    for (int i = line; i >= 1; i--) {
        for (int j = 0; j < game->width; j++) {
            game->grid[i][j] = game->grid[i - 1][j];
        }
    }

    for (int j = 0; j < game->width; j++) {
        game->grid[0][j] = empty_tile();
    }
}

void free_game(struct game_t *game)
{
    if (game == NULL)
        return;

    if (game->grid != NULL) {
        for (int i = 0; i < game->height; i++) {
            free(game->grid[i]);
        }

        free(game->grid);
    }

    free(game);
}