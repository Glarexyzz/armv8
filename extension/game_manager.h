#ifndef GAME_MANAGER
#define GAME_MANAGER

#include "game.h"
#include "utils.h"

#define GRID_SIZE_X 20
#define GRID_SIZE_Y 10
#define TICK_SPEED 200 //in milliseconds

typedef enum screen_state {in_game,game_over} screen_state;

void init();

void update_tick();
void update_stepped();

#endif /* GAME_MANAGER */