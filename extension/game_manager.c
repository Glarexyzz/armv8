#include <stdlib.h>
#include "ncurses.h"
#include "game_manager.h"

// int grid[GRID_SIZE][GRID_SIZE];
screen_state state;
game * curr_game;

void init(int grid_size) {
    //Initalizes state into in_game
    vector * startPos = new_vector(0, grid_size/2);
    snake * start_snake = new_snake(startPos);

    if (curr_game != NULL) {
        free_game(curr_game);
    }
    curr_game = create_game(start_snake, grid_size);
    state = in_game;
}

//Updates every GAME_TICK milliseconds
void update_tick() {
    move_snake(curr_game);
    // render(curr_game);
}

//Updates every loop
void update_stepped() {
    char ch = getch();
    if (ch != ERR) {
        printw("Key pressed! It was: %c\n", ch);
        ungetch(ch);
    }
}