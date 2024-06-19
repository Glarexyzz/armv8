#include <stdlib.h>
#include "ncurses.h"
#include "game_manager.h"
#include "render.h"

// int grid[GRID_SIZE][GRID_SIZE];
screen_state state;
game * curr_game;

void init() {
    if (curr_game != NULL) {
        free_game(curr_game);
    }
    //Initalizes state into in_game
    vector * startPos = new_vector(0, GRID_SIZE_Y/2);
    vector * foodPos = new_vector(GRID_SIZE_X/2, GRID_SIZE_Y/2);
    snake * start_snake = new_snake(startPos);
    start_snake->direction = right;
    food * start_food = new_food(foodPos,FOOD_SCORE);

    curr_game = create_game(start_snake, start_food, GRID_SIZE_X, GRID_SIZE_Y);
    state = in_game;
}

//Updates every GAME_TICK milliseconds
void update_tick() {
    int success = move_snake(curr_game);
    if (success != 0) {
        init();
    }
    render(curr_game);
}

//Updates every loop
void update_stepped() {
    char ch = getch();
    if (ch != ERR) {
        switch (ch)
        {
        case 'w':
            if (curr_game->snake->direction != down) {
            curr_game->snake->direction = up;
            }
            break;
        case 'a':
            if (curr_game->snake->direction != right) {
                curr_game->snake->direction = left;
            }
            break;
        case 's':
            if (curr_game->snake->direction != up) {
                curr_game->snake->direction = down;
            }
            break;
        case 'd':
            if (curr_game->snake->direction != left) {
                curr_game->snake->direction = right;
            }
            break;
        }
        ungetch(ch);
    }
}