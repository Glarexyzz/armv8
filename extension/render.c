#include <stdlib.h>
#include "render.h"

void draw_rec(int start_x, int start_y, int width, int height) {
    // draw top and bottom sides
    for (int x_offset = 0; x_offset < width; x_offset++) {
        if (x_offset == 0) {
            mvaddch(start_y, start_x + x_offset, TOP_LEFT_GRID_CHAR);
            mvaddch(start_y + height - 1, start_x + x_offset, BOTTOM_LEFT_GRID_CHAR);
        } else
        if (x_offset == width - 1) {
            mvaddch(start_y, start_x + x_offset, TOP_RIGHT_GRID_CHAR);
            mvaddch(start_y + height - 1, start_x + x_offset, BOTTOM_RIGHT_GRID_CHAR);
        } else {
            mvaddch(start_y, start_x + x_offset, X_WALL_GRID_CHAR);
            mvaddch(start_y + height - 1, start_x + x_offset, X_WALL_GRID_CHAR);
        }
    }
    // draw left and right sides
    for (int y_offset = 1; y_offset < height - 1; y_offset++) {
        mvaddch(start_y + y_offset, start_x, Y_WALL_GRID_CHAR);
        mvaddch(start_y + y_offset, start_x + width - 1, Y_WALL_GRID_CHAR);
    }
}

void draw_snake(snake * snake, int offset_x, int offset_y) {
    snake_segment * curr_segment = snake->head;
    while (curr_segment != NULL)
    {
        mvaddch(curr_segment->pos.y + offset_y, curr_segment->pos.x + offset_x,'0');
        curr_segment = curr_segment->next;
    }
}

void draw_food(food * food, int offset_x, int offset_y) {
    mvaddch(food->pos.y + offset_y, food->pos.x + offset_x,'@');
}

void render(game * game) {
    clear();
    int grid_offset_x = 5;
    int grid_offset_y = 5;
    //origin is bottom left corner
    draw_rec(5, 5, game->grid_size.x + 2, game->grid_size.y + 2);
    draw_snake(game->snake, grid_offset_x + 1, grid_offset_y + 1);
    draw_food(game->food, grid_offset_x + 1, grid_offset_y + 1);

    refresh();
}