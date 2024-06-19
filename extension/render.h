#ifndef RENDER
#define RENDER

#include "ncurses.h"
#include "game.h"

void render(game * game);

#define TOP_LEFT_GRID_CHAR ACS_ULCORNER
#define TOP_RIGHT_GRID_CHAR ACS_URCORNER
#define BOTTOM_LEFT_GRID_CHAR ACS_LLCORNER
#define BOTTOM_RIGHT_GRID_CHAR ACS_LRCORNER
#define X_WALL_GRID_CHAR ACS_HLINE
#define Y_WALL_GRID_CHAR ACS_VLINE

#endif /* RENDER */