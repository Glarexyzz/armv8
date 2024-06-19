#include <stdlib.h>
#include "ncurses.h"
#include "game_manager.h"
#include "render.h"

// int grid[GRID_SIZE][GRID_SIZE];
screen_state state;
game * curr_game;

void init() {
    //Initalizes state into in_game
    vector * startPos = new_vector(0, GRID_SIZE_Y/2);
    // vector * foodPos = new_vector(GRID_SIZE_X/2, GRID_SIZE_Y/2);
    vector * foodPos = new_vector(GRID_SIZE_X - 1, 0);
    snake * start_snake = new_snake(startPos);
    start_snake->direction = right;
    food * start_food = new_food(foodPos,FOOD_SCORE);

    if (curr_game != NULL) {
        free_game(curr_game);
    }
    curr_game = create_game(start_snake, start_food, GRID_SIZE_X, GRID_SIZE_Y);
    state = in_game;
}

//Updates every GAME_TICK milliseconds
void update_tick() {
    move_snake(curr_game);
    render(curr_game);
}

//Updates every loop
void update_stepped() {
    char ch = getch();
    if (ch != ERR) {
        switch (ch)
        {
        case 'w':
            curr_game->snake->direction = up;
            break;
        case 'a':
            curr_game->snake->direction = left;
            break;
        case 's':
            curr_game->snake->direction = down;
            break;
        case 'd':
            curr_game->snake->direction = right;
            break;
        }
        ungetch(ch);
    }
}