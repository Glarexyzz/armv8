#ifndef GAME
#define GAME

#include "snake.h"
#include "food.h"

typedef struct game {
    snake * snake;
    food * food;
    vector grid_size;
    int score;
} game;

//oob - out of bounds, when the tile is outside of grid
typedef enum tile_type {empty_tile, oob_tile, snake_tile, food_tile} tile_type;

game * create_game(snake * snake,food * food, int grid_size_x, int grid_size_y);
void free_game(game * game);
tile_type check_tile(vector pos, game * game);

// -1 if loss
// 0 is success
// 1 is victory
int move_snake(game * game);

// true if move is successful
// false if no space left
bool move_food(game * game);

#endif /* GAME */