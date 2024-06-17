#include <stdlib.h>
#include <stdio.h>
#include "game.h"

game* create_game(snake * snake, int grid_size) {
    game *newGame;
    newGame = malloc(sizeof(game));
    newGame->snake = snake;
    newGame->grid_size.x = grid_size;
    newGame->grid_size.y = grid_size;

    return newGame;
}

void free_game(game * game) {
    free(game->food);
    free_snake(game->snake);
}

tile_type check_tile(vector pos,  game * game) {
    if (vec_comp(game->food->pos, pos)) {
        return food_tile;
    } else
    if (pos.x < 0 || pos.y < 0 || pos.x > game->grid_size.x - 1 || pos.y > game->grid_size.y) {
        
    }
}

bool move_snake(game * game) {
    if (game->snake == NULL) {
        printf("Snake needs to be parented to game!");
        return false;
    }
    vector move_pos = game->snake->head->pos;

    switch (game->snake->direction)
    {
    case up:
        move_pos.y ++;
        break;
    case right:
        move_pos.x ++;
        break;
    case down:
        move_pos.y --;
        break;
    case left:
        move_pos.x --;
        break;
    }

    tile_type next_tile = check_tile(move_pos, game);

    switch (next_tile)
    {
    case empty_tile:
        //Move in direction
        snake_segment * new_segment = new_snake_segment(move_pos);
        grow_snake_from_head(game->snake,new_segment);
        remove_tail(game->snake);
        return true;
    case oob_tile:
        return false;
    case snake_tile:
        return false;
    case food_tile:
        snake_segment * new_segment = new_snake_segment(move_pos);
        grow_snake_from_head(game->snake,new_segment);
        return true;
    }
}

bool move_food(food * food) {

}