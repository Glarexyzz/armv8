#include "snake.h"
#include "food.h"

typedef struct game {
    snake * snake;
    food * food;
    vector grid_size;
} game;

//oob - out of bounds, when the tile is outside of grid
typedef enum tile_type {empty_tile, oob_tile, snake_tile, food_tile} tile_type;

game * create_game(snake * snake, int grid_size);
game free_game(game * game);
tile_type check_tile(game);

// true if move is successful
// false if snake dead
bool move_snake(game * game);

// true if move is successful
// false if no space left
bool move_food(game * game);
