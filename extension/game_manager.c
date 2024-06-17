#include <stdlib.h>
#include "game_manager.h"

// int grid[GRID_SIZE][GRID_SIZE];
game * curr_game;
void init(int grid_size) {
    vector * startPos = new_vector(0, grid_size/2);
    snake * start_snake = new_snake(startPos);

    if (curr_game != NULL) {
        free_game(curr_game);
    }
    curr_game = create_game(start_snake, grid_size);
    
}