#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "game.h"
#include "render.h"
#include "utils.h"

game* create_game(snake * snake, food * food, int grid_size_x, int grid_size_y) {
    game *newGame;
    newGame = malloc(sizeof(game));
    newGame->snake = snake;
    newGame->food = food;
    newGame->grid_size.x = grid_size_x;
    newGame->grid_size.y = grid_size_y;

    return newGame;
}

void free_game(game * game) {
    free(game->food);
    free_snake(game->snake);
}

tile_type check_tile(vector pos,  game * game) {
    if (vec_comp(game->food->pos, pos)) {
        // food
        return food_tile;
    } else
    if (pos.x < 0 || pos.y < 0 || pos.x > game->grid_size.x - 1 || pos.y > game->grid_size.y - 1) {
        // out of bounds
        return oob_tile;
    }
    // Loop through snake to check
    snake_segment * curr_segment = game->snake->head;
    while (curr_segment != NULL)
    {
        if (vec_comp(curr_segment->pos, pos)) {
            // snake
            return snake_tile;
        }
        curr_segment = curr_segment->next;
    }
    // otherwise empty
    return empty_tile;
}

int move_snake(game * game) {
    vector move_pos = game->snake->head->pos;

    // y is flipped - top left is origin!
    switch (game->snake->direction)
    {
    case up:
        move_pos.y --;
        break;
    case right:
        move_pos.x ++;
        break;
    case down:
        move_pos.y ++;
        break;
    case left:
        move_pos.x --;
        break;
    }

    tile_type next_tile = check_tile(move_pos, game);

    switch (next_tile)
    {
    case food_tile:
        {
            //Eat food
            game->score += game->food->score;
            snake_segment * new_segment = new_snake_segment(&move_pos);
            bool success = move_food(game);
            grow_snake_from_head(game->snake,new_segment);
            if (success) {
                return 0;
            }
            //Victory
            return 1;
        }
    case empty_tile:
        {
            //Move in direction
            snake_segment * new_segment = new_snake_segment(&move_pos);
            grow_snake_from_head(game->snake,new_segment);
            remove_tail(game->snake);
            return 0;
        }
    default:
        return -1;
    }
}

bool move_food(game * game) {
    //Inefficient but idk
    int free_tiles = game->grid_size.x * game->grid_size.y - game->snake->length;
    if (free_tiles <= 1) {
        return false;
    }

    int food_tile = rand_in_range(1,free_tiles);

    bool moved = false;
    for (int x = 0; x < game->grid_size.x; x++) {
        for (int y = 0; y < game->grid_size.y; y++) {
            vector * potential_pos = new_vector(x, y);
            tile_type potential_tile = check_tile(*potential_pos, game);
            if (potential_tile == empty_tile) {
                food_tile = food_tile - 1;
                if (food_tile <= 0) {
                    game->food->pos = *potential_pos;
                    moved = true;
                    break;
                }
            }
            free_vector(potential_pos);
        }
        if (moved) {
            break;
        }
    }
    return true;
}