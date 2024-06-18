#include "utils.h"
#include "render.h"
#include "food.h"
#include "snake.h"
#include "game_manager.h"
#include "game.h"
#include "ncurses.h"

#include <stdlib.h>
#include <stdio.h>

int main () {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    nodelay(stdscr,TRUE);
    init();
    while (true)
    {
        // update_stepped();
        delay(TICK_SPEED, update_stepped);
        // update_tick();
    }
    
    return 0;
}