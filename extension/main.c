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
    curs_set(0);
    init();
    while (true)
    {
        delay(TICK_SPEED, update_stepped);
        update_tick();

        char ch = getch();
        if (ch == 'q') {
            //quit game
            endwin();
            return 0;
        }
        
        //clear inputs
        flushinp();
    }
    
    return 0;
}